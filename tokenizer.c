#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include "tokenizer.h"


Node* tokenizer(char *path){
	//initialization of variables
	int fileDesc, buffSize;
	buffSize=20;
	char *buffer = (char*)malloc(buffSize*sizeof(char));
	fileDesc = open(path, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
	int numCharsRead;
	numCharsRead = read(fileDesc, buffer, buffSize);
	Node *head;
	head=NULL;
	int stWord, endWord;
	stWord=endWord=0;
	
	if(isDelimeter(buffer[stWord])){
		while(stWord<buffSize && isDelimeter(buffer[stWord])){
			stWord++;
		}
	}
	while(numCharsRead!=0){
		int i;
		for(i=stWord; i<buffSize; i++){
			if(isDelimeter(buffer[i])){
				lowerCase(buffer, buffSize);
				head=addNode(head, buffer, stWord, i);
				int j;
				for(j=stWord; j<=i; j++){
					buffer[j]=' ';
				}
				while(i<buffSize && isDelimeter(buffer[i])==1){
					i++;
				}
				stWord=i;
			}
		}
		if(!isDelimeter(buffer[buffSize-1])){
				char str[buffSize-stWord];
				memcpy(str, &buffer[stWord], buffSize-stWord);
				
				initializeBuffer(buffer, buffSize);
				memcpy(buffer, str, (buffSize-stWord));
				char *tempPtr=buffer+(buffSize-stWord);
				numCharsRead = read(fileDesc, tempPtr, stWord);
				stWord=0;	
		}
		else{
			initializeBuffer(buffer, buffSize);
			numCharsRead = read(fileDesc, buffer, buffSize);
			stWord=0;
			if(isDelimeter(buffer[stWord])){
				while(stWord<buffSize && isDelimeter(buffer[stWord])){
					stWord++;
				}
			}
		}
		
	}
	free(buffer);
	return head;
}

Node* addNode(Node* head, char* buffer, int stWord, int endWord){
	int sizeWord;
	sizeWord = endWord-stWord;
	if(isdigit(buffer[stWord])){
		int k=stWord;
		while(k<endWord && isdigit(buffer[k])){
			k++;
		}
		if(k==endWord){
			return head;
		}
		else
			return addNode(head, buffer, k, endWord);
		
	}
	Node* newNode=NULL;
	newNode=(Node*)malloc(sizeof(Node));
	newNode->info=(char*)malloc((sizeWord+1)*sizeof(char));
	
	char *tempPtr = &buffer[stWord];
	char str[sizeWord+1];
	
	//copy string into newnode
	memcpy(str, tempPtr, sizeWord);
	str[sizeWord]='\0';
	strcpy(newNode->info, str);

	//find where new node should be inserted
	Node* currNode = head;
	if(currNode==NULL){
		head=newNode;
		newNode->next=NULL;
	}
	else{
		if(currNode!=NULL && strcmp(newNode->info, currNode->info) < 0){
			newNode->next=currNode;
			head=newNode;
			//return head;
		}
		else{
			while(currNode!=NULL && (strcmp(newNode->info, currNode->info) >= 0)){
				currNode=currNode->next;
			}
	
			Node* prevNode=NULL;
			prevNode=head;
		
			//if inserting at end of list
			if(currNode==NULL){
				while(prevNode->next != NULL){
					prevNode=prevNode->next;
				}
				prevNode->next=newNode;
				newNode->next=NULL;
			}
			//if list has more than one element
			else{
				while(prevNode->next != currNode){
					prevNode=prevNode->next;
				}
				newNode->next=currNode;
				prevNode->next=newNode;
			}
		}
	}
	return head;
}

void lowerCase(char* buffer, int buffSize){
	int i;
	for(i=0; i<buffSize; i++){
		if(!isDelimeter(buffer[i])){
			buffer[i]=tolower(buffer[i]);
		}
	}
}

void initializeBuffer(char* buffer, int buffSize){
	int i;
	for(i=0; i<buffSize; i++){
		buffer[i]=' ';
	}
}

int isDelimeter(char c){
	if(c=='\t' || c==' ' || c=='\n'){
		return 1;
	}
	if((c>='A' && c<='Z') || (c>='a' && c<='z') || (isdigit(c))){
		return 0;
	}
	
	return 1;
}

Record* getRecords(Node *head){
	Node *currNode;
	Record *headRec, *currRec;
	currNode=head;
	if(head==NULL){
		return NULL;
	}
	else{
		headRec=(Record*)malloc(sizeof(Record));
		currRec=headRec;
		currRec->token=(char*)malloc(sizeof(char)*strlen(currNode->info)+1);
		strcpy(currRec->token, currNode->info);
		currRec->count=1;
		currRec->next=NULL;
		currNode=currNode->next;
	}
	while(currNode!=NULL){
		if(containsRecord(headRec, currNode->info)){
			headRec=incRecord(headRec, currNode->info);
		}
		else{
			headRec=addRecord(headRec, currNode);
		}
		currNode=currNode->next;
	}
	return headRec;
}

Record* addRecord(Record *headRec, Node *currNode){
	Record *currRec;
	currRec=(Record*)malloc(sizeof(Record));
	currRec->token=(char*)malloc(sizeof(char)*strlen(currNode->info)+1);
	strcpy(currRec->token, currNode->info);
	currRec->count=1;
	currRec->next=NULL;
	Record* temprec = headRec;
	while(temprec->next!=NULL){
		temprec=temprec->next;
	}
	temprec->next=currRec;
	return headRec;
}

int containsRecord(Record *headRec, char* str){
	Record *currRec;
	currRec=headRec;
	while(currRec!=NULL){
		if(strcmp(str, currRec->token)==0){
			return 1;
		}
		currRec=currRec->next;
	}
	return 0;
}

Record* incRecord(Record *headRec, char* str){
	Record *currRec;
	currRec=headRec;
	while(currRec!=NULL){
		if(strcmp(str, currRec->token)==0){
			currRec->count++;
		}
		currRec=currRec->next;
	}
	return headRec;
}


