
from enum import Enum
import math
class OpType:
    PUSH_REG = 0
    LOAD_MEM = 1        # addr, size
    PUSH_IMM = 3
    STORE_MEM = 4       # addr, size, value
    POP_REG = 5         # value
    ADD = 6             # a, b
    AND = 7             # a, b
    CMP = 8             # a, b
    DIV = 9             # a, b
    MOD = 10             # a, b
    MUL = 11             # a, b
    OR = 12             # a, b
    SAR = 13             # a, b
    SHL = 14             # a, b
    SHR = 15             # a, b
    SUB = 16             # a, b
    XOR = 17             # a, b
    NEG = 18             # a, b
    NOT = 19             # a, b
    HLT = 20             # a, b
    CALL = 21
    BRANCH = 22
    SEXT = 24
    ZEXT = 25
    STORE_TMP = 26             # value
    LOAD_TMP = 27
    COND_BRANCH = 28             # cond, 

class OperandType:
    REGISTER = 0
    CONSTANT1 = 1
    CONSTANT2 = 2
    CONSTANT4 = 3
    CONSTANT8 = 4
    CONSTANT16 = 4
    SPECIAL = 5



def get_operand_type_size(op_type):
    if op_type == OperandType.REGISTER or op_type == OperandType.CONSTANT1 or op_type == OperandType.SPECIAL:
        return 1
    elif op_type == OperandType.CONSTANT8:
        return 8
    elif op_type == OperandType.CONSTANT2:
        return 2
    elif op_type == OperandType.CONSTANT4:
        return 4
    elif op_type == OperandType.CONSTANT16:
        return 16
    else:
        raise Exception("Unknown operand type")

def get_operand_type(size):
    if size == 1:
        return OperandType.CONSTANT1
    elif size == 2:
        return OperandType.CONSTANT2
    elif size == 4:
        return OperandType.CONSTANT4
    elif size == 8:
        return OperandType.CONSTANT8
    elif size == 16:
        return OperandType.CONSTANT16
    else:
        raise Exception("Unsupported size")

class Insn:
    def __init__(self, optype, opnum, opsize = 0, signed = False):
        assert opsize == 0 or opsize == 1 or opsize == 2 or opsize == 4 or opsize == 8 or opsize == 16
        assert opnum < 2
        self.optype = optype
        self.opnum = opnum
        self.opsize = opsize
        self.operands = []
        self.signed = signed

    def add_operand(self, opnd_type, value):
        if len(self.operands) > self.opnum:
            raise Exception("Too many operands")
        self.operands.append([opnd_type, value])
        return self

    def _encode_opcode(self) -> bytes:
        value = (self.optype << 3)
        if self.opsize != 0 :
            value += (int(math.log2(self.opsize)))
        assert value < 256
        return int.to_bytes(value, 1, byteorder = 'little', signed = False)

    def _get_all_opcode(self) -> list:
        orig = self.opsize
        if self.opsize == 0:
            return [ord(self._encode_opcode())]
        result = []
        self.opsize = 1
        result.append(ord(self._encode_opcode()))
        self.opsize = 2
        result.append(ord(self._encode_opcode()))
        self.opsize = 4
        result.append(ord(self._encode_opcode()))
        self.opsize = 8
        result.append(ord(self._encode_opcode()))
        self.opsize = 16
        result.append(ord(self._encode_opcode()))
        self.opsize = orig
        return result


    def _encode_operands(self) -> bytes:
        result = b''

        for op in self.operands:
            result += int.to_bytes(op[1], get_operand_type_size(op[0]), byteorder = 'little', signed = op[1] < 0)
        return result
        
    def _get_operand(self, idx):
        return self.operands[idx]
        
    def dump(self):
        opname = self.__class__.__name__.split('.')[-1]
        result = opname
        for op in self.operands:
            opnd_type = op[0]
            value = op[1]
            if opnd_type == OperandType.REGISTER:
                result += ' reg[' + hex(value) + '],'
            elif  opnd_type == OperandType.SPECIAL:
                result += ' flag=[' + hex(value) + '],'
            else:
                result += ' ' + hex(value) + ', '
        if self.opsize != 0:
            result += ' size=' + str(self.opsize)
        return result

    def encode(self) -> bytes:
        assert len(self.operands) == self.opnum
        return self._encode_opcode() + self._encode_operands()

class PushReg(Insn):
    def __init__(self, reg_idx):
        super(self.__class__, self).__init__(OpType.PUSH_REG, 1)
        self.add_operand(OperandType.REGISTER, reg_idx)

    
class LoadMem(Insn):
    def __init__(self):
        super(self.__class__, self).__init__(OpType.LOAD_MEM, 0)

class PushImm(Insn):
    def __init__(self, byte_size, value):
        super(self.__class__, self).__init__(OpType.PUSH_IMM, 1, byte_size)
        self.add_operand(get_operand_type(byte_size), value)

    def set_imm(self, byte_size, value):
        super(self.__class__, self).__init__(OpType.PUSH_IMM, 1, byte_size)
        self.add_operand(get_operand_type(byte_size), value)

class StoreMem(Insn):
    def __init__(self):
        super(self.__class__, self).__init__(OpType.STORE_MEM, 0)

class PopReg(Insn):
    def __init__(self, reg_idx):
        super(self.__class__, self).__init__(OpType.POP_REG, 1)
        self.add_operand(OperandType.REGISTER, reg_idx)

class Add(Insn):
    def __init__(self, byte_size):
        super(self.__class__, self).__init__(OpType.ADD, 0, byte_size)


class And(Insn):
    def __init__(self, byte_size):
        super(self.__class__, self).__init__(OpType.AND, 0, byte_size)
        
class Cmp(Insn):
    def __init__(self, cmp_type, byte_size):
        super(self.__class__, self).__init__(OpType.CMP, 1, byte_size)
        self.add_operand(OperandType.SPECIAL, cmp_type)
        
class Div(Insn):
    def __init__(self, byte_size, signed):
        super(self.__class__, self).__init__(OpType.DIV, 1, byte_size)
        if signed is True:
            self.add_operand(OperandType.SPECIAL, 1)
        else:
            self.add_operand(OperandType.SPECIAL, 0)
        
class Mod(Insn):
    def __init__(self, byte_size, signed):
        super(self.__class__, self).__init__(OpType.MOD, 1, byte_size)
        if signed is True:
            self.add_operand(OperandType.SPECIAL, 1)
        else:
            self.add_operand(OperandType.SPECIAL, 0)
                
class Mul(Insn):
    def __init__(self, byte_size, signed):
        super(self.__class__, self).__init__(OpType.MUL, 1, byte_size)
        if signed is True:
            self.add_operand(OperandType.SPECIAL, 1)
        else:
            self.add_operand(OperandType.SPECIAL, 0)

class Or(Insn):
    def __init__(self, byte_size):
        super(self.__class__, self).__init__(OpType.OR, 0, byte_size)

class Sar(Insn):
    def __init__(self, byte_size):
        super(self.__class__, self).__init__(OpType.SAR, 0, byte_size)

class Shr(Insn):
    def __init__(self, byte_size):
        super(self.__class__, self).__init__(OpType.SHR, 0, byte_size)
        
class Shl(Insn):
    def __init__(self, byte_size):
        super(self.__class__, self).__init__(OpType.SHL, 0, byte_size)

class Sub(Insn):
    def __init__(self, byte_size):
        super(self.__class__, self).__init__(OpType.SUB, 0, byte_size)

class Xor(Insn):
    def __init__(self, byte_size):
        super(self.__class__, self).__init__(OpType.XOR, 0, byte_size)

class Not(Insn):
    def __init__(self, byte_size):
        super(self.__class__, self).__init__(OpType.NOT, 0, byte_size)


class CondBranch(Insn):
    def __init__(self):
        super(self.__class__, self).__init__(OpType.COND_BRANCH, 0)
        

class Branch(Insn):
    def __init__(self):
        super(self.__class__, self).__init__(OpType.BRANCH, 0)

class Call(Insn):
    def __init__(self, call_addr):
        super(self.__class__, self).__init__(OpType.CALL, 1)
        self.add_operand(OperandType.CONSTANT8, call_addr)

TMP_MAP = {}
TMP_PTR = 0
def reset_tmp_map():
    global TMP_MAP, TMP_PTR
    TMP_MAP.clear()
    TMP_PTR = 0

class LoadTmp(Insn):
    def __init__(self, byte_size, tmp_idx):
        global TMP_MAP, TMP_PTR
        super(self.__class__, self).__init__(OpType.LOAD_TMP, 1, byte_size)
        idx = tmp_idx
        if tmp_idx in TMP_MAP.keys():
            idx = TMP_MAP[tmp_idx]
        else:
            idx = TMP_PTR
            TMP_PTR += 1
            TMP_MAP[tmp_idx] = idx
        self.add_operand(OperandType.SPECIAL, idx)


class StoreTmp(Insn):
    def __init__(self, byte_size, tmp_idx):
        global TMP_MAP, TMP_PTR
        super(self.__class__, self).__init__(OpType.STORE_TMP, 1, byte_size)
        idx = tmp_idx
        if tmp_idx in TMP_MAP.keys():
            idx = TMP_MAP[tmp_idx]
        else:
            idx = TMP_PTR
            TMP_PTR += 1
            TMP_MAP[tmp_idx] = idx
        self.add_operand(OperandType.SPECIAL, idx)

class Sext(Insn):
    def __init__(self, byte_size, target_size):
        super(self.__class__, self).__init__(OpType.SEXT, 1, byte_size)
        self.add_operand(OperandType.SPECIAL, target_size)

class Zext(Insn):
    def __init__(self, byte_size, target_size):
        super(self.__class__, self).__init__(OpType.ZEXT, 1, byte_size)
        self.add_operand(OperandType.SPECIAL, target_size)

class Hlt(Insn):
    def __init__(self):
        super(self.__class__, self).__init__(OpType.HLT, 0)

sample = [
    PushReg('rax'),
    LoadMem(),
    PushImm(1,1),
    StoreMem(),
    PopReg('rax'),
    Add(1),
    And(1),
    Cmp(1,1),
    Div(1,False),
    Mod(1,False),
    Mul(1,False),
    Or(1),
    Sar(1),
    Shl(1),
    Shr(1),
    Sub(1),
    Xor(1),
    Not(1),
    Call(123),
    CondBranch(),
    Branch(),
    LoadTmp(1,1),
    StoreTmp(1,1),
    Sext(1,1),
    Zext(1,1),
    Hlt()
]

def get_all_op():
    op_map = {}
    for insn in sample:
        op_map[insn.__class__.__name__] = insn._get_all_opcode()
    return op_map

def dump_insn():
    op_map = get_all_op()
    for name in op_map.keys():
        ops = op_map[name]
        if len(ops) == 1:
            print('#define %s %d' % (name, ops[0]))
        else:
            print('#define %s1 %d' % (name, ops[0]))
            print('#define %s2 %d' % (name, ops[1]))
            print('#define %s4 %d' % (name, ops[2]))
            print('#define %s8 %d' % (name, ops[3]))
            print('#define %s16 %d' % (name, ops[4]))


