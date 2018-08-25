#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include "invertedIndex.h"
#include "tokenizer.c"
#include "Indexer.c"
fileStruct *headFileStruct = NULL;
Index *invInd = NULL;
int main(int argc, char **argv)
{
    char writeFile[1024]; //char arrays to store the user's input text and the file they want to export to
    char readFrom[1024];
    if(inputsValid(argc, argv)) //Otherwise save their inputs as a string
    {
        strcpy(writeFile, argv[1]);
        strcpy(readFrom, argv[2]);
    }
    else if(!inputsValid(argc, argv))
        exit(EXIT_FAILURE);
    generateDataStructures(readFrom);
	fileStruct* tempFS=headFileStruct;
	invInd = getAllWords(tempFS);
	invInd = createIndexer(invInd, tempFS);
	createOutputFile(writeFile);
	freeInvertedIndex(invInd);
    freeFileStructList(headFileStruct);
}

void freeInvertedIndex(Index* invInd){
	if(invInd==NULL){
		return;
	}
	else if(invInd->next==NULL){
			free(invInd);
		return;
	}
	else{
		Index* tempInd = invInd;
		invInd = invInd->next;
		while(invInd!=NULL){
			free(tempInd);
			tempInd=invInd;
			invInd=invInd->next;
		}
	}
}

void freeFileStructList(fileStruct* headFS){
	if(headFS==NULL){
		return;
	}
	else if(headFS->next==NULL){
		free(headFS);
	}
	else{
		fileStruct* tempFS=headFS;
		headFS=headFS->next;
		while(headFS!=NULL){
			free(tempFS);
			tempFS=headFS;
			headFS=headFS->next;
		}
	}
}
bool inputsValid(int argc,char**argv)
{
    if(argc != 3) //If they provide the wrong number of arguments
    {
        printf("Error: Incorrect number of arguments given\n");
        return false;
    }
    else
        return true;
}

void createOutputFile(char* writeFile)
{
    if(checkExistence(writeFile)) //Check if the user's output file already exists or not
    {
        printf("The output file specified already exists, would you like to overwrite it?\nEnter y for yes or n for no\n");
        char input = getchar();
        if(input == 'n') //If they do not want to overwrite the existing file, exit the program
        {
            printf("Exiting program\n");
            exit(EXIT_SUCCESS);
        }
        else if(input == 'y') //Overwrite the file
        {
            indexer(invInd, writeFile);
        }
        else
        {
            printf("Invalid Input, Exiting program\n");
            exit(EXIT_FAILURE);
        }
    }
    else //Otherwise create the file if it doesn't exist
    {
        indexer(invInd, writeFile);
    }
}


void generateDataStructures(char* readFrom)
{
    DIR* dirPointer;
    if(!(dirPointer = opendir(readFrom))) //Try to open the given input as a directory
    {
        if(errno == ENOENT)
        {
            printf("Error: The directory or file given does not exist\n");
            exit(EXIT_FAILURE);
        }
        else if(errno == ENOTDIR) //If it is not a directory
        {
            if(!(checkExistence(readFrom))) //See if the given file exists
            {
                printf("Error: The given file does not exist\n");
                exit(EXIT_FAILURE);
            }
            else if(!(canRead(readFrom))) //See if there is read access to the given file
            {
                printf("Error: You do not have read access to the given file\n");
                exit(EXIT_FAILURE);
            }
            else if(canRead(readFrom) && checkExistence(readFrom))
            {
                addToFileStruct(readFrom);
            }
        }
    }
    else if((dirPointer = opendir(readFrom)))
    {
        navigateDirectory(readFrom);
    }
    headFileStruct = combineDuplicateFilenames();
}
bool checkExistence(char *filename) //See if a file exists
{
    if(access(filename, F_OK) != -1)
        return true;
    else
        return false;
}
bool canRead(char *filename) //See if the user has read access to a file
{
    if(access(filename, R_OK) != -1)
        return true;
    else
        return false;
}
void navigateDirectory(char* path)
{
    DIR *d = opendir(path); // open the path
    if(d == NULL) //If the directory failed to open
    {
        printf("Error: Could not open the given directory\n");
        exit(EXIT_FAILURE);
    }
    struct dirent *dir; //Store the entries for current directory
    while ((dir = readdir(d)) != NULL) //If reading from the directory was successful
    {
        if(dir-> d_type != DT_DIR) // If the current path is a file
        {
            char curPath[1024]; //This will store the entire path to the file
            sprintf(curPath, "%s/%s", path, dir->d_name); //Concatenate the current path with the current file name
            addToFileStruct(curPath);
        }
        else //Otherwise out current path is not a file
        {
            if(dir -> d_type == DT_DIR && strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0 ) // if it is a directory
            {
                char directoryPath[512]; //Store the new directory path here
                sprintf(directoryPath, "%s/%s", path, dir->d_name); //Concatenate the old path with the new path and store it in our char array
                navigateDirectory(directoryPath); //Recursive call with the new path
            }
        }
    }
    closedir(d);
}
void addToFileStruct(char* path)
{
    if(!(canRead(path)))
    {
        printf("Error: You do not have read access to a file in the given directory\n");
        exit(EXIT_FAILURE);
    }
    Node* curTokenList = tokenizer(path); //Get the list of tokens for the current file
    Record* curRecordList = getRecords(curTokenList); //Get the records struct for the current file
    fileStruct* curFileStruct = (fileStruct*)malloc(sizeof(fileStruct)); //Create the file struct object for the current file
    char* fileNamePtr = (strrchr(path, '/')); //Get just the filename from the current path
    if(fileNamePtr == NULL)
        fileNamePtr = path;
    else
        fileNamePtr=&fileNamePtr[1];
    char* fileName = (char*)malloc(sizeof(strlen(fileNamePtr))*2);
    strcpy(fileName, fileNamePtr);
    for(int i = 0; fileName[i]; i++)
    {
        fileName[i] = tolower(fileName[i]);
    }
    curFileStruct->fileName = fileName; //Set the filename field to our filename
    curFileStruct->next = NULL; //Set the next record to null
    curFileStruct->recordList = curRecordList; //Set the record list to the current file's record list
    if(headFileStruct == NULL) //If it is the first insert into our file structure linked list
    {
        headFileStruct = curFileStruct;
        return;
    }
    else //Otherwise just add it to the end
    {
        fileStruct *last = headFileStruct;
         while (last->next != NULL)
             last = last->next;
        last->next = curFileStruct;
        return;
    }
}
fileStruct *combineDuplicateFilenames()
{
    fileStruct *cur = headFileStruct;
    fileStruct *ptr1, *ptr2, *duplicate;
    ptr1 = cur;
    //Pick elements one by one
    while (ptr1 != NULL && ptr1->next != NULL)
    {
        ptr2 = ptr1;
         //Compare the picked element with rest of the elements
        while (ptr2->next != NULL)
        {
            if (strcmp(ptr1->fileName, ptr2->next->fileName) == 0) //Found a duplicate
            {
                if(ptr1->recordList == NULL) //If our record list in ptr1 is null, just set it to whatever ptr2 record list is
                {
                    ptr1->recordList = ptr2->next->recordList;
                }
                else if(ptr2->recordList != NULL) //Otherwise, we know that ptr1 record list is not null, and if ptr2's record list has something in it
                {
                    Record *lastRecord = ptr1->recordList; //Get to the last node in ptr1
                    while(lastRecord->next != NULL)
                        lastRecord = lastRecord->next;
                    lastRecord->next = ptr2->next->recordList; //Add ptr2's record list to the end of ptr1's record list
                    ptr1->recordList = combineDuplicateRecords(ptr1->recordList); //Combine duplicate records
                }
                duplicate = ptr2->next; //Store a pointer to the node that is to be deleted
                ptr2->next = ptr2->next->next; //Skip over the duplicate node's pointer
                free(duplicate); //Free the node
            }
            else
            {
                ptr2 = ptr2->next;
            }
        }
        ptr1 = ptr1->next;
    }
    return headFileStruct;
}
Record *combineDuplicateRecords(Record *curRecord)
{
    Record *ptr1, *ptr2, *duplicate;
    ptr1 = curRecord;
    while (ptr1 != NULL && ptr1->next != NULL)
    {
        ptr2 = ptr1;
        while (ptr2->next != NULL)
        {
            if((strcmp(ptr1->token,ptr2->next->token ) == 0)) //If we find two of the same token
            {
                ptr1->count = ptr1->count + ptr2->next->count; //Combine their counts, and put this count into ptr1
                duplicate = ptr2->next; //Store a pointer to the node that is to be deleted
                ptr2->next = ptr2->next->next; //Remove the node that needs to be deleted
                free(duplicate); //Free the duplicte node
            }
            else
                ptr2 = ptr2->next;
        }
        ptr1 = ptr1->next;
    }
    return curRecord;
}

//RETURNS INDEXED LIST
Index* createIndexer(Index* headInd, fileStruct* headFS){
	fileStruct* currFS = NULL;
	Index* currInd = NULL;
	if(headInd==NULL || headFS==NULL){
		return headInd;
	}
	else{
		currInd=headInd;
		currFS=headFS;
	}
	//iterate through list of Indexes
	while(currInd!=NULL){
		currFS=headFS;
		//iterate through list of FileStructs
		while(currFS!=NULL){
			Record* currRec = NULL;
			currRec = currFS->recordList;
			
			//iterate through FileStructs->Records
			while(currRec!=NULL){
				//if we find a record->token that matches the currentIndex token
				if(strcmp(currInd->token, currRec->token)==0){
					
					ListFiles* newListFile=(ListFiles*)malloc(sizeof(ListFiles));
					newListFile->record=currRec;
					newListFile->fileName=currFS->fileName;
					newListFile->next = NULL;
					ListFiles* tempListFile = currInd->listFiles;
					//if adding to the list for the first time
					if(currInd->listFiles==NULL){
						currInd->listFiles=newListFile;
					}
					
					else{
						//if adding with only one element
						if(tempListFile->next==NULL){
							if(newListFile->record->count <= tempListFile->record->count){
								//tempListFile->next=newListFile;
								if(newListFile->record->count == tempListFile->record->count){
									if(compStr(newListFile->fileName, tempListFile->fileName)>0){
										tempListFile->next=newListFile;
										
									}
									else{
										newListFile->next=tempListFile;
										currInd->listFiles=newListFile;
									}
								}
								else{
									//prevListFile=tempListFile;
									tempListFile->next=newListFile;
									tempListFile=tempListFile->next;
								}
							}
							else{
								newListFile->next=tempListFile;
								currInd->listFiles=newListFile;
								//code to add in front
							}
						}
						//if adding to list w/ more than one elt
						else{
							ListFiles* prevListFile = NULL;
							prevListFile=currInd->listFiles;
							tempListFile=prevListFile->next;
							//if adding to the beginning of list
							if(newListFile->record->count > prevListFile->record->count){
								newListFile->next=currInd->listFiles;
								currInd->listFiles=newListFile;
							}
							else if(newListFile->record->count == prevListFile->record->count 
							&& compStr(newListFile->fileName, prevListFile->fileName)<0){
								newListFile->next=prevListFile;
								currInd->listFiles=newListFile;
								
							}
							else{
							//iterate records until new count is > curr count
								while(tempListFile!=NULL 
								&& newListFile->record->count < tempListFile->record->count){
									tempListFile=tempListFile->next;
									prevListFile=prevListFile->next;
								}
								if(tempListFile==NULL 
								|| newListFile->record->count > tempListFile->record->count){
									prevListFile->next=newListFile;
								}
								else{
									while((tempListFile!=NULL)
									&& (newListFile->record->count == tempListFile->record->count)
									&& (compStr(newListFile->fileName, tempListFile->fileName)>0)){
										prevListFile=prevListFile->next;
										tempListFile=tempListFile->next;
									}
									if(tempListFile==NULL){
										prevListFile->next=newListFile;
									}
									else{
										newListFile->next=tempListFile;
										prevListFile->next=newListFile;
									}
								}
							}
						}
					}
				}
				currRec=currRec->next;
			}
			currFS=currFS->next;
		}
		currInd=currInd->next;
	}
	return headInd;
}

//STRING COMPARE FOR ORDER OF FILENAMES
int compStr(char* s1, char* s2){
	int i = 0;
	if(s1==NULL && s2==NULL){
		if(s1==NULL && s2!=NULL){
			return (-1);
		}
		else if(s2==NULL && s1!=NULL){
			return 1;
		}
		else{
			printf("Error: s1 or s2 is NULL\n");
			return 0;
		}
	}
	
	while(s1[i]==s2[i] && (s1[i]!='\0' || s2[i]!='\0')){
		i++;
	}
	
	//case for if we reach nullByte
	if(s1[i]=='\0' || s2[i]=='\0'){
			if(s1[i]=='\0'){
				if(s2[i]!='\0'){
					return (-1);
				}
			}
			else if(s2[i]=='\0'){
				if(s1[i]!='\0'){
					return 1;
				}
			}
			else{
				printf("Error: s1[i] or s2[i] = nullByte but return 0\n");
				return 0;
			}
	}
	//if we reach two diff letters
	else if(isalpha(s1[i]) && isalpha(s2[i])){
		if(s1[i]<s2[i]){
			return (-1);
		}
		else if(s1[i]>s2[i]){
			return 1;
		}
		else{
			printf("Error: strings are equal\n");
			return 0;
		}
	}
	//if we reach two diff digits
	else if(isdigit(s1[i]) && isdigit(s2[i])){
		return (((int)s1[i])-((int)s2[i]));
	}
	//if s1 is letter and s2 is digit
	else if(isalpha(s1[i]) && isdigit(s2[i])){
		return (-1);
	}
	//if s1 is digit and s2 is letter
	else if(isdigit(s1[i]) && isalpha(s2[i])){
		return 1;
	}
	else if(s1[i]=='.' || s2[i]=='.'){
		if((isalpha(s1[i]) || isdigit(s1[i])) && s2[i]=='.'){
			return (-1);
		}
		else if(s1[i]=='.' && (isalpha(s2[i]) || isdigit(s2[i]))){
			return 1;
		}
	}
	else{
		printf("ERROR reached end\n");
		return 0;
	}
	printf("ERROR reached end\n");
	return 0; 
}

//RETURN LIST OF ALL POSS. WORDS
Index* getAllWords(fileStruct* headFS){
	fileStruct *currFS = headFS;
	Index* invInd=NULL;
	Index *tempInd=NULL;

	if(headFS==NULL){
		return invInd;
	}
	else{
		//iterate through fileStructs
		while(currFS!=NULL){
			Record *currRec = currFS->recordList;
			//iterate through recordList
			while(currRec!=NULL){
				//if no words exist in invIndex yet
				if(invInd==NULL){
					invInd=(Index*)malloc(sizeof(Index));
					invInd->token=currRec->token;
					invInd->listFiles=NULL;
					invInd->next=NULL;
					tempInd=invInd;
				}
				else{
				//if there is at least one
					tempInd=invInd;
					int boolContains;
					boolContains = 0;
					//iterate through Index tokens and compare to see if we have one already
					while(tempInd!=NULL){
						if(strcmp(tempInd->token, currRec->token)==0){ //fix this
							boolContains = 1;
						}	
						tempInd=tempInd->next;
					}
					if(!boolContains){
						tempInd=invInd;
						Index *prevInd = invInd;
						Index *newInd = (Index*)malloc(sizeof(Index));
						newInd->listFiles=NULL;
						newInd->next=NULL;
						newInd->token=currRec->token;
						while(tempInd!=NULL && strcmp(newInd->token, tempInd->token) > 0){
							if(prevInd!=tempInd){
								prevInd=prevInd->next;
							}
							tempInd=tempInd->next;
						}
						//if adding newIndex to the front
						if(tempInd==invInd){
							tempInd=newInd;
							newInd->next=NULL;
						}
						//if adding newIndex to the back
						else if(tempInd==NULL){
							prevInd->next=newInd;
						}
						else{
							newInd->next=tempInd;
							prevInd->next=newInd;
						}
						newInd->listFiles=NULL;
					}
				}
				currRec=currRec->next;
			}
			currFS=currFS->next;
		}
	}
	return invInd;
}
