#ifndef INC_KEYBOARD_H_
#define INC_KEYBOARD_H_

#define TOTAL_ROWS 3
#define TOTAL_COLUMNS 11 // controlled by mux

// Set the control pins based on the channel (binary)
void setMuxChannel(uint8_t channel);

// Function to check the COM pin for a key press (returns 1 if pressed, 0 if not)
uint8_t readMuxInput(void);

// Function to scan MUX channels 0 to 10 and check for key presses
void scanMuxChannels();

// Scan the matrix keyboard continuously
void scanKeyboardMatrix(void);

#endif /* INC_KEYBOARD_H_ */
