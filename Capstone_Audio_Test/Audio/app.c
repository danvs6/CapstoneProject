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
#include <stdatomic.h>

extern ApplicationTypeDef Appli_state;
extern I2C_HandleTypeDef hi2c1;
extern I2S_HandleTypeDef hi2s3;
extern RNG_HandleTypeDef hrng;

// Define atomic variables
volatile atomic_bool isSdCardMounted = false;

uint8_t fileIndices[NUM_FILES];

char wavFileName[16];
char txtFileName[16];

char expectedWord[MAX_WORD_LENGTH];
char userInput[MAX_WORD_LENGTH];


int initializeDAC_USB() {
    MX_USB_HOST_Process();

    if (Appli_state == APPLICATION_START) {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET); // USB application starts
    } else if (Appli_state == APPLICATION_DISCONNECT) {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
        atomic_store(&isSdCardMounted, false);
        wavPlayer_stop();
    }

    if (Appli_state == APPLICATION_READY) {
        if (!atomic_load(&isSdCardMounted)) {
            f_mount(&USBHFatFS, (const TCHAR*)USBHPath, 0);
            atomic_store(&isSdCardMounted, true);
        }
    }
    return atomic_load(&isSdCardMounted);
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
