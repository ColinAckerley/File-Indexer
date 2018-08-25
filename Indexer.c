#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "tokenizer.h"
#include "invertedIndex.h"
#include "Indexer.h"

void indexer(Index* headInd, char* outFileName){
	FILE *outfile;
	outfile=fopen(outFileName, "w");
	Index* tempInd = headInd;
	if(outfile == NULL){
		printf("Error");
	}
	fprintf(outfile, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(outfile, "<fileIndex>\n");
	
	while(tempInd!=NULL){
    		fprintf(outfile, "\t<word text=\"%s\">\n", tempInd->token);
		ListFiles *tempListFiles = tempInd->listFiles;
		while(tempListFiles!=NULL){
			fprintf(outfile, "\t\t<file name=\"%s\">%d</file>\n", tempListFiles->fileName, tempListFiles->record->count);
			tempListFiles=tempListFiles->next;
		}
    		fprintf(outfile, "\t</word>\n");
		tempInd=tempInd->next;
   	}
	
	fprintf(outfile, "</fileIndex>\n");
}

