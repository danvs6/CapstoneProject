#include <stdio.h>
#include <string.h>
#include <stdatomic.h>
#include "keyboard.h"
#include "screen.h"
#include "gpio.h"
#include "tim.h"
#include "ESLApplication.h"

// External variables from main.c
extern uint8_t columnNumber;
extern uint8_t current_row;
extern atomic_int keyDetected;
extern char current_word[80];
extern char expected_word[80];

atomic_int started = 0;
int languageChosen = 0;
atomic_int USB_detected = 0;
char languageCode[10] = "";

// keyboard mapping
char keyboardMap[3][11] = {
		{KEY_START, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'}, // row 1
		{KEY_END, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', KEY_DELETE}, // row 2
		{KEY_HELP, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', KEY_ENTER, KEY_SPACEBAR} // row 3
};

// Set the control pins based on the channel (binary)
void setMuxChannel(uint8_t channel)
{
	// Set S0 and S1 on GPIOC
	HAL_GPIO_WritePin(S0_GPIO_Port, S0_Pin, (channel & 0x01) ? SET : RESET); // checks bit 0
	HAL_GPIO_WritePin(S1_GPIO_Port, S1_Pin, (channel & 0x02) ? SET : RESET); // checks bit 1

	// Set S2 and S3 on GPIOA
	HAL_GPIO_WritePin(S2_GPIO_Port, S2_Pin, (channel & 0x04) ? SET : RESET); // checks bit 2
	HAL_GPIO_WritePin(S3_GPIO_Port, S3_Pin, (channel & 0x08) ? SET : RESET); // checks bit 3
}

// Function to check the COM pin for a key press (returns 0 if pressed, 1 if not)
uint8_t readMuxInput()
{
	// Read the state of the COM pin (HIGH means no key pressed, LOW means key pressed)
	return HAL_GPIO_ReadPin(COM_GPIO_Port, COM_Pin);
}

// get the key press from the keyboard
char getKeyPressed(uint8_t row, uint8_t col)
{
    return keyboardMap[row][col];
}

// handle key press of each key
void processKeyPress(char key, Lcd_HandleTypeDef *lcd, int *screenRow, int *screenColumn)
{
	if(atomic_load(&started))
	{
		HAL_Delay(10);
		switch (key)
		{
			case KEY_DELETE:
			{
				// Remove the last character from the word
				size_t len = strlen(current_word);
				if (len > 0)
				{
					current_word[len - 1] = '\0';  // Remove the last character
				}
				deletePreviousChar(lcd, screenRow, screenColumn);  // Update display
				break;
			}

			case KEY_SPACEBAR:
			{
				// Add space if there's room in current_word and on the screen
				size_t len = strlen(current_word);
				if (len < sizeof(current_word) - 1 && (*screenRow < 4 && *screenColumn <= 19))  // Allow up to 32 characters
				{
					current_word[len] = ' ';  // Add space
					current_word[len + 1] = '\0';  // Null-terminate
					moveCursor(lcd, screenRow, screenColumn);  // Move cursor forward
				}
				break;
			}

			case KEY_ENTER:
			{
				// Check if the typed word matches the expected word
				if (strcmp(current_word, expected_word) == 0)
				{
					HAL_Delay(20);
					processSpecialKey(key, 1);
					HAL_Delay(20);
				}
				else
				{
					// Handle incorrect entry
					HAL_Delay(20);
					processSpecialKey(key, 0);
					HAL_Delay(20);
				}
				break;
			}

			case KEY_END:
			{
				atomic_store(&started, 0);

				HAL_Delay(20);
				processSpecialKey(key, 1);
				HAL_Delay(20);

				break;
			}

			case KEY_HELP:
			{
				HAL_Delay(20);
				processSpecialKey(key, 1);
				HAL_Delay(20);

				break;
			}

			case KEY_START:
			{
				HAL_Delay(20);
				break; // do nothing
			}

			default:
			{
				// Add regular key to current_word, if there's space in current_word and on the screen
				size_t len = strlen(current_word);
				if (len < sizeof(current_word) - 1 && (*screenRow < 4 && *screenColumn <= 19))  // Allow up to 32 characters
				{
					current_word[len] = key;
					current_word[len + 1] = '\0';  // Null-terminate

					// Display the key on the LCD
					char keyString[2] = {key, '\0'};  // Convert to string for LCD display
					Lcd_string(lcd, keyString);  // Display the character
					moveCursor(lcd, screenRow, screenColumn);  // Move cursor forward
				}

				break;
			}
		}
	}

	else // application not started
	{
		if(key == KEY_START && languageChosen)
		{
			atomic_store(&started, 1);
			Lcd_clear(lcd);  // Clear the display
			turnOnCursor(lcd); // Set cursor
			(*screenRow) = 0;
			(*screenColumn) = 0;
			moveCursor(lcd, 0,0); //move cursor to first position
			memset(current_word, 0, sizeof(current_word));  // Reset current_word to empty
			HAL_Delay(20);
			processSpecialKey(key, 1);
			HAL_Delay(20);
		}

		else if (key == 'E' && atomic_load(&USB_detected))
		{
			strcpy(languageCode, "Espanol"); // copy string into language code
			languageChosen = 1;
			startUpScreen();
		}

		else if (key == 'F' && atomic_load(&USB_detected))
		{
			strcpy(languageCode, "Francais"); // copy string into language code
			languageChosen = 1;
			startUpScreen();
		}

		else if (key == 'D' && atomic_load(&USB_detected))
		{
			strcpy(languageCode, "Deutsch"); // copy string into language code
			languageChosen = 1;
			startUpScreen();
		}
	}
}

// readjust rows due to clock cycle
uint8_t rowReadjustment(uint8_t current_row)
{
    if (current_row == 0)
    {
        current_row = 2;  // If current row is 0, wrap around to 2
    }

    else
    {
        current_row --;  // Otherwise, decrement the row
    }

    return current_row;
}

// keyboard matrix scanner
void scanKeyboard(Lcd_HandleTypeDef *lcd, int *screenRow, int *screenColumn)
{
	for (columnNumber = 0; columnNumber < 11; columnNumber++)  // Cycle through all columns
	{
		setMuxChannel(columnNumber);  // Set the multiplexer to the current column

		// Check if a key is pressed in the current column
		if (readMuxInput() == 0)  // 0 indicates a key press in the current column
		{
			HAL_Delay(1);  // Short delay for debouncing

			// Check if key press is still detected after debounce
			if (readMuxInput() == 0)
			{
				atomic_store(&keyDetected, 1);  // Stop the timer from iterating through rows

				// Adjust the row due to the clock
				current_row = rowReadjustment(current_row);

				// Register the key press
				char key = getKeyPressed(current_row, columnNumber);  // Get key from row/column

				// Process the key press
				processKeyPress(key, lcd, screenRow, screenColumn);

				// Wait until the key is released to avoid key repetition
				while (readMuxInput() == 0);

				// Reset keyDetected after the key is released
				atomic_store(&keyDetected, 0);
				break;
			}
		}
	}
}






