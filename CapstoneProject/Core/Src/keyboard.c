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
uint8_t readMuxInput(void)
{
	// Read the state of the COM pin (HIGH means no key pressed, LOW means key pressed)
	return HAL_GPIO_ReadPin(COM_PORT, COM_PIN) == GPIO_PIN_RESET ? 1 : 0;
}

// Function to scan MUX channels 0 to 10 and check for key presses
void scanMuxChannels()
{
	for (uint8_t channel = 0; channel < 11; channel++)
	{
		setMuxChannel(channel); // set the current channel

		HAL_Delay(0); // 1 ms delay

		if (readMuxInput())
		{
			// A key is pressed, handle the key press
		}
	}
}







