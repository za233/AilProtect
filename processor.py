from injector import *
from convertor import *
import struct
def process_x64(filename, func_rva : list, load_base):
    buffer = open('runtime_x64.bin', 'rb').read()
    global_info = {}
    convertor = CodeConverter(filename, load_base)
    print('[-] translating function bytecodes...')
    offset_ptr = 0
    for rva in func_rva:
        print('[+] - processing function: ' + hex(rva + load_base))
        convertor.process(rva + load_base)
        opcodes = convertor.get_opcode()
        global_info[rva + load_base] = (offset_ptr, convertor.get_func_graph())
        buffer += opcodes
        offset_ptr += len(opcodes)
    
    print('[-] building runtime codes')
    vm_entry = inject_data_to_section(filename, filename + '.out', b'.vmp', buffer)
    print('[-] patching functions..')
    for rva in func_rva:
        entry_code1 = b'\x48\x89\x64\x24\xF8\x48\x83\xEC\x08\xE8\x00\x00\x00\x00\x48\x83\x2C\x24\x0E\x48\xC7\x44\x24\xF8'
        entry_code2 = b'\x48\x83\xEC\x08\xE9'
        
        opcode_offset = struct.pack('I', global_info[rva + load_base][0])
        jump_stub0 = entry_code1 + opcode_offset + entry_code2
        jump_rva = rva + len(jump_stub0) - 1
        jump_offset = struct.pack('i', vm_entry - jump_rva - 5)
        jump_stub = jump_stub0 + jump_offset
        
        cfg = global_info[rva + load_base][1]
        patched_size = 0
        for n in cfg.nodes():
            patch_bytes(filename + '.out', filename + '.out', n.addr - load_base, b'\xCC' * n.original_size)
            patched_size += n.original_size

        assert len(jump_stub) <= patched_size

        patch_bytes(filename + '.out', filename + '.out', rva, jump_stub)
    print('[!] Done')


    

        


process_x64('.\\example\\rc4.exe', [0x1560], 0x400000)
        
    