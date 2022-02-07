#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <bitset>
using namespace std;


char* iFilename;
char* oFilename;
char* gen;
char* dw;

int bitCount = 0;
int cwdNum = 0;
int dwSize = 0;
string binStr = "";
string generator = "";

int binaryToDecimal(string n)
{
	string num = n;
	int ans = 0;

	// Initializing base value to 1, i.e 2^0
	int curr = 1;

	int len = num.length();
	for (int i = len - 1; i >= 0; i--) {
		if (num[i] == '1')
			ans += curr;
		curr = curr * 2;
	}

	return ans;
}

string doXOR(string x, string y) {
	string res = "";
	int n = y.length();
	for (int i = 1; i < n; i++) {
		if (x[i] != y[i]) res += "1";
		else res += "0";
	}
	return res;
}

string mod2div(string target, string divisor)
{

	int curIdx = divisor.length();

	string res = target.substr(0, curIdx);

	int targetLen = target.length();

	while (curIdx < targetLen)
	{
		if (res[0] == '1')
			res = doXOR(divisor, res) + target[curIdx];
		else
			res = doXOR(std::string(curIdx, '0'), res) + target[curIdx];

		curIdx += 1;
	}

	if (res[0] == '1')
		res = doXOR(divisor, res);
	else
		res = doXOR(std::string(curIdx, '0'), res);

	return res;
}


void do_CRC_4() {
	// do crc of dwsize 4
	//TODO : divide binStr into dwSize, and change each dataword to codeword
	int iter = binStr.length() / 8; // divide by 8, and there is two dataword

	// write in binary mode

	unsigned char* buf = new unsigned char[binStr.length()];
	memset(buf, '\0', sizeof(buf));
	//FILE* oFP = fopen("encoded_4.enc", "ab");
	fstream outFile;
	string buffer;
	outFile.open(oFilename, ios::app | ios::binary);

	for (int i = 0; i < iter; i++) { // 2 dataword
		string dataword1 = "";	string dataword2 = "";
		string codeword1 = "";	string codeword2 = "";

		dataword1 = binStr.substr(8 * i, 4); // 0, 8, 16, ..
		dataword2 = binStr.substr(8 * i + 4, 4); // 4, 12, 16 ...

		//cout << "dataword1 : " << dataword1 << endl;
		//cout << "dataword2 : " << dataword2 << endl;

		// Change two dataword to codeword
		codeword1 = dataword1; codeword2 = dataword2;

		// First
		// TODO : do mod2Div and get rmn
		for (int j = 0; j < generator.length() - 1; j++)
			dataword1 += "0";
		for (int j = 0; j < generator.length() - 1; j++)
			dataword2 += "0";

		string rm1 = mod2div(dataword1, generator);
		string rm2 = mod2div(dataword2, generator);

		codeword1 += rm1; codeword2 += rm2;

		// Get two codewords
		//cout << "rm1 : " << rm1 << ", rm2 : " << rm2 << endl;
		//cout << "codeword1 : " << codeword1 << endl;
		//cout << "codeword2 : " << codeword2 << endl;

		//unsigned char c1 = stoi(codeword1);
		buffer.append(codeword1);
		//buf[k++] = c1;

		//unsigned char c2 = stoi(codeword2);
		buffer.append(codeword2);
		//buf[k++] = c2;

		//outFile.write((char*)&c1, sizeof(c1));
		//outFile.write((char*)&c2, sizeof(c2));


		int cw1Len = codeword1.length();
		int cw2Len = codeword2.length();
		bitCount += cw1Len + cw2Len;
		//printf("End of Iteration : %d\n", i);
		cwdNum += 2;
	}

	//cout << buffer << endl;

	//cout << "total bits : " << bitCount << endl;
	unsigned char paddedNum = 0;
	int temp = bitCount / 8;
	int temp2 = 8 * (temp + 1);
	paddedNum = temp2 - bitCount;

	// padd #paddedNum in front
	for (int i = 0; i < paddedNum; i++)
		buffer.insert(0, "0");

	string paddedInfo = std::bitset<8>(paddedNum).to_string();
	buffer.insert(0, paddedInfo);

	//cout << "check buffer" << endl;
	//cout << "bufferlen : " << buffer.length() << endl;
	// now translate buffer to const char*
	//const unsigned char* constStr = reinterpret_cast<const unsigned char*> (buffer.c_str());
	//outFile.write((char*)constStr, sizeof(constStr));

	// we got string
	int newIter = buffer.length() / 8;
	for (int i = 0; i < newIter; i++) {
		string cur = buffer.substr(8 * i, 8);
		//unsigned char curChar = stoi(cur.c_str(), 0, 2);

		//TODO : replace stoi to another thing

		unsigned char curChar = binaryToDecimal(cur);

		outFile.write((char*)&curChar, sizeof(curChar));
	}

	//cout << "cwdNum : " << cwdNum << endl;
	//cout << "check the file " << endl;
}
void do_CRC_8() {
	// do crc of dwsize 8
	//TODO : divide binStr into dwSize, and change each dataword to codeword
	int iter = binStr.length() / 8; // divide by 8, and there is two dataword

	// write in binary mode


	fstream outFile;
	string buffer;
	outFile.open(oFilename, ios::app | ios::binary);

	//FILE* oFP = fopen("encoded_4.enc", "ab");
	for (int i = 0; i < iter; i++) { // anyway, we process data byte-by-byte
		string dataword = "";	string codeword = "";
		dataword = binStr.substr(8 * i, 8); // since dwSize==8
		// we got dataword
		//cout << "dataword : " << dataword << endl;
		codeword = dataword;
		// padd 0 at the end
		for (int j = 0; j < generator.length() - 1; j++)
			dataword += "0";
		// do mod2Div
		string rm = mod2div(dataword, generator);
		codeword += rm;

		//unsigned char c = stoi(codeword);
		buffer.append(codeword);

		int cwLen = codeword.length();
		bitCount += cwLen;
		cwdNum++;
	}

	//cout << buffer << endl;

	//cout << "total bits : " << bitCount << endl;
	unsigned char paddedNum = 0;
	int temp = bitCount / 8;
	int temp2 = 8 * (temp + 1);
	paddedNum = temp2 - bitCount;

	// padd #paddedNum in front
	for (int i = 0; i < paddedNum; i++)
		buffer.insert(0, "0");

	string paddedInfo = std::bitset<8>(paddedNum).to_string();
	buffer.insert(0, paddedInfo);

	//cout << "check buffer" << endl;
	//cout << "bufferlen : " << buffer.length() << endl;
	// now translate buffer to const char*
	//const unsigned char* constStr = reinterpret_cast<const unsigned char*> (buffer.c_str());
	//outFile.write((char*)constStr, sizeof(constStr));

	// we got string
	int newIter = buffer.length() / 8;
	for (int i = 0; i < newIter; i++) {
		string cur = buffer.substr(8 * i, 8);
		//unsigned char curChar = stoi(cur.c_str(), 0, 2);
		unsigned char curChar = binaryToDecimal(cur);
		outFile.write((char*)&curChar, sizeof(curChar));
	}

	//cout << "cwdNum : " << cwdNum << endl;

	//cout << "check the file " << endl;
}
int main(int argc, char* argv[]) {

	if (argc != 5) {
		printf("usage: ./crc_encoder input_file output_file generator dataword_size\n");
		exit(0);
	}
	iFilename = new char[32];
	oFilename = new char[32];
	gen = new char[10];
	dw=new char[2];
	strcpy(iFilename, argv[1]);
	strcpy(oFilename, argv[2]);
	strcpy(gen, argv[3]);
	strcpy(dw,argv[4]);

	FILE* iFP=fopen(iFilename,"rb");
	FILE* oFP=fopen(oFilename,"wb");
	if(iFP==NULL){
		printf("input file open error.\n");
		exit(0);
	}
	fclose(iFP);
	if(oFP==NULL){
		printf("input file open error.\n");
		exit(0);
	}
	fclose(oFP);

	// get dwSize
	dwSize=atoi(dw);
	if(!(dwSize==4 || dwSize==8)){
		printf("dataword size must be 4 or 8.\n");
		exit(0);
	}
	// get generator
	std::string tempGen(gen);
	generator=tempGen;

	//printf("dwSize : %d\n",dwSize);
	//cout << "generator : " << generator << endl;

	ifstream fin(iFilename, ios::binary);
	ostringstream ostrm;
	ostrm << fin.rdbuf();
	string res = ostrm.str();
	//cout << res << endl;

	for (std::size_t i = 0; i < res.size(); ++i)
	{
		binStr += bitset<8>(res.c_str()[i]).to_string();
	}

	//cout << binStr << endl;
	//printf("\nYes\n");

	//dwSize = 4;
	//generator = "10101";

	if (dwSize == 4) {
		do_CRC_4();
	}
	if (dwSize == 8) {
		do_CRC_8();
	}
	//printf("cwdNum : %d\n", cwdNum);
	return 0;
}
