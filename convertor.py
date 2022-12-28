
from ast import Raise
from logging import raiseExceptions
import angr
import inspect
import ailment
import networkx
from typing import *
from graphviz import Digraph
from ir import *
from archx64 import *
_PEEPHOLE_OPTIMIZATIONS_TYPE = Optional[Iterable[Union[Type['PeepholeOptimizationStmtBase'],Type['PeepholeOptimizationExprBase']]]]

def get_imm_size(value):
    if value >= -128 and value <= 255:
        return 1
    elif value >= -32768 and value <= 65535:
        return 2
    elif value >= -2147483648 and value <= 4294967295:
        return 4
    elif value >= -9223372036854775808 and value <= 18446744073709551615:
        return 8
    else:
        raise Exception("Overflow!")   

def get_insn_size(insn_list):
    result = 0
    for insn in insn_list:
        result += len(insn.encode())
    return result


CMP_ID_MAP = {
    'CmpEQ': 0,
    'CmpNE': 1,
    'CmpLT': 2,
    'CmpGE': 3,
    'CmpLE': 4,
    'CmpGT': 5,
    'CmpLTs': 6,
    'CmpGEs': 7,
    'CmpLEs': 8,
    'CmpGTs': 9,
}

def get_byte_len(bit_len):
    assert bit_len % 8 == 0 or bit_len == 1
    x = bit_len // 8
    if x == 0:
        return 1
    return x

class CodeConverter:

    def __init__(self, filepath, load_base : 0x400000, peephole_optimizations: _PEEPHOLE_OPTIMIZATIONS_TYPE = None, remove_dead_memdefs = True, stack_pointer_tracker = None):
        self.project = angr.Project(filepath, auto_load_libs = False, load_options = {
        'main_opts' : {
            'base_addr' : load_base,
            'force_rebase' : True
        }
        })
        self.cfg = self.project.analyses.CFGFast(normalize = True)
        
        self._peephole_optimizations = peephole_optimizations
        self._remove_dead_memdefs = remove_dead_memdefs
        self._stack_pointer_tracker = stack_pointer_tracker
        self.obfu_passes = []
        self.opcode = b''
        self.func_graph = None

    def insert_obfuscator(self, obfu_pass):
        self.obfu_passes.append(obfu_pass)
    
    def process(self, func_addr):
        self.func_graph = self._build_ail_graph(func_addr)
        self._replace_single_target_indirect_transitions(self.func_graph)
        self.func_graph = self._simplify_blocks(func_addr, self.func_graph)
        self._dump_ail_graph('test', self.func_graph)
        self.opcode = self._translate_into_opcode(self.func_graph)

        dec = self.project.analyses.Decompiler(self.cfg.kb.functions[func_addr], cfg = self.cfg.model)
        code = dec.codegen.text
        print(code)
        # for i in opcode:
        #    print('0x%02x, '%i,end = '')

    def get_opcode(self):
        return self.opcode

    def get_func_graph(self):
        return self.func_graph

    def _translate_into_opcode(self, func_graph):
        prev = 0
        all_insn = []
        addr_to_fix = []
        entry_insn_map = {}
        rev_entry_insn_map = {}
        for block in func_graph.nodes():
            if block.addr > prev:
                prev = block.addr
            else:
                raise Exception("WTF?")
            insns = self._translate_basicblock(block, addr_to_fix)
            all_insn += insns
            entry_insn_map[block.addr] = insns[0]
            rev_entry_insn_map[insns[0]] = block.addr
        addr = 0
        addr_map = {}
        for insn in all_insn:
            addr_map[insn] = addr
            addr += len(insn.encode())
            
        assert get_imm_size(addr) <= 4

        for tup in addr_to_fix:
            real_addr = tup[0]
            insn_to_fix = tup[1]
            assert isinstance(insn_to_fix, PushImm)
            insn_len = len(insn_to_fix.encode())
            offset = addr_map[entry_insn_map[real_addr]] - addr_map[insn_to_fix] - insn_len 
            insn_to_fix.set_imm(4, offset)
        
        result = b''
        for insn in all_insn:
            result += insn.encode()
            if insn in rev_entry_insn_map.keys():
                print()
                print('# block 0x%X' % rev_entry_insn_map[insn])
            print(('%04X' % addr_map[insn]) + '  ' + insn.dump())
        
        return result

    def _translate_basicblock(self, basic_block, addr_to_fix : list):
        block_insn = []
        # reset_tmp_map()
        for stmt in basic_block.statements:
            insns = []
            if isinstance(stmt, ailment.statement.Assignment):
                insn = []
                if isinstance(stmt.dst, ailment.expression.Register):
                    insn = self._translate_expression(stmt.src)
                    # print(stmt.dst.reg_name,stmt.dst.reg_offset)
                    insn.append(PopReg(get_reg(stmt.dst.reg_name)))
                elif isinstance(stmt.dst, ailment.expression.Tmp):
                    insn = self._translate_expression(stmt.src)
                    insn.append(StoreTmp(get_byte_len(stmt.dst.bits), stmt.dst.tmp_idx))
                else:
                    print('[!] unsupported stmt, ignored..')
                    continue
                    # raise Exception("Unsupported stmt!")
                insns += insn

            elif isinstance(stmt, ailment.statement.Call):
                print('[!] call inst not support, ignored..')
                pass
                # insns.append(Call(stmt.target))
                # raise Exception("Unsupported stmt!")

            elif isinstance(stmt, ailment.statement.ConditionalJump):
                assert isinstance(stmt.true_target, ailment.expression.Const) and isinstance(stmt.false_target, ailment.expression.Const)
                insns += self._translate_expression(stmt.condition)
                true_val = stmt.true_target.value
                false_val = stmt.false_target.value
                insns.append(PushImm(4, 0xdeadbeef))
                addr_to_fix.append((true_val, insns[-1]))
                insns.append(CondBranch())
                insns.append(PushImm(4, 0xdeadbeef))
                addr_to_fix.append((false_val, insns[-1]))
                insns.append(Branch())
                
            elif isinstance(stmt, ailment.statement.Jump):
                assert isinstance(stmt.target, ailment.expression.Const)
                insns.append(PushImm(4, 0xdeadbeef))
                addr_to_fix.append((stmt.target.value, insns[-1]))
                insns.append(Branch())

            elif isinstance(stmt, ailment.statement.Return):
                insns.append(Hlt())

            elif isinstance(stmt, ailment.statement.Store):
                addr = self._translate_expression(stmt.addr)
                insns += addr
                insns.append(PushImm(get_imm_size(stmt.size), stmt.size))
                value = self._translate_expression(stmt.data)
                insns += value
                insns.append(StoreMem())

            else:
                raise Exception("Not supported")
            
            block_insn += insns
        

        return block_insn

    def _translate_expression(self, expression):
        
        emit_insn = []
        def visitor(cur_expr, insn : list):
            assert isinstance(cur_expr, ailment.Expression)
            if isinstance(cur_expr, ailment.expression.Const):
                insn.append(PushImm(get_imm_size(cur_expr.value), cur_expr.value))

            elif isinstance(cur_expr, ailment.expression.Register):
                # print(cur_expr.reg_name,cur_expr.reg_offset)
                # print(cur_expr.reg_name)
                insn.append(PushReg(get_reg(cur_expr.reg_name)))
            
            elif isinstance(cur_expr, ailment.expression.Load):
                visitor(cur_expr.addr, insn)
                insn.append(PushImm(get_imm_size(cur_expr.size), cur_expr.size))
                insn.append(LoadMem())


            elif isinstance(cur_expr, ailment.expression.Convert):
                if cur_expr.to_bits > 128:
                    raise Exception("Unsupported size")
                visitor(cur_expr.operand, insn)
                if cur_expr.to_bits < cur_expr.from_bits:    
                    mask = (1 << (cur_expr.to_bits)) - 1
                    insn.append(PushImm(get_byte_len(cur_expr.to_bits), mask))
                    insn.append(And(8))

                else:
                    if cur_expr.is_signed:
                        insn.append(Sext(get_byte_len(cur_expr.from_bits), get_byte_len(cur_expr.to_bits)))
                    else:
                        insn.append(Zext(get_byte_len(cur_expr.from_bits), get_byte_len(cur_expr.to_bits)))
                    
            elif isinstance(cur_expr, ailment.expression.BinaryOp):
                bit_len = cur_expr.bits
                mask = (1 << (bit_len)) - 1
                if cur_expr.op == 'Add':
                    assert cur_expr.signed == False
                    visitor(cur_expr.operands[0], insn)
                    visitor(cur_expr.operands[1], insn)
                    insn.append(Add(get_byte_len(cur_expr.bits)))

                elif cur_expr.op == 'Sub':
                    assert cur_expr.signed == False
                    visitor(cur_expr.operands[0], insn)
                    visitor(cur_expr.operands[1], insn)
                    insn.append(Sub(get_byte_len(cur_expr.bits)))

                elif cur_expr.op == 'Mul':
                    visitor(cur_expr.operands[0], insn)
                    visitor(cur_expr.operands[1], insn)
                    insn.append(Mul(get_byte_len(cur_expr.bits), cur_expr.signed))

                elif cur_expr.op == 'Div':
                    visitor(cur_expr.operands[0], insn)
                    visitor(cur_expr.operands[1], insn)
                    insn.append(Div(get_byte_len(cur_expr.bits), cur_expr.signed))

                elif cur_expr.op == 'DivMod':
                    bit_len = get_byte_len(cur_expr.bits)
                    visitor(cur_expr.operands[0], insn)
                    visitor(cur_expr.operands[1], insn)
                    insn.append(Mod(bit_len, cur_expr.signed))
                    insn.append(PushImm(1, cur_expr.bits // 2))
                    insn.append(Shl(bit_len))
                    visitor(cur_expr.operands[0], insn)
                    visitor(cur_expr.operands[1], insn)
                    insn.append(Div(bit_len, cur_expr.signed))
                    insn.append(Or(bit_len))
                    # use dup to opt...

                elif cur_expr.op == 'Xor':
                    assert cur_expr.signed == False
                    visitor(cur_expr.operands[0], insn)
                    visitor(cur_expr.operands[1], insn)
                    insn.append(Xor(get_byte_len(cur_expr.bits)))

                elif cur_expr.op == 'And':
                    assert cur_expr.signed == False
                    visitor(cur_expr.operands[0], insn)
                    visitor(cur_expr.operands[1], insn)
                    insn.append(And(get_byte_len(cur_expr.bits)))

                elif cur_expr.op == 'LogicalAnd':
                    assert cur_expr.signed == False
                    visitor(cur_expr.operands[0], insn)
                    visitor(cur_expr.operands[1], insn)
                    insn.append(And(get_byte_len(cur_expr.bits)))
                    insn.append(PushImm(1, 1))
                    insn.append(And(get_byte_len(cur_expr.bits)))
                
                elif cur_expr.op == 'Or':
                    assert cur_expr.signed == False
                    visitor(cur_expr.operands[0], insn)
                    visitor(cur_expr.operands[1], insn)
                    insn.append(Or(get_byte_len(cur_expr.bits)))
                
                elif cur_expr.op == 'LogicalOr':
                    assert cur_expr.signed == False
                    visitor(cur_expr.operands[0], insn)
                    visitor(cur_expr.operands[1], insn)
                    insn.append(Or(get_byte_len(cur_expr.bits)))
                    insn.append(PushImm(1, 1))
                    insn.append(And(get_byte_len(cur_expr.bits)))

                elif cur_expr.op == 'Shl':
                    assert cur_expr.signed == False
                    visitor(cur_expr.operands[0], insn)
                    visitor(cur_expr.operands[1], insn)
                    insn.append(Shl(get_byte_len(cur_expr.bits)))
                
                elif cur_expr.op == 'Shr':
                    assert cur_expr.signed == False
                    visitor(cur_expr.operands[0], insn)
                    visitor(cur_expr.operands[1], insn)
                    insn.append(Shr(get_byte_len(cur_expr.bits)))
                
                elif cur_expr.op == 'Sar':
                    assert cur_expr.signed == False
                    visitor(cur_expr.operands[0], insn)
                    visitor(cur_expr.operands[1], insn)
                    insn.append(Sar(get_byte_len(cur_expr.bits)))

                elif 'Cmp' in cur_expr.op:
                    visitor(cur_expr.operands[0], insn)
                    visitor(cur_expr.operands[1], insn)
                    cmp_size = max(cur_expr.operands[0].bits, cur_expr.operands[1].bits)
                    insn.append(Cmp(CMP_ID_MAP[cur_expr.op] ,get_byte_len(cmp_size)))

                elif cur_expr.op == 'Concat':
                    assert cur_expr.signed == False
                    assert cur_expr.bits <= 128
                    visitor(cur_expr.operands[0], insn)
                    shift = cur_expr.operands[1].bits
                    insn.append(PushImm(get_imm_size(shift), shift))
                    insn.append(Shl(get_byte_len(cur_expr.bits)))
                    visitor(cur_expr.operands[1], insn)
                    insn.append(Or(get_byte_len(cur_expr.bits)))
                
                else:
                    raise Exception("Unsupported binop!")

            elif isinstance(cur_expr, ailment.expression.UnaryOp):
                if cur_expr.op == 'Not':
                    insn.append(Not(get_byte_len(cur_expr.bits)))
                else:
                    raise Exception("Unsupported unop!")

            elif isinstance(cur_expr, ailment.expression.ITE):
                print(cur_expr.cond)
                visitor(cur_expr.cond, insn)
                cmp_br = PushImm(4, 0xdeadbeef)
                insn.append(cmp_br)
                insn.append(CondBranch())
                false_insn = []
                visitor(cur_expr.iffalse, false_insn)
                true_insn = []
                visitor(cur_expr.iftrue, true_insn)
                false_size = get_insn_size(false_insn)
                true_size = get_insn_size(true_insn)


                insn += false_insn
                goto_br = PushImm(4, 0xdeadbeef)
                insn.append(goto_br)
                insn.append(Branch())
                insn += true_insn

                op_len = get_imm_size(true_size)
                goto_br.set_imm(op_len, true_size)

                false_size += len(goto_br.encode()) + len(Branch().encode())
                op_len = get_imm_size(false_size)
                cmp_br.set_imm(op_len, false_size)


            elif isinstance(cur_expr, ailment.expression.Tmp):
                bit_len = get_byte_len(cur_expr.bits)
                insn.append(LoadTmp(bit_len, cur_expr.tmp_idx))

            else:
                print(cur_expr)
                print('[!] unsupported stmt, ignored..')
                insn.append(PushImm(1, 0))
                # raise Exception("Not supported yet!")
            pass

        visitor(expression, emit_insn)
        
        # print(runtime_stack)
        #print(emit_insn)

        return emit_insn

    def _simplify_blocks(self, func_addr, func_graph):
        result_mapping = {}

        for ail_block in func_graph.nodes():
            simp = self.project.analyses.AILBlockSimplifier(
                ail_block,
                func_addr,
                remove_dead_memdefs = self._remove_dead_memdefs,
                peephole_optimizations = self._peephole_optimizations,
                stack_pointer_tracker = self._stack_pointer_tracker
            )
            result_mapping[ail_block] = simp.result_block

        new_graph = networkx.DiGraph()
        for n in func_graph.nodes():
            new_graph.add_node(result_mapping[n])

        for n in func_graph.nodes():
            for m in func_graph.successors(n):
                new_graph.add_edge(result_mapping[n], result_mapping[m])
        return new_graph
        

    def _dump_ail_graph(self, name, graph):
        g = Digraph(name)
        for n in graph.nodes():
            g.node(hex(n.addr), label = str(n).replace('\n', '\l') + '\l', shape = "polygon", style = 'rounded')

        for src, dst in graph.edges():
            g.edge(hex(src.addr), hex(dst.addr))
        g.view()

    def _replace_single_target_indirect_transitions(self, ail_graph: networkx.DiGraph) -> networkx.DiGraph:
        if self.cfg is None:
            return ail_graph

        for block in ail_graph.nodes():
            if not block.statements:
                continue

            last_stmt = block.statements[-1]
            if isinstance(last_stmt, ailment.Stmt.Call) and not isinstance(last_stmt.target, ailment.Expr.Const):
                node = self.cfg.get_any_node(block.addr)
                if node is None:
                    continue

                successors = self.cfg.get_successors(node, excluding_fakeret = True, jumpkind = 'Ijk_Call')
                if len(successors) == 1 and not isinstance(self.project.hooked_by(successors[0].addr), angr.procedures.stubs.UnresolvableCallTarget):
                    new_last_stmt = last_stmt.copy()
                    new_last_stmt.target = ailment.Expr.Const(None, None, successors[0].addr, last_stmt.target.bits)
                    block.statements[-1] = new_last_stmt

            elif isinstance(last_stmt, ailment.Stmt.Jump) and not isinstance(last_stmt.target, ailment.Expr.Const):
                node = self.cfg.get_any_node(block.addr)
                if node is None:
                    continue

                successors = self.cfg.get_successors(node, excluding_fakeret = True, jumpkind = 'Ijk_Boring')
                if len(successors) == 1 and not isinstance(self.project.hooked_by(successors[0].addr), angr.procedures.stubs.UnresolvableJumpTarget):
                    new_last_stmt = last_stmt.copy()
                    new_last_stmt.target = ailment.Expr.Const(None, None, successors[0].addr, last_stmt.target.bits)
                    block.statements[-1] = new_last_stmt

        return ail_graph


    def _build_ail_graph(self, func_addr) -> networkx.DiGraph:
        edges = {}
        graph = networkx.DiGraph()

        func = self.cfg.kb.functions[func_addr]
        manager = ailment.Manager(arch = self.project.arch)
        block_to_addr_map = {}
        
        sorted_addrs = sorted(func.block_addrs)

        for idx in range(len(sorted_addrs)):
            cur_addr = sorted_addrs[idx]
            
            node = self.cfg.get_any_node(cur_addr)
            irsb = node.block.vex

            converter = ailment.IRSBConverter()
            ail_block = converter.convert(irsb, manager)
            
            graph.add_node(ail_block)
            block_to_addr_map[cur_addr] = ail_block
            
            last_stmt = ail_block.statements[-1]
            succ = []

            if isinstance(last_stmt, ailment.statement.Call) and idx != len(sorted_addrs) - 1:
                succ.append((sorted_addrs[idx + 1]))
            elif isinstance(last_stmt, ailment.statement.Return):
                pass
            else:
                for bb in node.successors:
                    if bb.addr not in sorted_addrs:
                        print(bb.addr)
                        print(sorted_addrs)
                        raise Exception("WTF!")
                        continue
                    succ.append(bb.addr)
            
            edges[cur_addr] = succ

        for src, value in edges.items():
            for dst in value:
                graph.add_edge(block_to_addr_map[src], block_to_addr_map[dst])
        return graph
        


