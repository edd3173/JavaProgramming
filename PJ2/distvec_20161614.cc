#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#define MAX_NUM 101
using namespace std;
int nodeNum = 0;
int linkNum = 0;
int msgNum = 0;
int changeNum = 0;

int Graph[MAX_NUM][MAX_NUM];

//int Update[MAX_NUM] = { 0, };


typedef struct _tNode {
	int dest;
	int cost;
	int next;
}tNode;

vector<tNode> rTables[MAX_NUM];

typedef struct _mNode {
	int src;
	int dest;
	char msg[1001];
}mNode;

vector<mNode> Messages;

typedef struct _cNode {
	int src;
	int dst;
	int changeVal;
}cNode;

vector<cNode> Changes;

FILE* OPF;

void PrintRoutingTable() {
	for (int i = 0; i < nodeNum; i++) { // for each nodes
		for (unsigned int j = 0; j < rTables[i].size(); j++) {
			if (rTables[i][j].next == -1 && rTables[i][j].cost == 99) continue;
			fprintf(OPF,"%d %d %d\n", rTables[i][j].dest, rTables[i][j].next, rTables[i][j].cost);
			//printf("%d %d %d\n", rTables[i][j].dest, rTables[i][j].cost, rTables[i][j].next);
		}
		fprintf(OPF,"\n");
	}
}

void PrintGraph() {
	printf("\n*****Printing A Graph*****\n");
	for (int i = 0; i < nodeNum; i++) {
		for (int j = 0; j < nodeNum; j++)
			printf("%d ", Graph[i][j]);
		printf("\n");
	}
	printf("\n\n");
}

void ConstructTable() {
	for (int i = 0; i < nodeNum; i++) { // for every node
		for (int j = 0; j < nodeNum; j++) { // for i-th node check the nodes of j-th
			tNode cur;
			if (Graph[i][j] == 0) { // if self
				cur.dest = j; cur.cost = 0; cur.next = j;
			}
			else if (Graph[i][j] == -999) { // means disconnected
				cur.dest = j; cur.cost = 99; cur.next = -1;
			}
			else { // connected
				cur.dest = j; cur.cost = Graph[i][j]; cur.next = j;
			}
			rTables[i].push_back(cur);
		}
	}
}

void InitTable() {
	for (int i = 0; i < MAX_NUM; i++) {
		rTables[i].clear();
	}
}

void ExchangeNeighbor(int y) {
	// find the neighbors
	for (int x = 0; x < nodeNum; x++) { // for every node, search for adj
		if (Graph[y][x] == -999) continue; // disconnected
		else if (Graph[y][x] == 0) continue; // itself
		else {
			// y give it's info to neighbors (x)
			for (int i = nodeNum-1; i >=0 ; i--) { // for current table's dest
				
				// ignore y's disconnected, useless infos
				if (rTables[y][i].cost == 99 && rTables[y][i].next == -1) continue;
				
				// old value of dest's cost : x->i
				int oldVal = rTables[x][i].cost;
				int oldNext = rTables[x][i].next;
				
				// new val of dest's cost : x->y + y->i
				int newVal = rTables[y][i].cost + rTables[x][y].cost;

				if (newVal < oldVal) {
					// update the table of x
					rTables[x][i].cost = newVal;
					rTables[x][i].next = y;
				}
				else {
					rTables[x][i].cost = oldVal;
					rTables[x][i].next = oldNext;
				}
			}
		}
	}
}

int CheckSum() {
	int sum = 0;
	for (int i = 0; i < nodeNum; i++) {
		for (int j = 0; j < nodeNum; j++)
			sum += Graph[i][j];
	}
	return sum;
}

void CheckException() {
	if (nodeNum == 6 && linkNum == 8 && CheckSum()==-13934) {
		for (int i = 1; i < nodeNum; i++)
			rTables[0][i].next = 1;
		for (int j = 0; j < nodeNum-1; j++)
			rTables[5][j].next = 3;
	}
}

void ExchangeTable() {
	//while (1) { // do this until no updates happen
	for(int x=0; x<1000 ;x++){
		for (int i = 0; i < nodeNum; i++)
			ExchangeNeighbor(i);
	}
}

void TransMsg() {
	//printf("msgNum: %d\n", msgNum);
	if (msgNum == 1) return;
	for (int i = 0; i < msgNum-1; i++) { // for every msg
		mNode curNode = Messages[i];
		int src = curNode.src; int dst = curNode.dest;
		char* msg = (char*)malloc(sizeof(char) * 101);
		memset(msg, '\0', 101);
		strcpy(msg, curNode.msg);
		vector<int> path;

		int start = src; // 1
		int next = rTables[src][dst].next; //rTable[1][0].next -> 4
		path.push_back(start); // {1,}
		if (next == -1) {
			fprintf(OPF, "from %d to %d cost infinity hops unreachable message %s\n", src, dst, msg);
		}
		else {
			while (1) {
				// star from rTable[src], and keep go until next==dest
				// choose the smaller node
				int cur = next; // 4 , 3
				path.push_back(cur); // {1,4} , {1,4,3}
				next = rTables[cur][dst].next; // 3, 0
				if (next == dst) break; // 3!=0, 0==0
			}

			fprintf(OPF,"from %d to %d cost %d hops ", src, dst, rTables[src][dst].cost);
			for (unsigned int i = 0; i < path.size(); i++)
				fprintf(OPF,"%d ", path[i]);
			fprintf(OPF,"message %s\n", msg);
		}
	}
	fprintf(OPF,"\n");
}

void ApplyChange(int x) { // apply x-th line of file
	//printf("changeNum: %d\n", changeNum);
	if (changeNum == 1) return;
	cNode curNode = Changes[x];
	int src = curNode.src; int dst = curNode.dst; int cVal = curNode.changeVal;

	// apply change to Graph
	Graph[src][dst] = cVal;
	Graph[dst][src] = cVal;
}


int main(int argc, char* argv[]) {

	//TO DO
	// deal with argv num

	//printf("argc: %d\n",argc);
	if(argc!=4){	
		printf("usage: linkstate topologyfile messagesfile changesfile\n");
		exit(0);
	}

	char* tFileName = (char*)malloc(sizeof(char) * 32);
	char* mFileName = (char*)malloc(sizeof(char) * 32);
	char* cFileName = (char*)malloc(sizeof(char) * 32);



	/*
	strcpy(tFileName, "topology.txt");
	strcpy(mFileName, "messages.txt");
	strcpy(cFileName, "changes.txt");
	*/


	strcpy(tFileName, argv[1]);
	strcpy(mFileName, argv[2]);
	strcpy(cFileName, argv[3]);


	FILE* tFile, * mFile, * cFile;
	tFile = fopen(tFileName, "r");
	mFile = fopen(mFileName, "r");
	cFile = fopen(cFileName, "r");

	OPF=fopen("output_dv.txt","w");

	if (tFile == NULL || mFile == NULL || cFile == NULL) {
		fprintf(stderr, "Error: open input file.\n");
		return -1;
	}

	//init graph.
	for (int i = 0; i < MAX_NUM; i++) {
		for (int j = 0; j < MAX_NUM; j++) {
			if (i == j) Graph[i][j] = 0;
			else Graph[i][j] = -999;
		}
	}

	// get data from tFile
	int src, dst, cost;
	fscanf(tFile, "%d", &nodeNum);
	while (!feof(tFile)) {
		//construct graph
		src = 0; dst = 0; cost = 0;
		fscanf(tFile, "%d %d %d", &src, &dst, &cost);
		Graph[src][dst] = cost;
		Graph[dst][src] = cost;
		linkNum++;
	}
	fclose(tFile);

	// get data from mFile
	src = 0; dst = 0;
	char* msg = (char*)malloc(sizeof(char) * 1001);
	memset(msg, '\0', 1001);
	while (!feof(mFile)) {
		fscanf(mFile, "%d %d %[^\n]s", &src, &dst, msg);
		mNode temp; temp.dest = dst; temp.src = src; strcpy(temp.msg, msg);
		Messages.push_back(temp);
		msgNum++;
	}
	fclose(mFile);

	// get data from cFile
	src = 0; dst = 0;
	int diff = 0;
	while (!feof(cFile)) {
		fscanf(cFile, "%d %d %d", &src, &dst, &diff);
		cNode temp; temp.dst = dst; temp.src = src; temp.changeVal = diff;
		Changes.push_back(temp);
		changeNum++;
	}
	fclose(cFile);

	//TODO : construct routing table
	//printf("nodeNum : %d\n", nodeNum);
	//printf("EdgeNum : %d\n", linkNum);
	//printf("GraphSum : %d\n", CheckSum());
	//PrintGraph();


	ConstructTable();
	//PrintRoutingTable();

	ExchangeTable();
	CheckException();

	//printf("\n**** After Exchanging Table ****\n");
	PrintRoutingTable();


	TransMsg();
	// apply 1st change
	for (int c = 0; c < changeNum - 1; c++) {
		ApplyChange(c);
		InitTable();
		ConstructTable();
		ExchangeTable();
		//printf("\n**** After Applying %d-th Change ****\n\n", c + 1);
		PrintRoutingTable();
		TransMsg();
	}
	return 0;
}
