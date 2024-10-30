#include "ESLApplication.h"
#include "app.h"
#include "keyboard.h"
#include "screen.h"

extern int screenColumn;
extern int screenRow;
extern char current_word[32] ;  // Global word buffer with space for 32 characters
extern char expected_word[MAX_WORD_LENGTH];
extern char userInput[MAX_WORD_LENGTH];
extern uint8_t fileIndices[NUM_FILES];

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

//initializations
void startApplication(){
	// Initialize the DAC and USB
	initializeDAC_USB();

	// Set up the start screen
	Lcd_clear(&lcd);
	Lcd_cursor(&lcd, 0, 0);  // Set cursor to the first row, first column
	Lcd_string(&lcd, "Presione 'Start'");  // Display "Press" on the first row
	Lcd_cursor(&lcd, 1, 0);  // Set cursor to the second row, first column
	Lcd_string(&lcd, "para comenzar");  // Display "Start" on the second row

	// Wait for user to press start while scanning the keyboard
	while (1) {
		scanKeyboard(&lcd, &screenRow, &screenColumn);
		if (started) {
			break;
		}
	}

	// Create a list of numbers, Shuffles, start for loop, generate .wav and .txt files, play audio
	initializeIndices(fileIndices, NUM_FILES);
	fisherYatesShuffle(fileIndices, NUM_FILES);

	for (int i = 0; i < NUM_FILES; i++) {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET); // Indicate button pressed

		// Generate the file names
		snprintf(wavFileName, sizeof(wavFileName), "%d.wav", fileIndices[i]);
		snprintf(txtFileName, sizeof(txtFileName), "%d.txt", fileIndices[i]);

		if (!wavPlayer_fileSelect(wavFileName)) {
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
			continue;
		}

		wavPlayer_play();

		// Wait until the current .wav file is finished playing
		while (!wavPlayer_isFinished()) {
			wavPlayer_process();
		}

		wavPlayer_stop();

		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET); // Indicate playback ended

		// Read the expected word from the .txt file
		if (!readWordFromFile(txtFileName, expected_word, sizeof(expected_word))) {
			continue;
		}

		memset(current_word, 0, sizeof(current_word)); // clear userInput buffer
		started = 1;
		while (started) {
			scanKeyboard(&lcd, &screenRow, &screenColumn);
		}

		// Compare the user input with expected_word
		if (strcmp(current_word, expected_word) == 0) {
			handleCorrectWord();
		}
		else {
			handleIncorrectWord();
		}

		//
//		 If correct,
//			- play audio correct audio file
//			- light the green LED
//			- clear the screen
//			- go to the next word
//		 If incorrect,
//			- play audio wrong! file
//			- light the yellow LED
//			- handle user deleting words
	}

	// Shuffle the audio files after each full playback sequence
	fisherYatesShuffle(fileIndices, NUM_FILES);
}

//end
void endApplication(){

}

void handleHelpFunction(){

}

//handling for correct word entered
void handleCorrectWord(){

}

//handling for incorrect word entered
void handleIncorrectWord(){

}

