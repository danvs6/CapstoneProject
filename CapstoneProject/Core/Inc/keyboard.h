#ifndef INC_KEYBOARD_H_
#define INC_KEYBOARD_H_

#include <stdint.h>
#include <stdio.h>
#include "gpio.h"

// Set the control pins based on the channel (binary)
void setMuxChannel(uint8_t channel);

// Function to check the COM pin for a key press (returns 1 if pressed, 0 if not)
uint8_t readMuxInput();

// Function to scan MUX channels 0 to 10 and check for key presses (scans columns)
int scanColumns();

// scan the rows of the matrix for key presses
int scanRows();

// Scan the matrix keyboard continuously
void scanKeyboardMatrix();

#endif /* INC_KEYBOARD_H_ */
