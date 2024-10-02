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

// Read the state of the COM pin (connected to the MUX); returns 0 or 1
uint8_t readMuxInput(void)
{
	return HAL_GPIO_ReadPin(COM_PORT, COM_PIN) == GPIO_PIN_RESET; // active low switches
}

// Scan the matrix keyboard continuously
void scanKeyboardMatrix(void)
{

}








