#ifndef APP_H_
#define APP_H_

#include <stdint.h>
#include <stddef.h>

#define NUM_FILES 50
#define MAX_WORD_LENGTH 80

extern uint8_t fileIndices[NUM_FILES];
extern char wavFileName[16];
extern char txtFileName[16];
extern char expectedWord[MAX_WORD_LENGTH];
extern char userInput[MAX_WORD_LENGTH];

int initializeDAC_USB();

void initializeIndices(uint8_t *array, int n);

void fisherYatesShuffle(uint8_t *array, int n);

int readWordFromFile(const char *fileName, char *buffer, size_t bufferSize);

#endif /* APP_H_ */
