#include<cstdio>
#include<cstdlib>
void RC4(unsigned char* C,unsigned char* key,int len_c,int len_key)
{
	int S[256];
	int T[256];
	int  count = 0;
	count = len_key;
	
	for(int i = 0; i < 256; i++)
	{
	    S[i] = i;
	    int tmp = i % count;
	    T[i] = key[tmp];
	}
	int j = 0;
	
	for(int  i = 0; i < 256; i++)
	{
	    j = (j + S[i] + T[i]) &0xff;
	    //printf("%x ", j);
	    int tmp;
	    tmp = S[j];
	    S[j] = S[i];
	    S[i] = tmp;
	}
	//for(int  i = 0; i < 256; i++)
	//	printf("%x%c", S[i], i % 16 == 15 ? '\n' : ' ');
	int length = 0;
	length = len_c;
	
	int i;
	i=0,j=0;
	for(int p = 0; p < length; p++)
	{
	
	    i = (i + 1) & 0xff;
	    j = (j + S[i]) & 0xff;
	    
	    int tmp;
	    tmp = S[j];
	    S[j] = S[i];
	    S[i] = tmp;
	    
	    char ss=(S[i] + S[j]);
	    
	    int k = S[ss]; // Fail~!
	    //printf("%x\n", ss);
	    C[p]=C[p]^k;
	}

}
/*
24 39 BD 4E 70 F8 6C AB C4 7E 8F 0D 23 26 4C 23
0C 66 6C 58 9A 68 31 92 15 D0 26 00 9B 1B 80 58
C8 49 22 EB 43 D1 8A E0 B2 57 E4 DD 29 DB AC 89
D3 7B 25 01 09 67 06 33 19 FA 6C 5A 15 04 4B 17
*/
long long mod(long long a,long long b)
{
	int c = a % b;
	return c++;
}
unsigned char key[]="bupt_derevm";
unsigned char test_case[64];
int main()
{
	printf("%p %d\n", RC4, mod(123124423,123));
	for(int i = 0; i < 64; i++)
		test_case[i] = i;
	RC4(test_case, key,64,11);
	for(int i = 0; i < 4;i++)
	{
		for(int j = 0; j < 16; j++)
			printf("%02X ", test_case[16 * i + j]);
		puts("");
	}
}