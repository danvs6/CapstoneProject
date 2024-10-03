#include "keyboard.h"

int column = -1;
int row = -1;

char keyMatrix[3][11] = {
    {'Start', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'},
    {'End', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Delete'},
    {'HELP', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', 'Enter', 'Space', ''}
};

// Set the control pins based on the channel (binary)
void setMuxChannel(uint8_t channel)
{
	// Set S0 and S1 on GPIOC
	HAL_GPIO_WritePin(S0_PIN_GPIO_Port, S0_PIN_Pin, (channel & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET); // checks bit 0
	HAL_GPIO_WritePin(S1_PIN_GPIO_Port, S1_PIN_Pin, (channel & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET); // checks bit 1

	// Set S2 and S3 on GPIOA
	HAL_GPIO_WritePin(S2_PIN_GPIO_Port, S2_PIN_Pin, (channel & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET); // checks bit 2
	HAL_GPIO_WritePin(S3_PIN_GPIO_Port, S3_PIN_Pin, (channel & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET); // checks bit 3
}

// Function to check the COM pin for a key press (returns 1 if pressed, 0 if not)
uint8_t readMuxInput()
{
	// Read the state of the COM pin (HIGH means no key pressed, LOW means key pressed)
	return HAL_GPIO_ReadPin(COM_PIN_GPIO_Port, COM_PIN_Pin) == GPIO_PIN_RESET ? 1 : 0;
}

// Function to scan MUX channels 0 to 10 and check for key presses
int scanColumns()
{
    for (uint8_t column = 0; column < 11; column++)
    {
        // Set the MUX to the current column
        setMuxChannel(column);

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
    if (HAL_GPIO_ReadPin(ROW1_PIN_GPIO_Port, ROW1_PIN) == GPIO_PIN_RESET)
    {
        return 1; // Row 1 pressed
    }

    if (HAL_GPIO_ReadPin(ROW2_PIN_GPIO_Port, ROW2_PIN) == GPIO_PIN_RESET)
    {
        return 2; // Row 2 pressed
    }

    if (HAL_GPIO_ReadPin(ROW3_PIN_GPIO_Port, ROW3_PIN) == GPIO_PIN_RESET)
    {
        return 3; // Row 3 pressed
    }

    return -1; // no keypress
}

int setRowsScanColumns()
{
	// set rows 1 and 2, check columns corresponding to row 3
	HAL_GPIO_WritePin(ROW1_PIN_GPIO_Port, ROW1_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ROW2_PIN_GPIO_Port, ROW2_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ROW3_PIN_GPIO_Port, ROW3_PIN, GPIO_PIN_RESET);
	if (scanColumns() == column) {
		return 3;
	}
	// set rows 1 and 3, check columns corresponding to row 2
	HAL_GPIO_WritePin(ROW1_PIN_GPIO_Port, ROW1_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ROW2_PIN_GPIO_Port, ROW2_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ROW3_PIN_GPIO_Port, ROW3_PIN, GPIO_PIN_SET);
	if (scanColumns() == column) {
		return 2;
	}
	// set rows 2 and 3, check columns corresponding to row 1
	HAL_GPIO_WritePin(ROW1_PIN_GPIO_Port, ROW1_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ROW2_PIN_GPIO_Port, ROW2_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ROW3_PIN_GPIO_Port, ROW3_PIN, GPIO_PIN_SET);
	if (scanColumns() == column) {
		return 1;
	}
	return -1;
}

void scanKeyboardMatrix()
{
	// scan columns using MUX
	column = scanColumns();

	if (column != -1)
	{
		printf("Key Press Detected in Column %d/n", column);
		HAL_Delay(5); //prevents debouncing
		row = setRowsScanColumns();
	}

	if (column != -1 && row != -1){
		//handle logic
		printf("Key Press Detected at Row %d, Column %d\n", row, column);
		column = -1;
		row = -1;
	}
}







