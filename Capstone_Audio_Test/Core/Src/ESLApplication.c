#include "ESLApplication.h"
#include "app.h"
#include "keyboard.h"
#include "screen.h"
#include <ctype.h>

extern int screenColumn;
extern int screenRow;
extern char current_word[32] ;  // Global word buffer with space for 32 characters
extern char expected_word[MAX_WORD_LENGTH];
extern char userInput[MAX_WORD_LENGTH];
extern uint8_t fileIndices[NUM_FILES];
extern Lcd_HandleTypeDef lcd;
extern int started;
int current_index = 1;


void capitalizeWord(char *word) {
    for (int i = 0; word[i] != '\0'; i++) {
        word[i] = toupper((unsigned char)word[i]);
    }
}

void startUpScreen()
{
	  Lcd_cursor(&lcd, 0, 0);  // Set cursor to the first row, first column
	  Lcd_string(&lcd, "Presione 'Start'");  // Display "Press" on the first row
	  Lcd_cursor(&lcd, 1, 0);  // Set cursor to the second row, first column
	  Lcd_string(&lcd, "para comenzar");  // Display "Start" on the second row
}

void processSpecialKey(char key, int correct){
	switch (key){
		case KEY_START:
			// Handle KEY_START
			startApplication();
			break;

		case KEY_END:
			// Handle KEY_END
			endApplication();
			break;

		case KEY_HELP:
			// Handle KEY_HELP
			handleHelpFunction();
			break;

		case KEY_ENTER:
			// Handle KEY_ENTER
			if(correct){
				//handle correct entry
				handleCorrectWord();
			}
			else{
				//handle incorrect entry
				handleIncorrectWord();
			}

			break;

		default:
			break;
	}

}

void startApplication() {
    // Wait for DAC to initialize
    while (!initializeDAC_USB());

    // Initialize and shuffle file indices
    initializeIndices(fileIndices, NUM_FILES);
    fisherYatesShuffle(fileIndices, NUM_FILES);

	if (!wavPlayer_fileSelect("11.wav")) {
		printf("File %s not found or could not be selected.\n", wavFileName);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
		return;  // Exit if file is not found
	}

	wavPlayer_play();
	while (!wavPlayer_isFinished()) {
		wavPlayer_process();
	}
	wavPlayer_stop();

    // Reset current index to 1
    current_index = 1;

    playNextFile();
}

void handleCorrectWord() {
    // Clear screen and reset variables
    Lcd_clear(&lcd);
    screenRow = 0;
    screenColumn = 0;
    moveCursor(&lcd, 0, 0);
    memset(current_word, 0, sizeof(current_word));  // Reset current_word to empty

	// To turn off the Yellow LED
	HAL_GPIO_WritePin(GPIOE, GreenLED_Pin, GPIO_PIN_SET);

	HAL_Delay(2999);

    // To turn on the Green LED
    HAL_GPIO_WritePin(GPIOE, GreenLED_Pin, GPIO_PIN_RESET);


    playNextFile();
}

// Helper function to play the next file and handle index reset
void playNextFile() {
    // Check if current_index exceeds NUM_FILES
    if (current_index > NUM_FILES) {
        // Shuffle the files again and reset the index
        fisherYatesShuffle(fileIndices, NUM_FILES);
        current_index = 1;
    }

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET); // Indicate button pressed

    snprintf(wavFileName, sizeof(wavFileName), "%d.wav", fileIndices[current_index]);
    snprintf(txtFileName, sizeof(txtFileName), "%d.txt", fileIndices[current_index]);

    // Attempt to select and play the audio file
    if (!wavPlayer_fileSelect(wavFileName)) {
        printf("File %s not found or could not be selected.\n", wavFileName);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
        return;  // Exit if file is not found
    }

    wavPlayer_play();
    while (!wavPlayer_isFinished()) {
        wavPlayer_process();
    }
    wavPlayer_stop();

    // Attempt to read and capitalize the word from the text file
    if (readWordFromFile(txtFileName, expected_word, sizeof(expected_word))) {
        capitalizeWord(expected_word);
        printf("Expected word set to: %s\n", expected_word);
    } else {
        printf("Failed to read expected word from %s.\n", txtFileName);
    }

    // Increment the index for the next file
    current_index++;
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET); // Indicate end of playback
}


//end
void endApplication()
{
	startUpScreen();
}

void handleHelpFunction()
{
	// audio here
	current_index--;
	playNextFile();
}

//handling for incorrect word entered
void handleIncorrectWord()
{
	// audio here

	// To turn on the Yellow LED
	HAL_GPIO_WritePin(GPIOE, YellowLED_Pin, GPIO_PIN_SET);

	HAL_Delay(2999);

	// To turn off the Green LED
	HAL_GPIO_WritePin(GPIOE, YellowLED_Pin, GPIO_PIN_RESET);

}

