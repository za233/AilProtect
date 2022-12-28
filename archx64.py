reg_map = {
    "al" : 1,
    "ah" : 2,
    "ax" : 3,
    "eax" : 4,
    "rax" : 5,
    "bl" : 6,
    "bh" : 7,
    "bx" : 8,
    "ebx" : 9,
    "rbx" : 10,
    "cl" : 11,
    "ch" : 12,
    "cx" : 13,
    "ecx" : 14,
    "rcx" : 15,
    "dl" : 16,
    "dh" : 17,
    "dx" : 18,
    "edx" : 19,
    "rdx" : 20,
    "si" : 21,
    "esi" : 22,
    "rsi" : 23,
    "di" : 24,
    "edi" : 25,
    "rdi" : 26,
    "sp" : 27,
    "esp" : 28,
    "rsp" : 29,
    "bp" : 30,
    "ebp" : 31,
    "rbp" : 32,
    "r8b" : 33,
    "r8w" : 34,
    "r8d" : 35,
    "r8" : 36,
    "r9b" : 37,
    "r9w" : 38,
    "r9d" : 39,
    "r9" : 40,
    "r10b" : 41,
    "r10w" : 42,
    "r10d" : 43,
    "r10" : 44,
    "r11b" : 45,
    "r11w" : 46,
    "r11d" : 47,
    "r11" : 48,
    "r12b" : 49,
    "r12w" : 50,
    "r12d" : 51,
    "r12" : 52,
    "r13b" : 53,
    "r13w" : 54,
    "r13d" : 55,
    "r13" : 56,
    "r14b" : 57,
    "r14w" : 58,
    "r14d" : 59,
    "r14" : 60,
    "r15b" : 61,
    "r15w" : 62,
    "r15d" : 63,
    "r15" : 64,
    "cc_op" : 65,
    "cc_dep1" : 66,
    "cc_dep2" : 67,
    "rip" : 68,
}
def get_reg(name):
    if name in reg_map.keys():
        return reg_map[name]
    print('[!] unsupported register, using reserved index')
    return 0
def generate_store_code(load = False):
    global reg_map
    print('switch(reg_id)')
    print('{')
    for k, v in reg_map.items():
        print('\tcase %d:' % v)
        if load:
            print('\t\tvalue = context->%s;' % k)
        else:
            print('\t\tcontext->%s = value;' % k)
        print('\t\tbreak;')
    print('}')

