#include "keyboard.h"


// Set the control pins based on the channel (binary)
void setMuxChannel(uint8_t channel)
{
	// Set S0 and S1 on GPIOC
	HAL_GPIO_WritePin(GPIOC, S0_PIN_Pin, (channel & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET); // checks bit 0
	HAL_GPIO_WritePin(GPIOC, S1_PIN_Pin, (channel & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET); // checks bit 1

	// Set S2 and S3 on GPIOA
	HAL_GPIO_WritePin(GPIOA, S2_PIN_Pin, (channel & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET); // checks bit 2
	HAL_GPIO_WritePin(GPIOA, S3_PIN_Pin, (channel & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET); // checks bit 3
}

// Function to check the COM pin for a key press (returns 1 if pressed, 0 if not)
uint8_t readMuxInput()
{
	// Read the state of the COM pin (HIGH means no key pressed, LOW means key pressed)
	return HAL_GPIO_ReadPin(GPIOA, COM_PIN_Pin) == GPIO_PIN_RESET ? 1 : 0;
}

// Function to scan MUX channels 0 to 10 and check for key presses
int scanColumns()
{
    for (uint8_t column = 0; column < 10; column++)
    {
        // Set the MUX to the current column
        setMuxChannel(column);

        HAL_Delay(0); // 1 ms

        // Check if a key press is detected in this column (COM pin is LOW)
        if (readMuxInput())
        {
            return column;  // Return the column where a key press was detected
        }
    }

    return -1; // No key press detected in any column
}


int scanRows()
{
	// Read each row GPIO pin
    if (HAL_GPIO_ReadPin(GPIOE, ROW1_PIN) == GPIO_PIN_RESET)
    {
        return 1; // Row 1 pressed
    }

    if (HAL_GPIO_ReadPin(GPIOE, ROW2_PIN) == GPIO_PIN_RESET)
    {
        return 2; // Row 2 pressed
    }

    if (HAL_GPIO_ReadPin(GPIOE, ROW3_PIN) == GPIO_PIN_RESET)
    {
        return 3; // Row 3 pressed
    }

    return -1; // no keypress
}

void scanKeyboardMatrix()
{
	// scan columns using MUX
	int column = scanColumns();

	if (column != -1)
	{
		printf("Key Press Detected in Column %d/n", column);
	}

	int row = scanRows();

	if (row != -1)
	{
		printf("Key Press Detected in Row %d/n", row);
		// Handle the key press event here...
	}

}







