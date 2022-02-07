#pragma warning(disable:4996)
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <queue>
using namespace std;

typedef struct _stockNode{
	int id = -1;
	int quantity = -1;
	int price = -1;
}StockNode;

vector <StockNode> StockData;

typedef struct _treeNode {
	_treeNode* leftChild = NULL;
	_treeNode* rightChild = NULL;
	StockNode data;
}treeNode;

treeNode* ROOT;

treeNode* SearchNode(treeNode* Tree, int targetID) {
	if (Tree == NULL)
		return NULL;

	if (Tree->data.id == targetID)
		return Tree;
	else if (Tree->data.id > targetID)
		return SearchNode(Tree->leftChild, targetID);
	else
		return SearchNode(Tree->rightChild, targetID);
}

treeNode* SearchMinNode(treeNode* Tree) {
	if (Tree == NULL)
		return NULL;

	if (Tree->leftChild == NULL)
		return Tree;
	else
		return SearchMinNode(Tree->leftChild);
}

void InsertNode(treeNode* Tree, StockNode* target) {
	if (Tree->data.id < target->id) {
		if (Tree->rightChild == NULL)
			Tree->rightChild = target;
		else
			InsertNode(Tree->rightChild, target);
	}
	else if (Tree->data.id > target->id) {
		if (Tree->leftChild == NULL)
			Tree->leftChild = target;
		else
			InsertNode(Tree->leftChild, target);
	}
}



treeNode* RemoveNode(treeNode* Tree, treeNode* Parent, int targetID) {
	treeNode* Removed = NULL;
	if (Tree == NULL)
		return NULL;

	if (Tree->data.id > targetID)
		Removed = RemoveNode(Tree->leftChild, Tree, targetID);
	else if(Tree->data.id < targetID)
		Removed = RemoveNode(Tree->rightChild, Tree, targetID);
	else {
		Removed = Tree;

		if (Tree->leftChild == NULL && Tree->rightChild == NULL) {
			if (Parent->leftChild == Tree)
				Parent->leftChild = NULL;
			else
				Parent->rightChild = NULL;
		}
		else {
			if (Tree->leftChild != NULL && Tree->rightChild != NULL) {
				treeNode* minNode = SearchMinNode(Tree->rightChild);
				minNode = RemoveNode(Tree, NULL, minNode->data.id);
				Tree->data = minNode->data;
			}
			else {
				treeNode* temp = NULL;
				if (Tree->leftChild != NULL)
					temp = Tree->leftChild;
				else
					temp = Tree->rightChild;

				if (Parent->leftChild == Tree)
					Parent->leftChild = temp;
				else
					Parent->rightChild = temp;
			}
		}
	}
	return Removed;
}

treeNode* createNode(StockNode newStockNode) {
	treeNode* newNode = (treeNode*)malloc(sizeof(treeNode));
	newNode->leftChild = NULL;
	newNode->rightChild = NULL;
	newNode->data = newStockNode;

	return newNode;
}

treeNode* createNode(int targetID) {
	treeNode* newNode = (treeNode*)malloc(sizeof(treeNode));
	newNode->leftChild = NULL;
	newNode->rightChild = NULL;
	newNode->data.id = targetID;

	return newNode;
}

void Inorder(treeNode* Node) {
	if (Node == NULL)
		return;
	Inorder(Node->leftChild);
	printf("%d ", Node->data.id);
	Inorder(Node->rightChild);
}

void ConstructBinaryTree() {
	// With StockDatas, we contruct binary tree
	StockNode temp;
	temp.id = 123;
	ROOT = createNode(temp);
	InsertNode(ROOT, createNode(22));
	InsertNode(ROOT, createNode(9918));
	InsertNode(ROOT, createNode(424));
	InsertNode(ROOT, createNode(17));
	InsertNode(ROOT, createNode(3));

	InsertNode(ROOT, createNode(98));
	InsertNode(ROOT, createNode(34));

	InsertNode(ROOT, createNode(760));
	InsertNode(ROOT, createNode(317));
	InsertNode(ROOT, createNode(1));

	Inorder(ROOT);

	/*
	for (int i = 0; i < StockData.size(); i++) {
		treeNode* temp = createNode(StockData[i]);
	}
	*/
}

void writeStockData() {
	FILE* oFP = fopen("stock.txt", "w");
	int stockNum = StockData.size();
	for (int i = 0; i < stockNum; i++) {
		fprintf(oFP, "%d %d %d\n", StockData[i].id, StockData[i].quantity, StockData[i].price);
	}
	printf("\n***Writing stock.txt Completed***\n");
}

int main() {
	FILE* sFP = fopen("stock.txt", "r");
	while (!feof(sFP)) {
		StockNode temp;
		int tempID, tempQuantity, tempPrice;
		fscanf(sFP, "%d %d %d", &tempID, &tempQuantity, &tempPrice);
		temp.id = tempID; temp.quantity = tempQuantity; temp.price = tempPrice;
		StockData.push_back(temp);
	}
	ConstructBinaryTree();
}
