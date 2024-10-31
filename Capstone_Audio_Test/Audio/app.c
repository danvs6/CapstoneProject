/*
 * app.c
 *
 *  Created on: Oct 28, 2024
 *      Author: cuphi
 */

#include "app.h"
#include "main.h"
#include "fatfs.h"
#include "usb_host.h"
#include "audioI2S.h"
#include "MY_CS43L22.h"
#include "wav_player.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

extern ApplicationTypeDef Appli_state;
extern I2C_HandleTypeDef hi2c1;
extern I2S_HandleTypeDef hi2s3;
extern RNG_HandleTypeDef hrng;

#define NUM_FILES 10
#define MAX_WORD_LENGTH 32

uint8_t fileIndices[NUM_FILES];

char wavFileName[16];
char txtFileName[16];

char expectedWord[MAX_WORD_LENGTH];
char userInput[MAX_WORD_LENGTH];

void initializeIndices(uint8_t *array, int n);
void fisherYatesShuffle(uint8_t *array, int n);
int readWordFromFile(const char *fileName, char *buffer, size_t bufferSize);
void processAudioFiles(void);


// main while-loop code
void appMainLoop(void) {
	initializeDAC_USB();

    if (Appli_state == APPLICATION_READY) {
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) {
            processAudioFiles();
        }
    }
}

int initializeDAC_USB() {
	static bool isSdCardMounted = 0;
	MX_USB_HOST_Process();

	if (Appli_state == APPLICATION_START) {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET); // USB application starts
	}
	else if (Appli_state == APPLICATION_DISCONNECT) {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
		isSdCardMounted = 0;
		wavPlayer_stop();
	}

	if (Appli_state == APPLICATION_READY) {
		if (!isSdCardMounted) {
			f_mount(&USBHFatFS, (const TCHAR*)USBHPath, 0);
			isSdCardMounted = 1;
		}
	}
	return isSdCardMounted;
}

void processAudioFiles(void) {
	// Iterate through each wav file
	for (int i = 0; i < NUM_FILES; i++) {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET); // Indicate button pressed

		// Generate the file names
		snprintf(wavFileName, sizeof(wavFileName), "%d.wav", fileIndices[i]);
		snprintf(txtFileName, sizeof(txtFileName), "%d.txt", fileIndices[i]);

		//if (wavPlayer_fileSelect(wavFileName) != WAV_OK) {
		if (!wavPlayer_fileSelect(wavFileName)) {
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
			continue;
		}

		wavPlayer_play();

		// Wait until the current wav file is finished playing
		while (!wavPlayer_isFinished()) {
			wavPlayer_process();
		}

		wavPlayer_stop();

		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET); // Indicate playback ended

		// TODO:
		// 1. Read from the text file using readWordFromFile
		// 2. Get the user input (needs implementation)
		// 3. Compare the user input with the expected word (needs implementation)
		// 4. Trigger a feedback mechanism (e.g., LED)

		// Wait for the user to press the pushbutton to begin playing the next file
		while (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) {
			HAL_Delay(100);  // Debounce delay
		}
	}

	// Shuffle the audio files after each full playback sequence
	fisherYatesShuffle(fileIndices, NUM_FILES);
}

void initializeIndices(uint8_t *array, int n) {
    for (uint8_t i = 0; i < n; i++) {
        array[i] = i + 1;
    }
}

void fisherYatesShuffle(uint8_t *array, int n) {
    for (int i = n - 1; i > 0; i--) {
        uint32_t randomNumber;

        if (HAL_RNG_GenerateRandomNumber(&hrng, &randomNumber) != HAL_OK) {
            randomNumber = 0;
        }

        int j = randomNumber % (i + 1); // Generate a random index between 0 and i

        // Swap array[j] with array[i]
        uint8_t temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

int readWordFromFile(const char *fileName, char *buffer, size_t bufferSize) {
    FIL file;
    UINT bytesRead;
    FRESULT result;

    result = f_open(&file, fileName, FA_READ);

    // Handle error opening file
    if (result != FR_OK) {
        return 0;
    }

    result = f_read(&file, buffer, bufferSize - 1, &bytesRead);
    if (result != FR_OK) {
        f_close(&file);
        return 0;
    }

    buffer[bytesRead] = '\0';
    f_close(&file);

    for (UINT i = 0; i < bytesRead; i++) {
        if (buffer[i] == '\r' || buffer[i] == '\n') {
            buffer[i] = '\0';
            break;
        }
    }

    return 1;
}
