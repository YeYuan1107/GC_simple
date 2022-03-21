#include<iostream>
#include<fstream>
#include<bitset>
#include<string>
#include<stdlib.h>
#include<time.h>
#include<utility>
#include<vector>

using namespace std;

bitset<48> subkey[16];
bitset<64> w_key[19][2];
struct node
{
	int in1,in2,out,type;
	vector<pair<bitset<64>,bitset<128>>> table[4];
	void init(int a,int b,int c,int d){
		this->in1=a;
		this->in2=b;
		this->out=c;
		this->type=d;
	}
};

struct node door[9];


// Initial Permutation
int IP[] = {58, 50, 42, 34, 26, 18, 10, 2,
			60, 52, 44, 36, 28, 20, 12, 4,
			62, 54, 46, 38, 30, 22, 14, 6,
			64, 56, 48, 40, 32, 24, 16, 8,
			57, 49, 41, 33, 25, 17, 9,  1,
			59, 51, 43, 35, 27, 19, 11, 3,
			61, 53, 45, 37, 29, 21, 13, 5,
			63, 55, 47, 39, 31, 23, 15, 7};

int IP_inv[] = {40, 8, 48, 16, 56, 24, 64, 32,
			    39, 7, 47, 15, 55, 23, 63, 31,
			    38, 6, 46, 14, 54, 22, 62, 30,
			    37, 5, 45, 13, 53, 21, 61, 29,
			    36, 4, 44, 12, 52, 20, 60, 28,
			    35, 3, 43, 11, 51, 19, 59, 27,
			    34, 2, 42, 10, 50, 18, 58, 26,
			    33, 1, 41,  9, 49, 17, 57, 25};
//KEY: 64 to 48
int PC_1[] = {57, 49, 41, 33, 25, 17, 9,
			   1, 58, 50, 42, 34, 26, 18,
			  10,  2, 59, 51, 43, 35, 27,
			  19, 11,  3, 60, 52, 44, 36,
              63, 55, 47, 39, 31, 23, 15,
			   7, 62, 54, 46, 38, 30, 22,
			  14,  6, 61, 53, 45, 37, 29,
			  21, 13,  5, 28, 20, 12,  4};

int PC_2[] = {14, 17, 11, 24,  1,  5,
			   3, 28, 15,  6, 21, 10,
			  23, 19, 12,  4, 26,  8,
			  16,  7, 27, 20, 13,  2,
			  41, 52, 31, 37, 47, 55,
			  30, 40, 51, 45, 33, 48,
			  44, 49, 39, 56, 34, 53,
			  46, 42, 50, 36, 29, 32};
//Shift round
int shift[] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

int Ex[] = {32,  1,  2,  3,  4,  5,
		     4,  5,  6,  7,  8,  9,
		     8,  9, 10, 11, 12, 13,
		    12, 13, 14, 15, 16, 17,
            16, 17, 18, 19, 20, 21,
		    20, 21, 22, 23, 24, 25,
		    24, 25, 26, 27, 28, 29,
		    28, 29, 30, 31, 32,  1};
//S BOX
int S_BOX[8][4][16] = {
	{  
		{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},  
		{0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},  
		{4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0}, 
		{15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13} 
	},
	{  
		{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},  
		{3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5}, 
		{0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},  
		{13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}  
	}, 
	{  
		{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},  
		{13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},  
		{13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},  
		{1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}  
	}, 
	{  
		{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},  
		{13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},  
		{10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},  
		{3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}  
	},
	{  
		{2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},  
		{14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},  
		{4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},  
		{11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}  
	},
	{  
		{12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},  
		{10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},  
		{9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},  
		{4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}  
	}, 
	{  
		{4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},  
		{13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},  
		{1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},  
		{6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}  
	}, 
	{  
		{13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},  
		{1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},  
		{7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},  
		{2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}  
	} 
};

int P[] = {16,  7, 20, 21,
		   29, 12, 28, 17,
		    1, 15, 23, 26,
		    5, 18, 31, 10,
		    2,  8, 24, 14,
		   32, 27,  3,  9,
		   19, 13, 30,  6,
		   22, 11,  4, 25};


bitset<32> f(bitset<32> R, bitset<48> k)
{
	bitset<48> expandR;
	// 第一步：扩展置换，32 -> 48
	for(int i=0; i<48; ++i)
		expandR[47-i] = R[32-Ex[i]];
	// 第二步：异或
	expandR = expandR ^ k;
	// 第三步：查找S_BOX置换表
	bitset<32> output;
	int x = 0;
	for(int i=0; i<48; i=i+6)
	{
		int row = expandR[47-i]*2 + expandR[47-i-5];
		int col = expandR[47-i-1]*8 + expandR[47-i-2]*4 + expandR[47-i-3]*2 + expandR[47-i-4];
		int num = S_BOX[i/6][row][col];
		bitset<4> binary(num);
		output[31-x] = binary[3];
		output[31-x-1] = binary[2];
		output[31-x-2] = binary[1];
		output[31-x-3] = binary[0];
		x += 4;
	}
	// 第四步：P-置换，32 -> 32
	bitset<32> tmp = output;
	for(int i=0; i<32; ++i)
		output[31-i] = tmp[32-P[i]];
	return output;
}

//移位函数
bitset<28> leftShift(bitset<28> k, int shift)
{
	bitset<28> tmp = k;
	for(int i=27; i>=0; --i)
	{
		if(i-shift<0)
			k[i] = tmp[i-shift+28];
		else
			k[i] = tmp[i-shift];
	}
	return k;
}

void generateKeys(bitset<64> key) 
{
	bitset<56> realKey;
	bitset<28> left;
	bitset<28> right;
	bitset<48> compressKey;
	// 去掉奇偶标记位，将64位密钥变成56位
	for (int i=0; i<56; ++i)
		realKey[55-i] = key[64 - PC_1[i]];
	// 生成子密钥，保存在 subKeys[16] 中
	for(int round=0; round<16; ++round) 
	{
		// 前28位与后28位
		for(int i=28; i<56; ++i)
			left[i-28] = realKey[i];
		for(int i=0; i<28; ++i)
			right[i] = realKey[i];
		// 左移
		left = leftShift(left, shift[round]);
		right = leftShift(right, shift[round]);
		// 压缩置换，由56位得到48位子密钥
		for(int i=28; i<56; ++i)
			realKey[i] = left[i-28];
		for(int i=0; i<28; ++i)
			realKey[i] = right[i];
		for(int i=0; i<48; ++i)
			compressKey[47-i] = realKey[56 - PC_2[i]];
		subkey[round] = compressKey;
	}
}

//DES 加密
bitset<64> DES(bitset<64>& plain, bitset<64> key)
{
	bitset<64> cipher;
	bitset<64> currentBits;
	bitset<32> left;
	bitset<32> right;
	bitset<32> newLeft;
	generateKeys(key);
	// 第一步：初始置换IP
	for(int i=0; i<64; ++i)
		currentBits[63-i] = plain[64-IP[i]];
	// 第二步：获取 Li 和 Ri
	for(int i=32; i<64; ++i)
		left[i-32] = currentBits[i];
	for(int i=0; i<32; ++i)
		right[i] = currentBits[i];
	// 第三步：共16轮迭代
	for(int round=0; round<16; ++round)
	{
		newLeft = right;
		right = left ^ f(right,subkey[round]);
		left = newLeft;
	}
	// 第四步：合并L16和R16，注意合并为 R16L16
	for(int i=0; i<32; ++i)
		cipher[i] = left[i];
	for(int i=32; i<64; ++i)
		cipher[i] = right[i-32];
	// 第五步：结尾置换IP-1
	currentBits = cipher;
	for(int i=0; i<64; ++i)
		cipher[63-i] = currentBits[64-IP_inv[i]];
	// 返回密文
	return cipher;
}

bitset<128> PRG_G(bitset<64> k)
{
	bitset<128> ans;
	bitset<64> bitset1(1);
	bitset<64> bitset2(2);
	bitset<64> temp1 = DES(bitset1,k);
	bitset<64> temp2 = DES(bitset2,k);\
	for(int i=0;i<128;i++)
	{
		if(i<64)
		{
			ans[i]=temp1[i];
		}
		else
		{
			ans[i]=temp2[i-64];
		}
	}
	return ans;	
}

bitset<64> K_ge(bitset<64>& x, bitset<64> k)
{
	bitset<64> ans;
	bitset<64> temp = k;
	for(int i=0;i<64;i++)
	{
		bitset<128> temp1=PRG_G(temp);
		if(x[i]==0)
		{
			for(int j=0;j<64;j++)
			{
				ans[j]=temp1[j];
			}
		}
		else
		{
			for(int j=0;j<64;j++)
			{
				ans[j]=temp1[j+64];
			}
		}
		temp = ans;	
	}
	return ans;
}

bitset<128> PRF_F(bitset<64>& x, bitset<64> k)
{
	bitset<128> ans;
	ans=PRG_G(K_ge(x,k));
	return ans;
}

pair<bitset<64>, bitset<128>> Enc(bitset<64> k,bitset<64> x)
{
	srand((int)time(0));
	bitset<64> r;
	for(int i=0;i<64;i++)
	{
		r[i]=rand()%2;
	}
	bitset<128> Fr = PRF_F(r,k);
	bitset<128> x_aug;
	for(int i=0;i<64;i++)
	{
		x_aug[i]=x[i];
	} 
	for(int i=64;i<128;i++)
	{
		x_aug[i]=0;
	}
	pair<bitset<64>, bitset<128>> code;
	code.first = r;
	code.second = x_aug ^ Fr;
	return code;
}

bitset<128> Dec(bitset<64> k, pair<bitset<64>,bitset<128>> code)
{
	bitset<128> Fr = PRF_F(code.first,k);
	bitset<128> ans = Fr ^ code.second;
	return ans;
}

void init()
{
	srand((int)time(0));
	for(int i=0;i<19;i++)
	{
		for(int j=0;j<2;j++)
		{
			for(int k=0;k<64;k++)
			{
				w_key[i][j][k]=rand()%2;
			}
		}
	}
	door[0].init(0,2,10,0);
	door[1].init(1,3,11,1);
	door[2].init(4,6,12,0);
	door[3].init(5,7,13,1);
	door[4].init(8,9,14,0);
	door[5].init(13,14,15,0);
	door[6].init(12,15,16,1);
	door[7].init(11,16,17,0);
	door[8].init(10,17,18,1);
	for(int i=0;i<9;i++)
	{
		if(door[i].type==0)
		{
			pair<bitset<64>,bitset<128>> temp1;
			pair<bitset<64>,bitset<128>> temp2;
			temp1 = Enc(w_key[door[i].in1][0],w_key[door[i].out][0]);
			temp2 = Enc(w_key[door[i].in2][0],temp1.first);
			temp1.first=0;
			door[i].table[0].push_back(temp2);
			door[i].table[0].push_back(temp1);
			temp1 = Enc(w_key[door[i].in1][0],w_key[door[i].out][0]);
			temp2 = Enc(w_key[door[i].in2][1],temp1.first);
			temp1.first=0;
			door[i].table[1].push_back(temp2);
			door[i].table[1].push_back(temp1);
			temp1 = Enc(w_key[door[i].in1][1],w_key[door[i].out][0]);
			temp2 = Enc(w_key[door[i].in2][0],temp1.first);
			temp1.first=0;
			door[i].table[2].push_back(temp2);
			door[i].table[2].push_back(temp1);
			temp1 = Enc(w_key[door[i].in1][1],w_key[door[i].out][1]);
			temp2 = Enc(w_key[door[i].in2][1],temp1.first);
			temp1.first=0;
			door[i].table[3].push_back(temp2);
			door[i].table[3].push_back(temp1);
		}
		else
		{
			pair<bitset<64>,bitset<128>> temp1;
			pair<bitset<64>,bitset<128>> temp2;
			temp1 = Enc(w_key[door[i].in1][0],w_key[door[i].out][0]);
			temp2 = Enc(w_key[door[i].in2][0],temp1.first);
			temp1.first=0;
			door[i].table[0].push_back(temp2);
			door[i].table[0].push_back(temp1);
			temp1 = Enc(w_key[door[i].in1][0],w_key[door[i].out][1]);
			temp2 = Enc(w_key[door[i].in2][1],temp1.first);
			temp1.first=0;
			door[i].table[1].push_back(temp2);
			door[i].table[1].push_back(temp1);
			temp1 = Enc(w_key[door[i].in1][1],w_key[door[i].out][1]);
			temp2 = Enc(w_key[door[i].in2][0],temp1.first);
			temp1.first=0;
			door[i].table[2].push_back(temp2);
			door[i].table[2].push_back(temp1);
			temp1 = Enc(w_key[door[i].in1][1],w_key[door[i].out][1]);
			temp2 = Enc(w_key[door[i].in2][1],temp1.first);
			temp1.first=0;
			door[i].table[3].push_back(temp2);
			door[i].table[3].push_back(temp1);
		}
	}
}

int verify(bitset<128> r)
{
	int sta = 0;
	for(int k =64;k<128;k++)
	{
		if(r[k]!=0)
		{
			sta=1;
			break;
		}
	}
	return sta;
}

bitset<64> cut(bitset<128> r)
{
	bitset<64> ans;
	for(int i=0;i<64;i++)
	{
		ans[i]=r[i];
	}
	return ans;
}

void output()
{
	for(int i=0;i<9;i++)
	{
		cout<<"door: "<< i <<" input_wire1: "<< door[i].in1 <<" input_wire2: "<<door[i].in2<<" output_wire1: "<<door[i].out<<endl;
		for(int j=0;j<4;j++)
			cout<<'('<<door[i].table[j][0].first<<','<<door[i].table[j][0].second<<','<<door[i].table[j][1].second<<')'<<endl;
	}
}

int main()
{
	init();
	//output();
	int maxn = 8;
	int a,b;
	cout<<"请输入a和b"<<endl;
	cin>>a>>b;
	if((a>=maxn)||(b>=maxn))
	{
		cout<<"超出最大值"<<endl;
		return 0;
	}
	vector<bitset<64>> t_key(19);
	bitset<3> num_a(a);
	bitset<3> num_b(b);
	t_key[0]=w_key[0][num_a[2]];
	t_key[1]=w_key[1][num_a[2]];
	t_key[2]=w_key[2][1-num_b[2]];
	t_key[3]=w_key[3][1-num_b[2]];
	t_key[4]=w_key[4][num_a[1]];
	t_key[5]=w_key[5][num_a[1]];
	t_key[6]=w_key[6][1-num_b[1]];
	t_key[7]=w_key[7][1-num_b[1]];
	t_key[8]=w_key[8][num_a[0]];
	t_key[9]=w_key[9][1-num_b[0]];
	for(int i=0;i<9;i++)
	{
		bitset<64> finl;
		for(int j=0;j<4;j++)
		{
			bitset<128> temp;
			temp = Dec(t_key[door[i].in2],door[i].table[j][0]);
			if(verify(temp))
			{
				continue;
			}
			bitset<64> temp_r = cut(temp);
			pair<bitset<64>,bitset<128>> temp_cp;
			temp_cp.first=temp_r;
			temp_cp.second=door[i].table[j][1].second;
			bitset<128> temp1;
			temp1 = Dec(t_key[door[i].in1],temp_cp);
			if(verify(temp1))
			{
				continue;
			}
			finl = cut(temp1);
		}
		t_key[door[i].out] = finl;
	}
	int ans=-1;
	int temp_st1 = 0;
	int temp_st2 = 0;

	for(int i=0;i<64;i++)
	{
		if(t_key[18][i]!=w_key[18][1][i])
		{
			temp_st1=1;
		}
	}
	for(int i=0;i<64;i++)
	{
		if(t_key[18][i]!=w_key[18][0][i])
		{
			temp_st2=1;
		}
	}
	if(temp_st1&&temp_st2)
	{
		cout<<"error"<<endl;
		return 0;
	}
	else
	{
		if(temp_st2)
		{
			ans=1;
		}
		if(temp_st1)
		{
			ans=0;
		}
	}

	cout<<ans<<endl;
	return 0;
}