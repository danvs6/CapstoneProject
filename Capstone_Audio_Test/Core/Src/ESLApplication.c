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
volatile atomic_int score = 0;


extern int screenColumn;
extern int screenRow;
extern char current_word[80] ;  // Global word buffer with space for 32 characters
extern char expected_word[MAX_WORD_LENGTH];
extern char userInput[MAX_WORD_LENGTH];
extern uint8_t fileIndices[NUM_FILES];
extern Lcd_HandleTypeDef lcd;
extern atomic_int started;
extern int languageChosen;
extern atomic_int keyDetected;
extern char languageCode[10];


const char *encouragementMessagesSpanish[] = {
    "Sigue asi!", "Bien hecho!", "Excelente!",
	"Fantastico!", "Muy bien!", "Impresionante!"
};

const char *encouragementMessagesFrench[] = {
    "Continue comme ca!", "Bien joue!", "Excellent!",
    "Fantastique!", "Tres bien!", "Impressionnant!"
};

const char *encouragementMessagesGerman[] = {
    "Mach weiter so!", "Gut gemacht!", "Ausgezeichnet!",
    "Fantastisch!", "Sehr gut!", "Beeindruckend!"
};

void capitalizeWord(char *word)
{
    for (int i = 0; word[i] != '\0'; i++)
    {
        word[i] = toupper((unsigned char)word[i]);
    }
}

void chooseLanguageScreen()
{
	while (!(initializeDAC_USB()));

	languageChosen = 0;
	atomic_store(&started, 0);
	atomic_store(&score, 0);
	Lcd_clear(&lcd);
	//hide cursor
	turnOffCursor(&lcd);

	centerString(&lcd, 0, "Choose Language:");
	Lcd_string(&lcd, "Choose Language:");

	centerString(&lcd, 1, "E. Espanol");
	Lcd_string(&lcd, "E. Espanol");

	centerString(&lcd, 2, "F. Francais");
	Lcd_string(&lcd, "F. Francais");

	centerString(&lcd, 3, "D. Deutsch");
	Lcd_string(&lcd, "D. Deutsch");
}

void startUpScreen()
{
	Lcd_clear(&lcd);
	if (strcmp(languageCode, "Espanol") == 0)
	{
		centerString(&lcd, 1, "Presione 'Start'");
		Lcd_string(&lcd, "Presione 'Start'");

		centerString(&lcd, 2, "para comenzar");
		Lcd_string(&lcd, "para comenzar");
	}

	else if (strcmp(languageCode, "Francais") == 0)
	{
		centerString(&lcd, 1, "Appuyez sur 'Start'");
		Lcd_string(&lcd, "Appuyez sur 'Start'");

		centerString(&lcd, 2, "pour commencer");
		Lcd_string(&lcd, "pour commencer");
	}

	else if (strcmp(languageCode, "Deutsch") == 0)
	{
		centerString(&lcd, 1, "Drucken Sie 'Start'");
		Lcd_string(&lcd, "Drucken Sie 'Start'");

		centerString(&lcd, 2, "um zu beginnen");
		Lcd_string(&lcd, "um zu beginnen");
	}

}

void processSpecialKey(char key, int correct)
{
	switch (key)
	{
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
			if(correct)
			{
				handleCorrectWord();
			}

			else
			{
				handleIncorrectWord();
			}

			break;

		default:
			break;
	}

}

// Helper function to handle repeating the word
void repeatAudio()
{
	//GPIO Logic helps handle concurrency problems
	HAL_GPIO_WritePin(YellowLED_GPIO_Port, YellowLED_Pin, GPIO_PIN_RESET);

	const char *languageFolder;

	if (strcmp(languageCode, "Espanol") == 0)
	{
		languageFolder = "Spanish";
	}

	else if (strcmp(languageCode, "Francais") == 0)
	{
		languageFolder = "French";
	}

	else if (strcmp(languageCode, "Deutsch") == 0)
	{
		languageFolder = "German";
	}

	snprintf(wavFileName, sizeof(wavFileName), "%s/%d.wav", languageFolder, fileIndices[current_index]);

	if (!wavPlayer_fileSelect(wavFileName));

	wavPlayer_play();

	while (!wavPlayer_isFinished())
	{
		wavPlayer_process();
	}

	wavPlayer_stop();

	//GPIO Logic helps handle concurrency problems
	HAL_GPIO_WritePin(YellowLED_GPIO_Port, YellowLED_Pin, GPIO_PIN_RESET);
}

// Helper function to play the next file and handle index reset
void playNextFile()
{
	// Check if current_index exceeds NUM_FILES
	if (atomic_load(&current_index) >= NUM_FILES) {
		// Shuffle the files again and reset the index
		fisherYatesShuffle(fileIndices, NUM_FILES);
		atomic_store(&current_index, 1);
	}

	const char *languageFolder;

	if (strcmp(languageCode, "Espanol") == 0)
	{
		languageFolder = "Spanish";
	}

	else if (strcmp(languageCode, "Francais") == 0)
	{
		languageFolder = "French";
	}

	else if (strcmp(languageCode, "Deutsch") == 0)
	{
		languageFolder = "German";
	}

	snprintf(wavFileName, sizeof(wavFileName), "%s/%d.wav", languageFolder, fileIndices[current_index]);
	snprintf(txtFileName, sizeof(txtFileName), "%s/%d.txt", languageFolder, fileIndices[current_index]);


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

    //GPIO Logic helps handle concurrency problems
    HAL_GPIO_WritePin(YellowLED_GPIO_Port, YellowLED_Pin, GPIO_PIN_RESET);
}

void playTheGame()
{
   //GPIO Logic helps handle concurrency problems
	HAL_GPIO_WritePin(YellowLED_GPIO_Port, YellowLED_Pin, GPIO_PIN_RESET);

	if (!wavPlayer_fileSelect("start.wav"));

	wavPlayer_play();

	while (!wavPlayer_isFinished())
	{
		wavPlayer_process();
	}

	wavPlayer_stop();

    //GPIO Logic helps handle concurrency problems
    HAL_GPIO_WritePin(YellowLED_GPIO_Port, YellowLED_Pin, GPIO_PIN_RESET);
}

void playCorrectSound()
{
	//GPIO Logic helps handle concurrency problems
	HAL_GPIO_WritePin(YellowLED_GPIO_Port, YellowLED_Pin, GPIO_PIN_RESET);
	if (!wavPlayer_fileSelect("ding.wav"));

	wavPlayer_play();

	while (!wavPlayer_isFinished())
	{
		wavPlayer_process();
	}

	wavPlayer_stop();
	//GPIO Logic helps handle concurrency problems
	HAL_GPIO_WritePin(YellowLED_GPIO_Port, YellowLED_Pin, GPIO_PIN_RESET);
}

void playWrongSound()
{
	//GPIO Logic helps handle concurrency problems
	HAL_GPIO_WritePin(YellowLED_GPIO_Port, GreenLED_Pin, GPIO_PIN_RESET);
	if (!wavPlayer_fileSelect("x.wav"));

	wavPlayer_play();

	while (!wavPlayer_isFinished())
	{
		wavPlayer_process();
	}

	wavPlayer_stop();
	//GPIO Logic helps handle concurrency problems
	HAL_GPIO_WritePin(YellowLED_GPIO_Port, GreenLED_Pin, GPIO_PIN_RESET);
}

void startApplication()
{
	//THIS LINE CAUSES THE TIMING ISSUES WITH KEYS
	//while (!(initializeDAC_USB()));

    // Initialize and shuffle file indices
    initializeIndices(fileIndices, NUM_FILES);
    fisherYatesShuffle(fileIndices, NUM_FILES);

    // silly intro
	//playTheGame();

    // Reset current index to 1
    atomic_store(&current_index, 1);
    atomic_store(&helpCounter, 0);
    atomic_store(&enterCounter, 0);

    playNextFile();
}


void handleCorrectWord()
{
    // Clear screen and reset variables
    Lcd_clear(&lcd);
    screenRow = 0;
    screenColumn = 0;
    moveCursor(&lcd, 0, 0);
    memset(current_word, 0, sizeof(current_word));  // Reset current_word to empty

    showEncouragement();

    // To turn on the Green LED
    HAL_GPIO_WritePin(GreenLED_GPIO_Port, GreenLED_Pin, GPIO_PIN_SET);

	playCorrectSound();

	atomic_fetch_add(&score, 1);

    HAL_Delay(999);

    // turn off green LED
    HAL_GPIO_WritePin(GreenLED_GPIO_Port, GreenLED_Pin, GPIO_PIN_RESET);

    Lcd_clear(&lcd);

    HAL_Delay(10);

    // reset atomic counters
    atomic_store(&helpCounter, 0);
    atomic_store(&enterCounter, 0);
    atomic_fetch_add(&current_index, 1);

    playNextFile();
}


//end
void endApplication()
{
	Lcd_clear(&lcd);
	turnOffCursor(&lcd);
	centerString(&lcd, 1, "Score:");
	Lcd_string(&lcd, "Score:");
	Lcd_int(&lcd, atomic_load(&score));

	HAL_Delay(1999);

	chooseLanguageScreen();
}

//locking issues with counters
void handleHelpFunction()
{
    // Increment the help counter atomically
    int currentHelpCount = atomic_fetch_add(&helpCounter, 1) + 1;

    // Check if helpCounter has reached the limit (e.g., 3)
    if (currentHelpCount >= 3)
    {
        // Calculate the current length of current_word
        size_t currentLength = strlen(current_word);

        // Check if current_word matches the beginning of expected_word
		if (strncmp(current_word, expected_word, currentLength) != 0)
		{
			// If it doesn't match, reset current_word
			memset(current_word, 0, sizeof(current_word));
			currentLength = 0;
			Lcd_clear(&lcd);
		}

        // Check if current_word is not already fully revealed
        if (currentLength < strlen(expected_word))
        {
            // Clear the LCD and reset the displayed content
            Lcd_clear(&lcd);

            // Reveal the next letter from expected_word and add it to current_word
            current_word[currentLength] = expected_word[currentLength];
            current_word[currentLength + 1] = '\0';  // Null-terminate current_word

            // Display the updated current_word on the LCD
            Lcd_string(&lcd, current_word);

            // Move the cursor to the next position
            screenRow = 0;
            screenColumn = currentLength;
            moveCursor(&lcd, &screenRow, &screenColumn);

            HAL_Delay(10);
            repeatAudio();

            // If the word is fully revealed, show correction and reset
            if (strlen(current_word) == strlen(expected_word))
            {
                showCorrection();
                HAL_Delay(999);
                handleNewPlayAfterRevealingWord();
            }
        }

        else
        {
            // If the word is already completed, repeat the audio instead
            repeatAudio();
        }
    }

    else
    {
        // If the helpCounter has not reached the limit, repeat the audio
        repeatAudio();
    }
}


void handleNewPlayAfterRevealingWord()
{
	// Clear screen and reset variables
	Lcd_clear(&lcd);
	screenRow = 0;
	screenColumn = 0;
	moveCursor(&lcd, 0, 0);
	memset(current_word, 0, sizeof(current_word));  // Reset current_word to empty

	// To turn on the Yellow LED
	HAL_GPIO_WritePin(YellowLED_GPIO_Port, YellowLED_Pin, GPIO_PIN_SET);

	HAL_Delay(999);

	// To turn off the Yellow LED
	HAL_GPIO_WritePin(YellowLED_GPIO_Port, YellowLED_Pin, GPIO_PIN_RESET);

	atomic_store(&helpCounter, 0);
	atomic_store(&enterCounter, 0);
	atomic_fetch_add(&current_index, 1);
	playNextFile();
}

//handling for incorrect word entered
void handleIncorrectWord()
{
	int currentEnterCount = atomic_fetch_add(&enterCounter, 1) + 1;

	if(currentEnterCount >= 5)
	{
		showCorrection();
		HAL_Delay(999);
		handleNewPlayAfterRevealingWord();
		return;
	}

	// To turn on the Yellow LED
	HAL_GPIO_WritePin(YellowLED_GPIO_Port, YellowLED_Pin, GPIO_PIN_SET);

	playWrongSound();

	HAL_Delay(999);

	// To turn off the Yellow LED
	HAL_GPIO_WritePin(YellowLED_GPIO_Port, YellowLED_Pin, GPIO_PIN_RESET);
}

void showCorrection()
{
    Lcd_clear(&lcd);
    Lcd_cursor(&lcd, 0, 0);

    if (strcmp(languageCode, "Espanol") == 0)
    {
    	centerString(&lcd, 1, "Respuesta Correcto: ");
    	Lcd_string(&lcd, "Respuesta Correcto: ");
    }


    else if (strcmp(languageCode, "Francais") == 0)
    {
    	centerString(&lcd, 1, "Bonne Reponse: ");
    	Lcd_string(&lcd, "Bonne Reponse: ");
    }

    else if (strcmp(languageCode, "Deutsch") == 0)
    {
    	centerString(&lcd, 1, "Richtige Antwort: ");
    	Lcd_string(&lcd, "Richtige Antwort: ");
    }

    centerString(&lcd, 2, expected_word);
    Lcd_string(&lcd, expected_word);
}

void showEncouragement()
{
    int randomIndex = rand() % 6;  // Get a random message
    Lcd_clear(&lcd);
    Lcd_cursor(&lcd, 0, 0);
    if (strcmp(languageCode, "Espanol") == 0)
	{
    	centerString(&lcd, 1, (char *)encouragementMessagesSpanish[randomIndex]);
    	Lcd_string(&lcd, (char *)encouragementMessagesSpanish[randomIndex]);
	}

	else if (strcmp(languageCode, "Francais") == 0)
	{
		centerString(&lcd, 1, (char *)encouragementMessagesFrench[randomIndex]);
		Lcd_string(&lcd, (char *)encouragementMessagesFrench[randomIndex]);
	}

	else if (strcmp(languageCode, "Deutsch") == 0)
	{
		centerString(&lcd, 1, (char *)encouragementMessagesGerman[randomIndex]);
		Lcd_string(&lcd, (char *)encouragementMessagesGerman[randomIndex]);
	}
}




