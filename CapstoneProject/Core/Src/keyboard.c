#include <stdio.h>
#include <string.h>
#include "keyboard.h"
#include "screen.h"

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
char handleKeyPress(uint8_t row, uint8_t col)
{
	 return getKeyPressed(row, col);  // Get the key value from keyboard mapping
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
        current_row -= 1;  // Otherwise, decrement the row
    }

    return current_row;
}







