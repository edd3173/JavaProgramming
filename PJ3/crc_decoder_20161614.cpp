#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <bitset>
using namespace std;

char* iFilename;
char* oFilename;
char* rFilename;
char* gen;
char* dw;

int errNum = 0;
int codewordNum = 0;

int dwSize = 0;
string binStr = "";
string generator = "";
string buffer = "";
int chunkLen = 0;
int buffLen = 0;

int binaryToDecimal(string n)
{
	string num = n;
	int ans = 0;


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
	
	while (curIdx < targetLen){
		if (res[0] == '0')
			res = doXOR(std::string(curIdx, '0'), res) + target[curIdx];
		else
			res = doXOR(divisor, res) + target[curIdx];

		curIdx++;
	}
	

	if (res[0] == '0')
		res = doXOR(std::string(curIdx, '0'), res);
	else
		res = doXOR(divisor, res);

	return res;
}

void Decode(int chunkNum) {
	if (dwSize == 4) {
		int iter = chunkNum / 2; // must be 42
		fstream outFile;
		//outFile.open("decoded_4.enc", ios::app | ios::binary);
		outFile.open(oFilename, ios::app | ios::binary);

		string BUFFER = "";
		for (int i = 0; i < iter; i++) { // 
			// read from string with size of chunk
			string codeword1 = ""; // current chunk
			string codeword2 = "";
			// is this right? no, since in dwsize 2, there is two
			codeword1 = buffer.substr(chunkLen * 2 * i, chunkLen);
			codeword2 = buffer.substr(chunkLen * (2 * i + 1), chunkLen);

			// curr string is dataword+rm
			string dataword1 = ""; string rem1 = "";
			string dataword2 = ""; string rem2 = "";

			dataword1 = codeword1.substr(0, dwSize);
			rem1 = codeword1.substr(dwSize, generator.size() - 1);

			dataword2 = codeword2.substr(0, dwSize);
			rem2 = codeword2.substr(dwSize, generator.size() - 1);

			// let's see if ans is correct
			string ans = "";
			for (int j = 0; j < generator.size() - 1; j++)
				ans.insert(0, "0");

			// divid mod2div
			string res1 = mod2div(codeword1, generator);
			if (res1 == ans) {
				//printf("For %d-th iter, cw1 is CORRECT!\n", i);
			}
			else {
				errNum++;
			}
			string res2 = mod2div(codeword2, generator);
			if (res2 == ans) {
				//printf("For %d-th iter, cw2 is CORRECT!\n", i);
			}
			else {
				errNum++;
			}
			codewordNum += 2;

			// restore dataword and write in outputfile
			BUFFER.append(dataword1); // 4bit
			BUFFER.append(dataword2); // 4it

			//cout << "Appending dataword1 : " << dataword1 << endl;
			//cout << "Appending dataword2 : " << dataword1 << endl;
		} // End of Iteration

		// we got buffer, filled with datawords.
		// we now write byte-by-byte
		//printf("size of buffer : %d\n", BUFFER.length());
		//cout << "codeWordNum : " << codewordNum << endl;
		int newIter = BUFFER.length() / 8;
		for (int i = 0; i < newIter; i++) {
			string cur = BUFFER.substr(8 * i, 8);
			//unsigned char curChar = stoi(cur.c_str(), 0, 2);
			unsigned char curChar = binaryToDecimal(cur);
			outFile.write((char*)&curChar, sizeof(curChar));
		}

	} // End of dwSize==4

	else if (dwSize == 8) { // if dwSize==8

		int iter = chunkNum;
		fstream outFile;
		//outFile.open("decoded_8.enc", ios::app | ios::binary);
		outFile.open(oFilename, ios::app | ios::binary);

		string BUFFER = "";
		for (int i = 0; i < iter; i++) { // 
			// read from string with size of chunk
			string codeword = ""; // current chunk
			// is this right? no, since in dwsize 2, there is two
			codeword = buffer.substr(chunkLen * i, chunkLen);

			// curr string is dataword+rm
			string dataword = ""; string rem = "";

			dataword = codeword.substr(0, dwSize);
			rem = codeword.substr(dwSize, generator.size() - 1);

			// let's see if ans is correct
			string ans = "";
			for (int j = 0; j < generator.size() - 1; j++)
				ans.insert(0, "0");

			// divid mod2div
			string res1 = mod2div(codeword, generator);
			if (res1 == ans) {
				//printf("For %d-th iter, codeword is CORRECT!\n", i);
			}
			else {
				errNum++;
			}
			codewordNum++;

			// restore dataword and write in outputfile
			BUFFER.append(dataword); // 4it

			//cout << "Appending dataword : " << dataword << endl;
		} // End of Iteration

		// we got buffer, filled with datawords.
		// we now write byte-by-byte
		//printf("size of buffer : %d\n", BUFFER.length());
		//cout << "codeWordNum : " << codewordNum << endl;
		int newIter = BUFFER.length() / 8;
		for (int i = 0; i < newIter; i++) {
			string cur = BUFFER.substr(8 * i, 8);
			//unsigned char curChar = stoi(cur.c_str(), 0, 2);
			unsigned char curChar = binaryToDecimal(cur);
			outFile.write((char*)&curChar, sizeof(curChar));
		}
	}
}

int main(int argc, char* argv[]) {

	if (argc != 6) {
		printf("./crc_decoder input_file output_file result_file generator dataword_size\n");
		exit(0);
	}
	iFilename = new char[32];
	oFilename = new char[32];
	rFilename = new char[32];
	gen = new char[10];
	dw=new char[2];

	strcpy(iFilename, argv[1]);
	strcpy(oFilename, argv[2]);
	strcpy(rFilename, argv[3]);
	strcpy(gen, argv[4]);
	strcpy(dw,argv[5]);

	FILE* iFP=fopen(iFilename,"rb");
	FILE* oFP=fopen(oFilename,"wb");
	FILE* rFP=fopen(rFilename,"w");

	if(iFP==NULL){
		printf("input file open error.\n");
		exit(0);
	}
	fclose(iFP);
	if(oFP==NULL){
		printf("output file open error.\n");
		exit(0);
	}
	fclose(oFP);

	if(rFP==NULL){
		printf("result file open error.\n");
		exit(0);
	}


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
	//ifstream fin("codedstream8.tx", ios::binary);
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

	// preprocessing
	string paddedBits = binStr.substr(0, 8);
	//int paddedNum = stoi(paddedBits, 0, 2);
	int paddedNum = binaryToDecimal(paddedBits);

	// need to delete front zeros of binStr. how long?
	// 8+paddedNum
	int zeros = 8 + paddedNum;
	buffer = binStr.substr(zeros, binStr.length());

	// we have pure datas. let's check the length
	buffLen = buffer.length();
	//cout << "bufLen : " << buffLen << endl;

	chunkLen = dwSize + generator.length() - 1;
	//cout << "chunkLen : " << chunkLen << endl;
	int chunkNum = 0;
	chunkNum = buffLen / chunkLen; // 84
	//cout << "chunkNum : " << chunkNum << endl;

	Decode(chunkNum);
	fprintf(rFP,"%d %d\n",codewordNum,errNum);
	fclose(rFP);
	return 0;
}
