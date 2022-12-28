from ir import *
class Function:
    def __init__(self, insns):
        pass

    def verify(self):
        pass

class BasicBlock:
    def __init__(self, addr, insn):
        self.insn = insn
        self.addr = addr
        
        pass

    def get_successors(self):
        pass

    def get_insns(self):
        pass

class TransformPass:
    def __init__(self, all_insn):
        self.all_insn = all_insn
        pass

    def generate_func(self):
        pass
