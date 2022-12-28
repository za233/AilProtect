#include<cstdio>
#include<cstdint>
#define R(a,b) (((a) << (b)) | ((a) >> (32 - (b))))
void salsa20_word_specification(uint32_t out[16],uint32_t in[16])
{
    int i;
    uint32_t x[16];
    for (i = 0;i < 16;++i) x[i] = in[i];
    for (i = 20;i > 0;i -= 2) 
    {
        x[ 4] ^= R(x[ 0]+x[12], 7);  x[ 8] ^= R(x[ 4]+x[ 0], 9);
        x[12] ^= R(x[ 8]+x[ 4],13);  x[ 0] ^= R(x[12]+x[ 8],18);
        x[ 9] ^= R(x[ 5]+x[ 1], 7);  x[13] ^= R(x[ 9]+x[ 5], 9);
        x[ 1] ^= R(x[13]+x[ 9],13);  x[ 5] ^= R(x[ 1]+x[13],18);
        x[14] ^= R(x[10]+x[ 6], 7);  x[ 2] ^= R(x[14]+x[10], 9);
        x[ 6] ^= R(x[ 2]+x[14],13);  x[10] ^= R(x[ 6]+x[ 2],18);
        x[ 3] ^= R(x[15]+x[11], 7);  x[ 7] ^= R(x[ 3]+x[15], 9);
        x[11] ^= R(x[ 7]+x[ 3],13);  x[15] ^= R(x[11]+x[ 7],18);
        x[ 1] ^= R(x[ 0]+x[ 3], 7);  x[ 2] ^= R(x[ 1]+x[ 0], 9);
        x[ 3] ^= R(x[ 2]+x[ 1],13);  x[ 0] ^= R(x[ 3]+x[ 2],18);
        x[ 6] ^= R(x[ 5]+x[ 4], 7);  x[ 7] ^= R(x[ 6]+x[ 5], 9);
        x[ 4] ^= R(x[ 7]+x[ 6],13);  x[ 5] ^= R(x[ 4]+x[ 7],18);
        x[11] ^= R(x[10]+x[ 9], 7);  x[ 8] ^= R(x[11]+x[10], 9);
        x[ 9] ^= R(x[ 8]+x[11],13);  x[10] ^= R(x[ 9]+x[ 8],18);
        x[12] ^= R(x[15]+x[14], 7);  x[13] ^= R(x[12]+x[15], 9);
        x[14] ^= R(x[13]+x[12],13);  x[15] ^= R(x[14]+x[13],18);
    }
    for (i = 0;i < 16;++i) out[i] = x[i] + in[i];
}
uint32_t test_case[16];
//e1a0ebe0 06032a02 1ec19924 91ffb36e 83068335 a7c06e2c 75b43867 ccf8517e f61ba820 ab1b8972 ec9ed7f6 478636b9 745dd6ad 8bbe722d a1cc9bdf 5fc70cf0
int main()
{
    for(int i = 0; i < 16; i++)
    	test_case[i] = i;
    salsa20_word_specification(test_case,test_case);
    for(int i = 0; i < 16; i++)
    	printf("%08x ", test_case[i]);
    return 0;
}