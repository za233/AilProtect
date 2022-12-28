#include<cstdio>
#include<cstdlib>
#include<cstdint>
#include<cstring>
#include<cstddef>
#define PushReg 0
#define LoadMem 8
#define PushImm1 24
#define PushImm2 25
#define PushImm4 26
#define PushImm8 27
#define PushImm16 28
#define StoreMem 32
#define PopReg 40
#define Add1 48
#define Add2 49
#define Add4 50
#define Add8 51
#define Add16 52
#define And1 56
#define And2 57
#define And4 58
#define And8 59
#define And16 60
#define Cmp1 64
#define Cmp2 65
#define Cmp4 66
#define Cmp8 67
#define Cmp16 68
#define Div1 72
#define Div2 73
#define Div4 74
#define Div8 75
#define Div16 76
#define Mod1 80
#define Mod2 81
#define Mod4 82
#define Mod8 83
#define Mod16 84
#define Mul1 88
#define Mul2 89
#define Mul4 90
#define Mul8 91
#define Mul16 92
#define Or1 96
#define Or2 97
#define Or4 98
#define Or8 99
#define Or16 100
#define Sar1 104
#define Sar2 105
#define Sar4 106
#define Sar8 107
#define Sar16 108
#define Shl1 112
#define Shl2 113
#define Shl4 114
#define Shl8 115
#define Shl16 116
#define Shr1 120
#define Shr2 121
#define Shr4 122
#define Shr8 123
#define Shr16 124
#define Sub1 128
#define Sub2 129
#define Sub4 130
#define Sub8 131
#define Sub16 132
#define Xor1 136
#define Xor2 137
#define Xor4 138
#define Xor8 139
#define Xor16 140
#define Not1 152
#define Not2 153
#define Not4 154
#define Not8 155
#define Not16 156
//#define Call 168
#define CondBranch 224
#define Branch 176
#define LoadTmp1 216
#define LoadTmp2 217
#define LoadTmp4 218
#define LoadTmp8 219
#define LoadTmp16 220
#define StoreTmp1 208
#define StoreTmp2 209
#define StoreTmp4 210
#define StoreTmp8 211
#define StoreTmp16 212
#define Sext1 192
#define Sext2 193
#define Sext4 194
#define Sext8 195
#define Sext16 196
#define Zext1 200
#define Zext2 201
#define Zext4 202
#define Zext8 203
#define Zext16 204
#define Hlt 160
struct VMContext
{
	union
	{
		uint64_t rax;
		uint32_t eax;
		uint16_t ax;
		struct
		{
			uint8_t al : 8;
			uint8_t ah : 8;
		};
		
	};	
	union
	{
		uint64_t rbx;
		uint32_t ebx;
		uint16_t bx;
		struct
		{
			uint8_t bl : 8;
			uint8_t bh : 8;
		};
		
	};
	union
	{
		uint64_t rcx;
		uint32_t ecx;
		uint16_t cx;
		struct
		{
			uint8_t cl : 8;
			uint8_t ch : 8;
		};
		
	};
	union
	{
		uint64_t rdx;
		uint32_t edx;
		uint16_t dx;
		struct
		{
			uint8_t dl : 8;
			uint8_t dh : 8;
		};
		
	};
	union
	{
		uint64_t rsi;
		uint32_t esi;
		uint16_t si;
	};
	union
	{
		uint64_t rdi;
		uint32_t edi;
		uint16_t di;
	};
	union
	{
		uint64_t rsp;
		uint32_t esp;
		uint16_t sp;
	};
	union
	{
		uint64_t rbp;
		uint32_t ebp;
		uint16_t bp;
	};
	union
	{
		uint64_t r8;
		uint32_t r8d;
		uint16_t r8w;
		uint8_t r8b;
		
	};
	
	union
	{
		uint64_t r9;
		uint32_t r9d;
		uint16_t r9w;
		uint8_t r9b;
		
	};
	union
	{
		uint64_t r10;
		uint32_t r10d;
		uint16_t r10w;
		uint8_t r10b;
		
	};
	union
	{
		uint64_t r11;
		uint32_t r11d;
		uint16_t r11w;
		uint8_t r11b;
		
	};
	union
	{
		uint64_t r12;
		uint32_t r12d;
		uint16_t r12w;
		uint8_t r12b;
		
	};
	union
	{
		uint64_t r13;
		uint32_t r13d;
		uint16_t r13w;
		uint8_t r13b;
		
	};
	union
	{
		uint64_t r14;
		uint32_t r14d;
		uint16_t r14w;
		uint8_t r14b;
		
	};
	union
	{
		uint64_t r15;
		uint32_t r15d;
		uint16_t r15w;
		uint8_t r15b;
		
	};
	uint64_t rip;
	uint64_t cc_op;
	uint64_t cc_dep1,cc_dep2;
	
}; 
void DereVM(uint8_t *opcode,VMContext *context)
{
	unsigned __int128 stack[256];
	unsigned __int128 tmp[100];
    for(int i = 0; i < 256; i++)
        stack[i] = 0;
    for(int i = 0; i < 100; i++)
        tmp[i] = 0;
	uint8_t top = 0;
	uint32_t pc = 0;
	uint8_t *ptr = (uint8_t*)&stack[0];
	for(int i = 0; i < 16; i++)
		ptr[i] = 0xCC;
	while(true)
	{
		uint8_t op = opcode[pc];
		if(op == CondBranch)
		{
			uint32_t offset = stack[top];
			uint8_t cond = stack[top - 1];
			top -= 2;
			if(cond)
				pc += offset;
			else
				pc += 1; 
			continue;
		}
		else if(op == Branch)
		{
			uint32_t offset = stack[top];
			top -= 1;
			pc += offset;
			continue;
		}
		else if(op == LoadMem)
		{
			uint8_t *src = (uint8_t*)stack[top - 1];
			uint32_t size = stack[top];
			top -= 2;
			if(size > 16)
				break;
			uint8_t *dst = (uint8_t*)&(stack[top + 1]);
			for(int i = 0;i < size; i++)
				dst[i] = src[i];
			top += 1;
			pc += 1;
			continue; 
		}
		else if(op == StoreMem)
		{
			uint8_t *dst = (uint8_t*)stack[top - 2];
			uint32_t size = stack[top - 1];
			uint8_t *src = (uint8_t*)&(stack[top]);
			top -= 3;
			if(size > 16)
				break;
			for(int i = 0;i < size; i++)
				dst[i] = src[i];
			pc += 1;
			continue;
		}
		else if(op == PushReg)
		{
			uint8_t reg_id = opcode[pc + 1];
			unsigned __int128 value = 0;
			switch(reg_id)
			{
				case 1:
					value = context->al;
					break;
				case 2:
					value = context->ah;
					break;
				case 3:
					value = context->ax;
					break;
				case 4:
					value = context->eax;
					break;
				case 5:
					value = context->rax;
					break;
				case 6:
					value = context->bl;
					break;
				case 7:
					value = context->bh;
					break;
				case 8:
					value = context->bx;
					break;
				case 9:
					value = context->ebx;
					break;
				case 10:
					value = context->rbx;
					break;
				case 11:
					value = context->cl;
					break;
				case 12:
					value = context->ch;
					break;
				case 13:
					value = context->cx;
					break;
				case 14:
					value = context->ecx;
					break;
				case 15:
					value = context->rcx;
					break;
				case 16:
					value = context->dl;
					break;
				case 17:
					value = context->dh;
					break;
				case 18:
					value = context->dx;
					break;
				case 19:
					value = context->edx;
					break;
				case 20:
					value = context->rdx;
					break;
				case 21:
					value = context->si;
					break;
				case 22:
					value = context->esi;
					break;
				case 23:
					value = context->rsi;
					break;
				case 24:
					value = context->di;
					break;
				case 25:
					value = context->edi;
					break;
				case 26:
					value = context->rdi;
					break;
				case 27:
					value = context->sp;
					break;
				case 28:
					value = context->esp;
					break;
				case 29:
					value = context->rsp;
					break;
				case 30:
					value = context->bp;
					break;
				case 31:
					value = context->ebp;
					break;
				case 32:
					value = context->rbp;
					break;
				case 33:
					value = context->r8b;
					break;
				case 34:
					value = context->r8w;
					break;
				case 35:
					value = context->r8d;
					break;
				case 36:
					value = context->r8;
					break;
				case 37:
					value = context->r9b;
					break;
				case 38:
					value = context->r9w;
					break;
				case 39:
					value = context->r9d;
					break;
				case 40:
					value = context->r9;
					break;
				case 41:
					value = context->r10b;
					break;
				case 42:
					value = context->r10w;
					break;
				case 43:
					value = context->r10d;
					break;
				case 44:
					value = context->r10;
					break;
				case 45:
					value = context->r11b;
					break;
				case 46:
					value = context->r11w;
					break;
				case 47:
					value = context->r11d;
					break;
				case 48:
					value = context->r11;
					break;
				case 49:
					value = context->r12b;
					break;
				case 50:
					value = context->r12w;
					break;
				case 51:
					value = context->r12d;
					break;
				case 52:
					value = context->r12;
					break;
				case 53:
					value = context->r13b;
					break;
				case 54:
					value = context->r13w;
					break;
				case 55:
					value = context->r13d;
					break;
				case 56:
					value = context->r13;
					break;
				case 57:
					value = context->r14b;
					break;
				case 58:
					value = context->r14w;
					break;
				case 59:
					value = context->r14d;
					break;
				case 60:
					value = context->r14;
					break;
				case 61:
					value = context->r15b;
					break;
				case 62:
					value = context->r15w;
					break;
				case 63:
					value = context->r15d;
					break;
				case 64:
					value = context->r15;
					break;
				case 65:
					value = context->cc_op;
					break;
				case 66:
					value = context->cc_dep1;
					break;
				case 67:
					value = context->cc_dep2;
					break;
				case 68:
					value = context->rip;
					break;
			}
			stack[top + 1] = value;
			top += 1;
			pc += 2;
			continue;
		}
		else if(op == PopReg)
		{
			uint8_t reg_id = opcode[pc + 1];
			unsigned __int128 value = stack[top];
			switch(reg_id)
			{
				case 1:
					context->al = value;
					break;
				case 2:
					context->ah = value;
					break;
				case 3:
					context->ax = value;
					break;
				case 4:
					context->eax = value;
					break;
				case 5:
					context->rax = value;
					break;
				case 6:
					context->bl = value;
					break;
				case 7:
					context->bh = value;
					break;
				case 8:
					context->bx = value;
					break;
				case 9:
					context->ebx = value;
					break;
				case 10:
					context->rbx = value;
					break;
				case 11:
					context->cl = value;
					break;
				case 12:
					context->ch = value;
					break;
				case 13:
					context->cx = value;
					break;
				case 14:
					context->ecx = value;
					break;
				case 15:
					context->rcx = value;
					break;
				case 16:
					context->dl = value;
					break;
				case 17:
					context->dh = value;
					break;
				case 18:
					context->dx = value;
					break;
				case 19:
					context->edx = value;
					break;
				case 20:
					context->rdx = value;
					break;
				case 21:
					context->si = value;
					break;
				case 22:
					context->esi = value;
					break;
				case 23:
					context->rsi = value;
					break;
				case 24:
					context->di = value;
					break;
				case 25:
					context->edi = value;
					break;
				case 26:
					context->rdi = value;
					break;
				case 27:
					context->sp = value;
					break;
				case 28:
					context->esp = value;
					break;
				case 29:
					context->rsp = value;
					break;
				case 30:
					context->bp = value;
					break;
				case 31:
					context->ebp = value;
					break;
				case 32:
					context->rbp = value;
					break;
				case 33:
					context->r8b = value;
					break;
				case 34:
					context->r8w = value;
					break;
				case 35:
					context->r8d = value;
					break;
				case 36:
					context->r8 = value;
					break;
				case 37:
					context->r9b = value;
					break;
				case 38:
					context->r9w = value;
					break;
				case 39:
					context->r9d = value;
					break;
				case 40:
					context->r9 = value;
					break;
				case 41:
					context->r10b = value;
					break;
				case 42:
					context->r10w = value;
					break;
				case 43:
					context->r10d = value;
					break;
				case 44:
					context->r10 = value;
					break;
				case 45:
					context->r11b = value;
					break;
				case 46:
					context->r11w = value;
					break;
				case 47:
					context->r11d = value;
					break;
				case 48:
					context->r11 = value;
					break;
				case 49:
					context->r12b = value;
					break;
				case 50:
					context->r12w = value;
					break;
				case 51:
					context->r12d = value;
					break;
				case 52:
					context->r12 = value;
					break;
				case 53:
					context->r13b = value;
					break;
				case 54:
					context->r13w = value;
					break;
				case 55:
					context->r13d = value;
					break;
				case 56:
					context->r13 = value;
					break;
				case 57:
					context->r14b = value;
					break;
				case 58:
					context->r14w = value;
					break;
				case 59:
					context->r14d = value;
					break;
				case 60:
					context->r14 = value;
					break;
				case 61:
					context->r15b = value;
					break;
				case 62:
					context->r15w = value;
					break;
				case 63:
					context->r15d = value;
					break;
				case 64:
					context->r15 = value;
					break;
				case 65:
					context->cc_op = value;
					break;
				case 66:
					context->cc_dep1 = value;
					break;
				case 67:
					context->cc_dep2 = value;
					break;
				case 68:
					context->rip = value;
					break;
			}
			top -= 1;
			pc += 2;
			continue;
		}
		else if(op == LoadTmp1)
		{
			uint8_t tmp_idx = opcode[pc + 1];
			uint8_t value = tmp[tmp_idx];
			stack[top + 1] = value;
			top += 1; 
			pc += 2;
			continue;
		}
		else if(op == LoadTmp2)
		{
			uint8_t tmp_idx = opcode[pc + 1];
			uint16_t value = tmp[tmp_idx];
			stack[top + 1] = value;
			top += 1; 
			pc += 2;
			continue;
		}
		else if(op == LoadTmp4)
		{
			uint8_t tmp_idx = opcode[pc + 1];
			uint32_t value = tmp[tmp_idx];
			stack[top + 1] = value;
			top += 1; 
			pc += 2;
			continue;
		}
		else if(op == LoadTmp8)
		{
			uint8_t tmp_idx = opcode[pc + 1];
			uint64_t value = tmp[tmp_idx];
			stack[top + 1] = value;
			top += 1; 
			pc += 2;
			continue;
		}
		else if(op == LoadTmp16)
		{
			uint8_t tmp_idx = opcode[pc + 1];
			unsigned __int128 value = tmp[tmp_idx];
			stack[top + 1] = value;
			top += 1; 
			pc += 2;
			continue;
		}
		else if(op == StoreTmp1)
		{
			uint8_t tmp_idx = opcode[pc + 1];
			uint8_t value = stack[top];
			tmp[tmp_idx] = value;
			top -= 1;
			pc += 2;
			continue;
		}
		else if(op == StoreTmp2)
		{
			uint8_t tmp_idx = opcode[pc + 1];
			uint16_t value = stack[top];
			tmp[tmp_idx] = value;
			top -= 1;
			pc += 2;
			continue;
		}
		else if(op == StoreTmp4)
		{
			uint8_t tmp_idx = opcode[pc + 1];
			uint32_t value = stack[top];
			tmp[tmp_idx] = value;
			top -= 1;
			pc += 2;
			continue;
		}
		else if(op == StoreTmp8)
		{
			uint8_t tmp_idx = opcode[pc + 1];
			uint64_t value = stack[top];
			tmp[tmp_idx] = value;
			top -= 1;
			pc += 2;
			continue;
		}
		else if(op == StoreTmp16)
		{
			uint8_t tmp_idx = opcode[pc + 1];
			unsigned __int128 value = stack[top];
			tmp[tmp_idx] = value;
			top -= 1;
			pc += 2;
			continue;
		}
		else if(op == PushImm1)
		{
			uint8_t v = opcode[pc + 1];
			stack[top + 1] = v;
			top += 1;
			pc += 2;
			continue;
		}
		else if(op == PushImm2)
		{
			uint16_t v = *(uint16_t*)&opcode[pc + 1];
			stack[top + 1] = v;
			top += 1;
			pc += 3;
			continue;
		}
		else if(op == PushImm4)
		{
			uint32_t v = *(uint32_t*)&opcode[pc + 1];
			stack[top + 1] = v;
			top += 1;
			pc += 5;
			continue;
		}
		else if(op == PushImm8)
		{
			uint64_t v = *(uint64_t*)&opcode[pc + 1];
			stack[top + 1] = v;
			top += 1;
			pc += 9;
			continue;
		}
		else if(op == PushImm16)
		{
			unsigned __int128 v = *(unsigned __int128*)&opcode[pc + 1];
			stack[top + 1] = v;
			top += 1;
			pc += 17;
			continue;
		}
		else if(op == Add1)
		{
			uint8_t a = stack[top], b = stack[top - 1];
			uint8_t c = a + b;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Add2)
		{
			uint16_t a = stack[top], b = stack[top - 1];
			uint16_t c = a + b;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		} 
		else if(op == Add4)
		{
			uint32_t a = stack[top], b = stack[top - 1];
			uint32_t c = a + b;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Add8)
		{
			uint64_t a = stack[top], b = stack[top - 1];
			uint64_t c = a + b;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Add16)
		{
			stack[top - 1] += stack[top];
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == And1)
		{
			uint8_t a = stack[top], b = stack[top - 1];
			uint8_t c = a & b;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == And2)
		{
			uint16_t a = stack[top], b = stack[top - 1];
			uint16_t c = a & b;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		} 
		else if(op == And4)
		{
			uint32_t a = stack[top], b = stack[top - 1];
			uint32_t c = a & b;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == And8)
		{
			uint64_t a = stack[top], b = stack[top - 1];
			uint64_t c = a & b;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == And16)
		{
			stack[top - 1] &= stack[top];
			top -= 1;
			pc += 1;
			continue;
		}
		/*
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
		*/
		else if(op == Cmp1)
		{
			uint8_t b = stack[top], a = stack[top - 1];
			uint8_t c = 0;
			uint8_t cc = opcode[pc + 1];
			if(cc == 0)
				c = (a == b);
			else if(cc == 1)
				c = (a != b);
			else if(cc == 2)
				c = (a < b);
			else if(cc == 3)
				c = (a >= b);
			else if(cc == 4)
				c = (a <= b);
			else if(cc == 5)
				c = (a > b);
			else if(cc == 6)
				c = ((int8_t)a < (int8_t)b);
			else if(cc == 7)
				c = ((int8_t)a >= (int8_t)b);
			else if(cc == 8)
				c = ((int8_t)a <= (int8_t)b);
			else if(cc == 9)
				c = ((int8_t)a > (int8_t)b);
			stack[top - 1] = c;
			top -= 1;
			pc += 2;
			continue;
		}
		else if(op == Cmp2)
		{
			uint16_t b = stack[top], a = stack[top - 1];
			uint16_t c = 0;
			uint8_t cc = opcode[pc + 1];
			if(cc == 0)
				c = (a == b);
			else if(cc == 1)
				c = (a != b);
			else if(cc == 2)
				c = (a < b);
			else if(cc == 3)
				c = (a >= b);
			else if(cc == 4)
				c = (a <= b);
			else if(cc == 5)
				c = (a > b);
			else if(cc == 6)
				c = ((int8_t)a < (int8_t)b);
			else if(cc == 7)
				c = ((int8_t)a >= (int8_t)b);
			else if(cc == 8)
				c = ((int8_t)a <= (int8_t)b);
			else if(cc == 9)
				c = ((int8_t)a > (int8_t)b);
			stack[top - 1] = c;
			top -= 1;
			pc += 2;
			continue;
		} 
		else if(op == Cmp4)
		{
			uint32_t b = stack[top], a = stack[top - 1];
			uint32_t c = 0;
			uint8_t cc = opcode[pc + 1];
			if(cc == 0)
				c = (a == b);
			else if(cc == 1)
				c = (a != b);
			else if(cc == 2)
				c = (a < b);
			else if(cc == 3)
				c = (a >= b);
			else if(cc == 4)
				c = (a <= b);
			else if(cc == 5)
				c = (a > b);
			else if(cc == 6)
				c = ((int32_t)a < (int32_t)b);
			else if(cc == 7)
				c = ((int32_t)a >= (int32_t)b);
			else if(cc == 8)
				c = ((int32_t)a <= (int32_t)b);
			else if(cc == 9)
				c = ((int32_t)a > (int32_t)b);
			stack[top - 1] = c;
			top -= 1;
			pc += 2;
			continue;
		}
		else if(op == Cmp8)
		{
			uint64_t b = stack[top], a = stack[top - 1];
			uint64_t c = 0;
			uint8_t cc = opcode[pc + 1];
			if(cc == 0)
				c = (a == b);
			else if(cc == 1)
				c = (a != b);
			else if(cc == 2)
				c = (a < b);
			else if(cc == 3)
				c = (a >= b);
			else if(cc == 4)
				c = (a <= b);
			else if(cc == 5)
				c = (a > b);
			else if(cc == 6)
				c = ((int64_t)a < (int64_t)b);
			else if(cc == 7)
				c = ((int64_t)a >= (int64_t)b);
			else if(cc == 8)
				c = ((int64_t)a <= (int64_t)b);
			else if(cc == 9)
				c = ((int64_t)a > (int64_t)b);
			stack[top - 1] = c;
			top -= 1;
			pc += 2;
			continue;
		}
		else if(op == Cmp16)
		{
			unsigned __int128 b = stack[top], a = stack[top - 1];
			unsigned __int128 c = 0;
			uint8_t cc = opcode[pc + 1];
			if(cc == 0)
				c = (a == b);
			else if(cc == 1)
				c = (a != b);
			else if(cc == 2)
				c = (a < b);
			else if(cc == 3)
				c = (a >= b);
			else if(cc == 4)
				c = (a <= b);
			else if(cc == 5)
				c = (a > b);
			else if(cc == 6)
				c = ((__int128)a < (__int128)b);
			else if(cc == 7)
				c = ((__int128)a >= (__int128)b);
			else if(cc == 8)
				c = ((__int128)a <= (__int128)b);
			else if(cc == 9)
				c = ((__int128)a > (__int128)b);
			stack[top - 1] = c;
			top -= 1;
			pc += 2;
			continue;
				
		}
		else if(op == Div1)
		{
			uint8_t a = stack[top], b = stack[top - 1];
			uint8_t s = opcode[pc + 1];
			if(s)
				stack[top - 1] = (int8_t)b / (int8_t)a;
			else
				stack[top - 1] = b / a;
			top -= 1;
			pc += 2;
			continue;
		}
		else if(op == Div2)
		{
			uint16_t a = stack[top], b = stack[top - 1];
			uint8_t s = opcode[pc + 1];
			if(s)
				stack[top - 1] = (int16_t)b / (int16_t)a;
			else
				stack[top - 1] = b / a;
			top -= 1;
			pc += 2;
			continue;
		} 
		else if(op == Div4)
		{
			uint32_t a = stack[top], b = stack[top - 1];
			uint8_t s = opcode[pc + 1];
			if(s)
				stack[top - 1] = (int32_t)b / (int32_t)a;
			else
				stack[top - 1] = b / a;
			top -= 1;
			pc += 2;
			continue;
		}
		else if(op == Div8)
		{
			uint64_t a = stack[top], b = stack[top - 1];
			uint8_t s = opcode[pc + 1];
			if(s)
				stack[top - 1] = (int64_t)b / (int64_t)a;
			else
				stack[top - 1] = b / a;
			top -= 1;
			pc += 2;
			continue;
		}
		else if(op == Div16)
		{
			unsigned __int128 a = stack[top], b = stack[top - 1];
			uint8_t s = opcode[pc + 1];
			if(s)
				stack[top - 1] = (__int128)b / (__int128)a;
			else
				stack[top - 1] = b / a;
			top -= 1;
			pc += 2;
			continue;
				
		}
		else if(op == Mod1)
		{
			uint8_t a = stack[top], b = stack[top - 1];
			uint8_t s = opcode[pc + 1];
			if(s)
				stack[top - 1] = (int8_t)b % (int8_t)a;
			else
				stack[top - 1] = b % a;
			top -= 1;
			pc += 2;
			continue;
		}
		else if(op == Mod2)
		{
			uint16_t a = stack[top], b = stack[top - 1];
			uint8_t s = opcode[pc + 1];
			if(s)
				stack[top - 1] = (int16_t)b % (int16_t)a;
			else
				stack[top - 1] = b % a;
			top -= 1;
			pc += 2;
			continue;
		} 
		else if(op == Mod4)
		{
			uint32_t a = stack[top], b = stack[top - 1];
			uint8_t s = opcode[pc + 1];
			if(s)
				stack[top - 1] = (int32_t)b % (int32_t)a;
			else
				stack[top - 1] = b % a;
			top -= 1;
			pc += 2;
			continue;
		}
		else if(op == Mod8)
		{
			uint64_t a = stack[top], b = stack[top - 1];
			uint8_t s = opcode[pc + 1];
			if(s)
				stack[top - 1] = (int64_t)b % (int64_t)a;
			else
				stack[top - 1] = b % a;
			top -= 1;
			pc += 2;
			continue;
		}
		else if(op == Mod16)
		{
			unsigned __int128 a = stack[top], b = stack[top - 1];
			uint8_t s = opcode[pc + 1];
			if(s)
				stack[top - 1] = (__int128)b % (__int128)a;
			else
				stack[top - 1] = b % a;
			top -= 1;
			pc += 2;
			continue;
				
		}
		else if(op == Mul1)
		{
			uint8_t a = stack[top], b = stack[top - 1];
			uint8_t s = opcode[pc + 1];
			if(s)
				stack[top - 1] = (int8_t)a * (int8_t)b;
			else
				stack[top - 1] = a * b;
			top -= 1;
			pc += 2;
			continue;
		}
		else if(op == Mul2)
		{
			uint16_t a = stack[top], b = stack[top - 1];
			uint8_t s = opcode[pc + 1];
			if(s)
				stack[top - 1] = (int16_t)a * (int16_t)b;
			else
				stack[top - 1] = a * b;
			top -= 1;
			pc += 2;
			continue; 
		} 
		else if(op == Mul4)
		{
			uint32_t a = stack[top], b = stack[top - 1];
			uint8_t s = opcode[pc + 1];
			if(s)
				stack[top - 1] = (int32_t)a * (int32_t)b;
			else
				stack[top - 1] = a * b;
			top -= 1;
			pc += 2;
			continue;
		}
		else if(op == Mul8)
		{
			uint64_t a = stack[top], b = stack[top - 1];
			uint8_t s = opcode[pc + 1];
			if(s)
				stack[top - 1] = (int64_t)a * (int64_t)b;
			else
				stack[top - 1] = a * b;
			top -= 1;
			pc += 2;
			continue;
		}
		else if(op == Mul16)
		{
			unsigned __int128 a = stack[top], b = stack[top - 1];
			uint8_t s = opcode[pc + 1];
			if(s)
				stack[top - 1] = (__int128)a * (__int128)b;
			else
				stack[top - 1] = a * b;
			top -= 1;
			pc += 2;
			continue;
		}
		
		if(op == Or1)
		{
			uint8_t a = stack[top], b = stack[top - 1];
			uint8_t c = a | b;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Or2)
		{
			uint16_t a = stack[top], b = stack[top - 1];
			uint16_t c = a | b;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
			
		} 
		else if(op == Or4)
		{
			uint32_t a = stack[top], b = stack[top - 1];
			uint32_t c = a | b;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Or8)
		{
			uint64_t a = stack[top], b = stack[top - 1];
			uint64_t c = a | b;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Or16)
		{
			stack[top - 1] |= stack[top];
			top -= 1;
			pc += 1;
			continue;
				
		}
		else if(op == Sar1)
		{
			int8_t a = stack[top], b = stack[top - 1];
			int8_t c = b >> a;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Sar2)
		{
			int16_t a = stack[top], b = stack[top - 1];
			int16_t c = b >> a;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
			
		} 
		else if(op == Sar4)
		{
			int32_t a = stack[top], b = stack[top - 1];
			int32_t c = b >> a;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Sar8)
		{
			int64_t a = stack[top], b = stack[top - 1];
			int64_t c = b >> a;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Sar16)
		{
			
			__int128 a = stack[top], b = stack[top - 1];
			__int128 c = b >> a;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Shr1)
		{
			uint8_t a = stack[top], b = stack[top - 1];
			uint8_t c = b >> a;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Shr2)
		{
			uint16_t a = stack[top], b = stack[top - 1];
			uint16_t c = b >> a;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		} 
		else if(op == Shr4)
		{
			uint32_t a = stack[top], b = stack[top - 1];
			uint32_t c = b >> a;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Shr8)
		{
			uint64_t a = stack[top], b = stack[top - 1];
			uint64_t c = b >> a;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Shr16)
		{
			
			unsigned __int128 a = stack[top], b = stack[top - 1];
			unsigned __int128 c = b >> a;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;	
		}
		else if(op == Shl1)
		{
			uint8_t a = stack[top], b = stack[top - 1];
			uint8_t c = b << a;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Shl2)
		{
			uint16_t a = stack[top], b = stack[top - 1];
			uint16_t c = b << a;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		} 
		else if(op == Shl4)
		{
			uint32_t a = stack[top], b = stack[top - 1];
			uint32_t c = b << a;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Shl8)
		{
			uint64_t a = stack[top], b = stack[top - 1];
			uint64_t c = b << a;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Shl16)
		{
			
			unsigned __int128 a = stack[top], b = stack[top - 1];
			unsigned __int128 c = b << a;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;	
		}
		else if(op == Sub1)
		{
			uint8_t a = stack[top], b = stack[top - 1];
			uint8_t c = b - a;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Sub2)
		{
			uint16_t a = stack[top], b = stack[top - 1];
			uint16_t c = b - a;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		} 
		else if(op == Sub4)
		{
			uint32_t a = stack[top], b = stack[top - 1];
			uint32_t c = b - a;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Sub8)
		{
			uint64_t a = stack[top], b = stack[top - 1];
			uint64_t c = b - a;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Sub16)
		{
			stack[top - 1] -= stack[top];
			top -= 1;
			pc += 1;
			continue;	
		}
		else if(op == Xor1)
		{
			uint8_t a = stack[top], b = stack[top - 1];
			uint8_t c = a ^ b;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Xor2)
		{
			uint16_t a = stack[top], b = stack[top - 1];
			uint16_t c = a ^ b;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		} 
		else if(op == Xor4)
		{
			uint32_t a = stack[top], b = stack[top - 1];
			uint32_t c = a ^ b;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Xor8)
		{
			uint64_t a = stack[top], b = stack[top - 1];
			uint64_t c = a ^ b;
			stack[top - 1] = c;
			top -= 1;
			pc += 1;
			continue;
		}
		else if(op == Xor16)
		{
			stack[top - 1] ^= stack[top];
			top -= 1;
			pc += 1;
			continue;	
		}
		else if(op == Not1)
		{
			uint8_t a = stack[top];
			stack[top] = ~ a;
			pc += 1;
			continue;
		}
		else if(op == Not2)
		{
			uint16_t a = stack[top];
			stack[top] = ~ a;
			pc += 1;
			continue;
		}
		else if(op == Not4)
		{
			uint32_t a = stack[top];
			stack[top] = ~ a;
			pc += 1;
			continue;
		}
		else if(op == Not8)
		{
			uint64_t a = stack[top];
			stack[top] = ~ a;
			pc += 1;
			continue;
		}
		else if(op == Not16)
		{
			stack[top] = ~ stack[top];
			pc += 1;
			continue;
		}
		else if(op == Sext1)
		{
			int8_t v = stack[top];
			uint8_t s = opcode[pc + 1];
			if(s == 2)
			{
				int16_t xv = v;
				stack[top] = xv & 0xffff;
			}
			else if(s == 4)
			{
				int32_t xv = v;
				stack[top] = xv & 0xffffffff;
			}
			else if(s == 8)
			{
				int64_t xv = v;
				stack[top] = xv & 0xffffffffffffffff;
			}
			else if(s == 16)
			{
				__int128 xv = v;
				stack[top] = xv;
			}
			pc += 2;
			continue;
		}
		else if(op == Sext2)
		{
			int16_t v = stack[top];
			uint8_t s = opcode[pc + 1];
			if(s == 4)
			{
				int32_t xv = v;
				stack[top] = xv & 0xffffffff;
			}
			else if(s == 8)
			{
				int64_t xv = v;
				stack[top] = xv & 0xffffffffffffffff;
			}
			else if(s == 16)
			{
				__int128 xv = v;
				stack[top] = xv;
			}
			pc += 2;
			continue;
		}
		else if(op == Sext4)
		{
			int32_t v = stack[top];
			uint8_t s = opcode[pc + 1];
			if(s == 8)
			{
				int64_t xv = v;
				stack[top] = xv & 0xffffffffffffffff;
			}
			else if(s == 16)
			{
				__int128 xv = v;
				stack[top] = xv;
			}
			pc += 2;
			continue;
		}
		else if(op == Sext8)
		{
			int64_t v = stack[top];
			uint8_t s = opcode[pc + 1];
			if(s == 16)
			{
				__int128 xv = v;
				stack[top] = xv;
			}
			pc += 2;
		}
		else if(op == Sext16)
		{
			pc += 2;
			continue;
		}
        else if(op == Zext1)
		{
			int8_t v = stack[top];
			uint8_t s = opcode[pc + 1];
			if(s == 2)
			{
				int16_t xv = v;
				stack[top] = xv & 0xffff;
			}
			else if(s == 4)
			{
				int32_t xv = v;
				stack[top] = xv & 0xffffffff;
			}
			else if(s == 8)
			{
				int64_t xv = v;
				stack[top] = xv & 0xffffffffffffffff;
			}
			else if(s == 16)
			{
				__int128 xv = v;
				stack[top] = xv;
			}
			pc += 2;
			continue;
		}
		else if(op == Zext2)
		{
			int16_t v = stack[top];
			uint8_t s = opcode[pc + 1];
			if(s == 4)
			{
				uint32_t xv = v;
				stack[top] = xv & 0xffffffff;
			}
			else if(s == 8)
			{
				uint64_t xv = v;
				stack[top] = xv & 0xffffffffffffffff;
			}
			else if(s == 16)
			{
				unsigned __int128 xv = v;
				stack[top] = xv;
			}
			pc += 2;
			continue;
		}
		else if(op == Zext4)
		{
			int32_t v = stack[top];
			uint8_t s = opcode[pc + 1];
			if(s == 8)
			{
				uint64_t xv = v;
				stack[top] = xv & 0xffffffffffffffff;
			}
			else if(s == 16)
			{
				unsigned __int128 xv = v;
				stack[top] = xv;
			}
			pc += 2;
			continue;
		}
		else if(op == Zext8)
		{
			int64_t v = stack[top];
			uint8_t s = opcode[pc + 1];
			if(s == 16)
			{
				unsigned __int128 xv = v;
				stack[top] = xv;
			}
			pc += 2;
		}
		else if(op == Zext16)
		{
			pc += 2;
			continue;
		}
		else if(op == Hlt)
		{
			break;	
		}
		//printf("unknown opcode %d at %d",op,pc);
		//exit(0);
		return;
			
	}
	/*printf("dump stack\n");
	for(int i = top; i >= 1; i--)
	{
		uint8_t *ptr = (uint8_t*)&stack[i];
		for(int j = 0; j < 16; j++)
			printf("%02X ", ptr[j]);
		puts("");
	}
	printf("dump tmp\n");
	for(int i = 0; i <=4; i++)
	{
		uint8_t *ptr = (uint8_t*)&tmp[i];
		for(int j = 0; j < 16; j++)
			printf("%02X ", ptr[j]);
		puts("");
	}*/
	
} 

int main()
{

	return 0;
}

