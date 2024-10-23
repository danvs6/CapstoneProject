#include <stdio.h>
#include <string.h>
#include "keyboard.h"
#include "screen.h"
#include "gpio.h"
#include "tim.h"

// External variables from main.c
extern uint8_t columnNumber;
extern uint8_t current_row;
extern int keyDetected;

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
	// Handle special keys first
	if (key == KEY_DELETE)
	{
		deletePreviousChar(lcd, screenRow, screenColumn);
	}

	else if (key == KEY_SPACEBAR)
	{
		moveCursor(lcd, screenRow, screenColumn);
	}

    else
    {
        char keyString[2] = {key, '\0'};  // Convert to string for LCD display
        Lcd_string(lcd, keyString);
        moveCursor(lcd, screenRow, screenColumn);
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

// Function to scan the keyboard matrix
//char scanKeyboard(Lcd_HandleTypeDef *lcd, int *screenRow, int *screenColumn)
//{
//    for (columnNumber = 0; columnNumber < 11; columnNumber++)  // Cycle through all columns
//    {
//        setMuxChannel(columnNumber);  // Set the multiplexer to the current column
//
//        // Check if a key is pressed in the current column
//        if (readMuxInput() == 0)  // 0 indicates a key press in the current column
//        {
//            HAL_Delay(1); // Short delay for debouncing
//
//            // Check if key press is still detected after debounce
//            if (readMuxInput() == 0)
//            {
//                keyDetected = 1;
//
//                // readjust row due to clock
//                current_row = rowReadjustment(current_row);
//
//                // Register the key press
//                char key = processKeyPress(current_row, columnNumber);  // Get key from row/column
//
//                if (key == KEY_DELETE)
//                {
//                    deletePreviousChar(lcd, screenRow, screenColumn);
//                }
//                else if (key == KEY_SPACEBAR)
//                {
//                    moveCursor(lcd, screenRow, screenColumn);
//                }
//                else
//                {
//                    char keyString[2] = {key, '\0'};  // Convert to string for LCD display
//                    Lcd_string(lcd, keyString);
//                    moveCursor(lcd, screenRow, screenColumn);
//                }
//
//                // Wait for key release before continuing
//                while (readMuxInput() == 0)
//                {
//                    HAL_Delay(5);  // Small delay to avoid excessive checking and CPU usage
//                }
//
//                keyDetected = 0;
//                return key;
//            }
//        }
//    }
//    return '\0';
//}







