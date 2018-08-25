all: default

default: invertedIndex.c tokenizer.c Indexer.c invertedIndex.h tokenizer.h Indexer.h
	gcc -Wall -Werror -fsanitize=address -std=gnu99 invertedIndex.c -o invertedIndex

clean:
	rm -f invertedIndex
