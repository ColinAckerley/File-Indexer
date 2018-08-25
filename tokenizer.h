#ifndef _tokenizer_h
#define _tokenizer_h
typedef struct node
{
	char* info;
	struct node *next;
} Node;
typedef struct recNode
{
	char* token;
	int count;
	struct recNode *next;
} Record;
Node* tokenizer(char*);
int isValidChar(char*);
Node* addNode(Node*, char*, int, int);
int isDelimeter(char);
void initializeBuffer(char*, int);
void lowerCase(char*, int);
Record* incRecord(Record*, char*);
int containsRecord(Record*, char*);
Record* addRecord(Record*, Node*);
Record* getRecords(Node*);
#endif
