#ifndef _invertedIndex_h
#define _invertedIndex_h
#include "tokenizer.h"
typedef struct fileStruct
{
    char* fileName;
    Record *recordList;
    struct fileStruct *next;
} fileStruct;
bool checkExistence(char*);
bool canRead(char*);
void navigateDirectory(char*);
void addToFileStruct(char*);
fileStruct *combineDuplicateFilenames();
Record *combineDuplicateRecords(Record*);
void generateDataStructures(char*);
bool inputsValid(int,char**);
void createOutputFile(char*);
typedef struct listFileNames{
	char* fileName;
	Record* record;
	struct listFileNames *next;
}ListFiles;

typedef struct index{
	char *token;
	ListFiles* listFiles;
	struct index *next;
}Index;

Index* getAllWords(fileStruct*);
Index* createIndexer(Index*, fileStruct*);
void indexer(Index*, char*);
void freeFileStructList(fileStruct*);
void freeInvertedIndex(Index*);
int compStr(char*, char*);
#endif
