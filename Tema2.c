#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFLEN 1024
#define ELEMENT_TREE_LENGTH 3

#include "AVLTree.h"

typedef struct Range{
	int *index;
	int size;
	int capacity;
}Range;

void printFile(char* fileName){
	if(fileName == NULL) return;
	FILE * file = fopen(fileName,"r");
	if (file == NULL) return;
	char *buf = (char*) malloc(BUFLEN+1);
	while(fgets(buf,BUFLEN,file) != NULL){
		printf("%s",buf);
	}
	printf("\n");
	free(buf);
	fclose(file);
}

void printWordsInRangeFromFile(Range* range, char* fileName){
	if(fileName == NULL || range == NULL) return;
	FILE * file = fopen(fileName,"r");
	if (file == NULL) return;
	char *buf = (char*) malloc(BUFLEN+1);
	for(int i = 0; i < range->size;i++){
		fseek(file,range->index[i],SEEK_SET);
		if(fgets(buf,BUFLEN,file) != NULL){
			char* token = strtok(buf," .,\n");
			printf("%d. %s:%d\n",i+1, token, range->index[i]);
		}
	}
	printf("\n");
	free(buf);
	fclose(file);
}

void printTreeInOrderHelper(TTree* tree, TreeNode* node){
	if(node != NULL){
		printTreeInOrderHelper(tree, node->lt);
		TreeNode* begin = node;
		TreeNode* end = node->end->next;
		while(begin != end){
			printf("%d:%s  ",*((int*)begin->info),((char*)begin->elem));
			begin = begin->next;
		}
		printTreeInOrderHelper(tree, node->rt);
	}
}

void printTreeInOrder(TTree* tree){
	if(tree == NULL) return;
	printTreeInOrderHelper(tree, tree->root);
}


void* createStrElement(void* str){
	char *c = malloc(4 * sizeof(char));
	strncpy(c, (char*) (str), 3);
	c[3] = 0;
	return c;
}

void destroyStrElement(void* elem){
	free((char*)elem);
}


void* createIndexInfo(void* index){
	long *i = malloc(sizeof(long));
	*i = *((long*)index);
	return i; 
}

void destroyIndexInfo(void* index){
	free((long*)index);
}

int compareStrElem(void* str1, void* str2){
	if(strcmp((char*)str1, (char*)str2) < 0) {
		return -1;
	}
	if(strcmp((char*)str1, (char*)str2) > 0) {
		return 1;
	}
	return 0;
}

/*
 * Name function: buildTreeFromFile
 * Return: the memory address of the tree
 * Arguments: the file that I read from
 * Purpose: form a tree with the given words from a file, concerning the index,
 * and the string
 */
TTree* buildTreeFromFile(char* fileName){
	//create the tree
	TTree *tree = createTree(createStrElement, destroyStrElement,
			createIndexInfo, destroyStrElement, compareStrElem);

	//open the file I am going to read from
	FILE *in = fopen(fileName, "rt");
	long fl_size;
	char *buffer;
	if (in == NULL) {
		printf("ERROR: Can't open file %s", fileName);
		return NULL;
	}

	//get the size of the file
	fseek(in, 0, SEEK_END);
	fl_size = ftell(in);
	rewind(in);

	//read in a buffer the whole file
	buffer = (char*)malloc(sizeof(char) * (fl_size + 1));
	if(buffer == NULL) {
		printf("Not enough memory\n");
		return NULL;
	}
	//read everything in a buffer
	fread(buffer, 1, fl_size, in);
	buffer[fl_size] = 0;
	fclose(in);
	long i;
	char *string = (char*)malloc(sizeof(char) * (fl_size + 1));
	long d;
	d = 0;
	long comma = 0;
	//form a string of characters
	for(i = 0; i < fl_size; i++) {
		if(buffer[i] >= 'a' && buffer[i] <= 'z' || buffer[i] == '-' ||
				buffer[i] == ':') {
			string[d] = buffer[i];
			d++;
		} else {
			//calculating the comman
			if(buffer[i] == ',' && buffer[i + 1] == ' ') {
				comma = comma + 1;
			} else {
				if(d != 0) {
					string[d] = 0;
					//add the new element into the tree
					char *str = createStrElement(string);
					//determining the index of the string and substracting the commas
					long j = i - strlen(string) - comma;
					long *index = createIndexInfo(&j);
					insert(tree, str, index);
					d = 0;
					string[d] = 0;
					destroyStrElement(str);
					destroyIndexInfo(index);
				}
			}
		}
	}
	free(string);
	free(buffer);
	return tree;
}

/*
 * Name function: find
 * Return: void (it does not return a value)
 * Arguments: the node, the given string and the words
 * Purpose: find the words that start with the given string and form an array
 * of indexes
 */
void find(TreeNode* node, char* q, Range* words) {
	if(node != NULL) {
		//getting the elements in order
		find(node->lt, q, words);
		int i, b = 1;
		TreeNode *first = node;
		TreeNode *last = node->end;
		//get the elements from a list
		while(first != last) {
			b = 1;
			for(i = 0; i < strlen(q); i++) {
				if(q[i] != ((char*)first->elem)[i]) {
					b = 0;
				}
			}
			if(b == 1) {
				//update the array
				words->index[words->size] = *(long*)first->info;
				words->size++;
			}
			first = first->next;
		}
		b = 1;
		//check for the last element of the list
		for(i = 0; i < strlen(q); i++) {
			if(q[i] != ((char*)last->elem)[i]) {
				b = 0;
			}
		}
		if(b == 1) {
			//update the array
			words->index[words->size] = *(long*)last->info;
			words->size++;
		}
		find(node->rt, q, words);
	}
}

/*
 * Name function: singleKeyRangeQuery
 * Return: the address of the words
 * Arguments: the tree and the given string
 * Purpose: find the words that start with the given string and form an array
 * of indexes
 */
Range* singleKeyRangeQuery(TTree* tree, char* q){
	Range *words;
	words = (Range*)malloc(sizeof(Range));
	if(words == NULL) {
		printf("Not enough memory\n");
		return NULL;
	}
	//create the array of indexes
	words->index = (int*)malloc(sizeof(int) * BUFLEN);
	if(words->index == NULL) {
		printf("Not enough memory\n");
		return NULL;
	}
	words->capacity = BUFLEN;
	words->size = 0;
	TreeNode *node = tree->root;
	//find the words with a specific rule
	find(node, q, words);
	return words;
}

/*
 * Name function: findInt
 * Return: void (it does not return a value)
 * Arguments: a node, the two strings q, p and the words
 * Purpose: find the words that are located between the two strings and form 
 * an array of indexes
 */
void findInt(TreeNode* node, char* q, char*p, Range* words) {
	if(node != NULL) {
		findInt(node->lt, q, p, words);
		int i, b = 1;
		TreeNode *first = node;
		TreeNode *last = node->end;
		//search between the elements of a list
		while(first != last) {
			if(strncmp(q, ((char*)first->elem), strlen(q)) <= 0
					&& strncmp(p, ((char*)first->elem), strlen(p)) >= 0) {
				words->index[words->size] = *(long*)first->info;
				words->size++;
			}
			first = first->next;
		}
		//check for the last element of the list
		if(strncmp(q, ((char*)first->elem), strlen(q)) <= 0
				&& strncmp(p, ((char*)first->elem), strlen(p)) >= 0) {
			words->index[words->size] = *(long*)last->info;
			words->size++;
		}
		findInt(node->rt, q, p, words);
	}
}

/*
 * Name function: multiKeyRangeQuery
 * Return: the memory address of words
 * Arguments: the tree, the two strings q, p
 * Purpose: find the words that are located between the two strings and form 
 * an array of indexes
 */
Range* multiKeyRangeQuery(TTree* tree, char* q, char* p){
	Range *words;
	words = (Range*)malloc(sizeof(Range));
	if(words == NULL) {
		printf("Not enough memory\n");
		return NULL;
	}
	words->index = (int*)malloc(sizeof(int) * BUFLEN);
	if(words->index == NULL) {
		printf("Not enough memory\n");
		return NULL;
	}
	words->capacity = BUFLEN;
	words->size = 0;
	TreeNode *node = tree->root;
	findInt(node, q, p, words); 
	return words;
}


int main(void) {

	printf("The text file:\n");
	printFile("text.txt");

	TTree* tree = buildTreeFromFile("text.txt");
	printf("Tree In Order:\n");
	printTreeInOrder(tree);
	printf("\n\n");

	printf("Single search:\n");
	Range *range = singleKeyRangeQuery(tree,"v");
	printWordsInRangeFromFile(range,"text.txt");

	printf("Multi search:\n");
	Range *range2 = multiKeyRangeQuery(tree,"j","pr");
	printWordsInRangeFromFile(range2,"text.txt");

	if(range != NULL) free(range->index);
	free(range);

	if(range2 != NULL) free(range2->index);
	free(range2);

	destroyTree(tree);
	return 0;
}





