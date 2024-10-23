#ifndef INC_KEYBOARD_H_
#define INC_KEYBOARD_H_

#include <stdint.h>
#include <stdio.h>
#include "gpio.h"
#include "screen.h"

// Special Keys
#define KEY_START 0xF0
#define KEY_END 0xF1
#define KEY_HELP 0xF2
#define KEY_ENTER 0xF3
#define KEY_DELETE 0xF4
#define KEY_SPACEBAR 0xF5

// Set the control pins based on the channel (binary)
void setMuxChannel(uint8_t channel);

// Function to check the COM pin for a key press (returns 1 if pressed, 0 if not); scans columns
uint8_t readMuxInput();

// get the key press from the keyboard
char getKeyPressed(uint8_t row, uint8_t col);

// handle key press of each key
void processKeyPress(char key, Lcd_HandleTypeDef *lcd, int *screenRow, int *screenColumn);

// readjust rows due to clock cycle
uint8_t rowReadjustment(uint8_t current_row);


char scanKeyboard(Lcd_HandleTypeDef *lcd, int *screenRow, int *screenColumn);

#endif /* INC_KEYBOARD_H_ */
