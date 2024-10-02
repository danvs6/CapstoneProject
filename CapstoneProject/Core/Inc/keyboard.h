#ifndef INC_KEYBOARD_H_
#define INC_KEYBOARD_H_

#define TOTAL_ROWS 3
#define TOTAL_COLUMNS 11 // controlled by mux

// Set the control pins based on the channel (binary)
void setMuxChannel(uint8_t channel);

// Read the state of the COM pin (connected to the MUX)
uint8_t readMuxInput(void);

// Scan the matrix keyboard continuously
void scanKeyboardMatrix(void);

#endif /* INC_KEYBOARD_H_ */
