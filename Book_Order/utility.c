#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utility.h"

/**
   Function to convert a string to an integer value to map to hash map. Returns a long int.
   Note: this is a djb2 algorithm. Credit goes to Dan Bernstein for making this hash function.  
*/
unsigned long getIndex(unsigned char *word) {

	unsigned long hashValue = 5381;
	int index;

	for(index = 0; index < strlen(word); index ++) 
		hashValue = ((hashValue << 5) + hashValue) + word[index]; 
	
	return hashValue;
}

/**
This function reads in a line from a file of an unkown string size. The string to return will reallocate if space runs out and there are  more characters to read in. Finally reallocate size to the actual length of the string line. Returns a line store in a string
*/
char *getStringFromFile(FILE *file) {

	int size = 150;
	int length = 0;
	int ch;

	char *stringLine = (char *)malloc(sizeof(char) * size);

	while(EOF != (ch = fgetc(file)) && ch != '\n') {
		stringLine[length++] = ch;
		/* Reallocate size if length if the maximum size of string */
		if(length == size) {
			stringLine = realloc(stringLine, sizeof(char) * (size += 50));
			if(stringLine == NULL) return stringLine;
		}
	}
	stringLine[length++] = '\0';
	stringLine = realloc(stringLine, sizeof(char) * length); /* Final reallocation */

	return stringLine;	/* Return string */
}
