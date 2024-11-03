#include "ESLApplication.h"
#include "app.h"
#include "keyboard.h"
#include "screen.h"
#include "wav_player.h"
#include <ctype.h>
#include <stdlib.h>

#include <stdatomic.h>

// Define atomic counters
volatile atomic_int helpCounter = 0;
volatile atomic_int enterCounter = 0;
volatile atomic_int current_index = 1;


extern int screenColumn;
extern int screenRow;
extern char current_word[32] ;  // Global word buffer with space for 32 characters
extern char expected_word[MAX_WORD_LENGTH];
extern char userInput[MAX_WORD_LENGTH];
extern uint8_t fileIndices[NUM_FILES];
extern Lcd_HandleTypeDef lcd;
extern int started;


const char *encouragementMessages[] = {
    "Sigue asi", "Bien hecho", "Excelente",
	"Fantastico", "Muy bien", "Impresionante"
};


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
				handleCorrectWord();
			}
			else{
				handleIncorrectWord();
			}

			break;

		default:
			break;
	}

}

// Helper function to handle repeating the word
void repeatAudio() {
	snprintf(wavFileName, sizeof(wavFileName), "%d.wav", fileIndices[current_index]);
	if (!wavPlayer_fileSelect(wavFileName));

	wavPlayer_play();

	while (!wavPlayer_isFinished())
	{
		wavPlayer_process();
	}

	wavPlayer_stop();
}

// Helper function to play the next file and handle index reset
void playNextFile() {
    // Check if current_index exceeds NUM_FILES
    if (atomic_load(&current_index) >= NUM_FILES) {
        // Shuffle the files again and reset the index
        fisherYatesShuffle(fileIndices, NUM_FILES);
        atomic_store(&current_index, 1);
    }

//    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET); // Indicate button pressed

    snprintf(wavFileName, sizeof(wavFileName), "%d.wav", fileIndices[current_index]);
    snprintf(txtFileName, sizeof(txtFileName), "%d.txt", fileIndices[current_index]);

    // Attempt to select and play the audio file
    if (!wavPlayer_fileSelect(wavFileName));

    wavPlayer_play();

    while (!wavPlayer_isFinished())
    {
        wavPlayer_process();
    }

    wavPlayer_stop();

    // Attempt to read and capitalize the word from the text file
    if (readWordFromFile(txtFileName, expected_word, sizeof(expected_word)))
    {
        capitalizeWord(expected_word);
    }
}

void startApplication() {

	//THIS LINE CAUSES THE TIMING ISSUES WITH KEYS
	while (!(initializeDAC_USB()));

//    // Initialize and shuffle file indices
    initializeIndices(fileIndices, NUM_FILES);
    fisherYatesShuffle(fileIndices, NUM_FILES);

//	if (!wavPlayer_fileSelect("11.wav"));
//
//	wavPlayer_play();
//
//	while (!wavPlayer_isFinished())
//	{
//		wavPlayer_process();
//	}
//
//	wavPlayer_stop();

    // Reset current index to 1
    atomic_store(&current_index, 1);

    playNextFile();
}

void handleCorrectWord() {
    // Clear screen and reset variables
    Lcd_clear(&lcd);
    screenRow = 0;
    screenColumn = 0;
    moveCursor(&lcd, 0, 0);
    memset(current_word, 0, sizeof(current_word));  // Reset current_word to empty

    showEncouragement();
	// To turn off the Yellow LED
	HAL_GPIO_WritePin(YellowLED_GPIO_Port, YellowLED_Pin, GPIO_PIN_RESET);

    // To turn on the Green LED
    HAL_GPIO_WritePin(GreenLED_GPIO_Port, GreenLED_Pin, GPIO_PIN_SET);

    HAL_Delay(2999);

    HAL_GPIO_WritePin(GreenLED_GPIO_Port, GreenLED_Pin, GPIO_PIN_RESET);

    Lcd_clear(&lcd);

    HAL_Delay(10);

    atomic_fetch_add(&current_index, 1);
    playNextFile();
}


//end
void endApplication()
{
	startUpScreen();
}

//locking issues with counters
void handleHelpFunction() {
    // Increment the help counter atomically
//    atomic_fetch_add(&helpCounter, 1);
//
//    // Calculate the current length of current_word
//    size_t currentLength = strlen(current_word);
//
//    // Check if current_word is not already fully revealed
//    if(atomic_load(&helpCounter) >= 3){
//		if (currentLength < strlen(expected_word)) {
//			// Clear the LCD and reset the displayed content
//			Lcd_clear(&lcd);
//
//			// Reveal the next letter from expected_word and add it to current_word
//			current_word[currentLength] = expected_word[currentLength];
//			current_word[currentLength + 1] = '\0';  // Null-terminate current_word
//
//			// Display the updated current_word on the LCD
//			Lcd_string(&lcd, current_word);
//
//			// Move the cursor to the next position
//			screenRow = 0;
//			screenColumn = currentLength;
//			moveCursor(&lcd, &screenRow, &screenColumn);
//			HAL_Delay(999);
//
//			// Reset the help counter if the word is fully revealed
//			if (strlen(current_word) == strlen(expected_word)) {
//				atomic_store(&helpCounter, 0);
//				showCorrection();
//				HAL_Delay(2999);
//				handleNewPlayAfterRevealingWord();
//			}
//		}
//    } else {
        // If the word is already completed, reset the help counter
        repeatAudio();
//    }
}

void handleNewPlayAfterRevealingWord(){
	// Clear screen and reset variables
	Lcd_clear(&lcd);
	screenRow = 0;
	screenColumn = 0;
	moveCursor(&lcd, 0, 0);
	memset(current_word, 0, sizeof(current_word));  // Reset current_word to empty

	/// To turn off the Green LED
	HAL_GPIO_WritePin(GreenLED_GPIO_Port, GreenLED_Pin, GPIO_PIN_RESET);

	// To turn on the Yellow LED
	HAL_GPIO_WritePin(YellowLED_GPIO_Port, YellowLED_Pin, GPIO_PIN_SET);

	HAL_Delay(2999);

	// To turn off the Yellow LED
	HAL_GPIO_WritePin(GPIOE, YellowLED_Pin, GPIO_PIN_RESET);

	atomic_fetch_add(&current_index, 1);
	playNextFile();
}

//handling for incorrect word entered
void handleIncorrectWord()
{
	// To turn off the Green LED
	HAL_GPIO_WritePin(GreenLED_GPIO_Port, GreenLED_Pin, GPIO_PIN_RESET);

	// To turn on the Yellow LED
	HAL_GPIO_WritePin(YellowLED_GPIO_Port, YellowLED_Pin, GPIO_PIN_SET);

	HAL_Delay(2999);

	// To turn off the Yellow LED
	HAL_GPIO_WritePin(GPIOE, YellowLED_Pin, GPIO_PIN_RESET);
}

void showCorrection() {
    Lcd_clear(&lcd);
    Lcd_cursor(&lcd, 0, 0);
    Lcd_string(&lcd, "Correcto: ");
    Lcd_cursor(&lcd, 1, 0);
    Lcd_string(&lcd, expected_word);
}

void showEncouragement() {
    int randomIndex = rand() % 6;  // Get a random message
    Lcd_clear(&lcd);
    Lcd_cursor(&lcd, 0, 0);
    Lcd_string(&lcd, (char *)encouragementMessages[randomIndex]);
}

//counters can cause locking, try to find better way




