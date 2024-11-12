#include "screen.h"

const uint8_t ROW_16[] = {0x00, 0x40, 0x10, 0x50};
const uint8_t ROW_20[] = {0x00, 0x40, 0x14, 0x54};
/************************************** Static declarations **************************************/

static void lcd_write_data(Lcd_HandleTypeDef * lcd, uint8_t data);
static void lcd_write_command(Lcd_HandleTypeDef * lcd, uint8_t command);
static void lcd_write(Lcd_HandleTypeDef * lcd, uint8_t data, uint8_t len);


/************************************** Function definitions **************************************/

/**
 * Create new Lcd_HandleTypeDef
 */
Lcd_HandleTypeDef Lcd_create(
		Lcd_PortType port[], Lcd_PinType pin[],
		Lcd_PortType rs_port, Lcd_PinType rs_pin,
		Lcd_PortType en_port, Lcd_PinType en_pin, Lcd_ModeTypeDef mode)
{
	Lcd_HandleTypeDef lcd;

	lcd.mode = mode;

	lcd.en_pin = en_pin;
	lcd.en_port = en_port;

	lcd.rs_pin = rs_pin;
	lcd.rs_port = rs_port;

	lcd.data_pin = pin;
	lcd.data_port = port;

	Lcd_init(&lcd);

	return lcd;
}

/**
 * Initialize 16x2-lcd with cursor
 */
void Lcd_init(Lcd_HandleTypeDef * lcd)
{
	if(lcd->mode == LCD_4_BIT_MODE)
	{
			lcd_write_command(lcd, 0x33);
			lcd_write_command(lcd, 0x32);
			lcd_write_command(lcd, FUNCTION_SET | OPT_N);				// 4-bit mode
	}

	else
		lcd_write_command(lcd, FUNCTION_SET | OPT_DL | OPT_N);


	lcd_write_command(lcd, CLEAR_DISPLAY);						// Clear screen
	lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL | OPT_D | OPT_C | OPT_B);		// Lcd-on, cursor-on, blink
	lcd_write_command(lcd, ENTRY_MODE_SET | OPT_INC);			// Increment cursor
}

void turnOffCursor(Lcd_HandleTypeDef * lcd){
	lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL | OPT_D);
}

void turnOnCursor(Lcd_HandleTypeDef * lcd){
	lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL | OPT_D | OPT_C | OPT_B);
}
/**
 * Write a number on the current position
 */
void Lcd_int(Lcd_HandleTypeDef * lcd, int number)
{
	char buffer[11];
	sprintf(buffer, "%d", number);

	Lcd_string(lcd, buffer);
}

/**
 * Write a string on the current position
 */
void Lcd_string(Lcd_HandleTypeDef * lcd, char * string)
{
	for(uint8_t i = 0; i < strlen(string); i++)
	{
		lcd_write_data(lcd, string[i]);
	}
}

/**
 * Set the cursor position
 */
void Lcd_cursor(Lcd_HandleTypeDef * lcd, uint8_t row, uint8_t col)
{
	#ifdef LCD20xN
	lcd_write_command(lcd, SET_DDRAM_ADDR + ROW_20[row] + col);
	#endif

	#ifdef LCD16xN
	lcd_write_command(lcd, SET_DDRAM_ADDR + ROW_16[row] + col);
	#endif
}

/**
 * Clear the screen
 */
void Lcd_clear(Lcd_HandleTypeDef * lcd) {
	lcd_write_command(lcd, CLEAR_DISPLAY);
}

void Lcd_define_char(Lcd_HandleTypeDef * lcd, uint8_t code, uint8_t bitmap[])
{
	lcd_write_command(lcd, SETCGRAM_ADDR + (code << 3));

	for(uint8_t i=0;i<8;++i)
	{
		lcd_write_data(lcd, bitmap[i]);
	}

}

// move cursor
void moveCursor(Lcd_HandleTypeDef *lcd, int *screenRow, int *screenColumn)
{
    if (*screenColumn < 19)  // Column index 0-15 -> 0-19
    {
        (*screenColumn)++;  // Move to the next column
    }
    else
    {
        // Move to the next row only if you're on row 0
        if (*screenRow < 3)
        {
            (*screenRow)++;
            *screenColumn = 0;  // Reset column to 0 for the new row
        }
        else
        {
            *screenRow = 3;  // Stay on the last row (row 1)
            *screenColumn = 19;  // Stay at the last column (column 15 -> 19)
        }
    }

    // Set the new cursor position on the LCD
    Lcd_cursor(lcd, *screenRow, *screenColumn);
}

void deletePreviousChar(Lcd_HandleTypeDef *lcd, int *screenRow, int *screenColumn)
{
    // If we're not at the start of a row, just move back one column
    if (*screenColumn > 0)
    {
        (*screenColumn)--;  // Move to the previous column
    }
    // If we're at the start of a row, move to the end of the previous row
    else if (*screenRow > 0)
    {
        (*screenRow)--;  // Move to the previous row
        *screenColumn = 19;  // Set the column to the last column (15 -> 19) of the previous row
    }

    // Set the cursor to the new position
    Lcd_cursor(lcd, *screenRow, *screenColumn);

    // Replace the character with a space (delete it visually)
    Lcd_string(lcd, " ");

    // Move the cursor back to the deleted character's position
    Lcd_cursor(lcd, *screenRow, *screenColumn);
}

void turnDisplayOff(Lcd_HandleTypeDef *lcd)
{
	lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL);
}

void turnDisplayOn(Lcd_HandleTypeDef *lcd)
{
	lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL | OPT_D | OPT_C | OPT_B);
}

void centerString(Lcd_HandleTypeDef *lcd, int row, const char *str)
{
    int screenWidth = 20;  // Width of the 20x4 LCD screen
    int strLength = strlen(str);

    // Calculate the starting column to center the string
    int startColumn = (screenWidth - strLength) / 2;

    if (startColumn < 0) // Ensure startColumn isn't negative
    {
    	startColumn = 0;
    }

    // Set the cursor to the calculated position
    Lcd_cursor(lcd, row, startColumn);
}

/************************************** Static function definition **************************************/

/**
 * Write a byte to the command register
 */
void lcd_write_command(Lcd_HandleTypeDef * lcd, uint8_t command)
{
	HAL_GPIO_WritePin(lcd->rs_port, lcd->rs_pin, LCD_COMMAND_REG);		// Write to command register

	if(lcd->mode == LCD_4_BIT_MODE)
	{
		lcd_write(lcd, (command >> 4), LCD_NIB);
		lcd_write(lcd, command & 0x0F, LCD_NIB);
	}

	else
	{
		lcd_write(lcd, command, LCD_BYTE);
	}

}

/**
 * Write a byte to the data register
 */
void lcd_write_data(Lcd_HandleTypeDef * lcd, uint8_t data)
{
	HAL_GPIO_WritePin(lcd->rs_port, lcd->rs_pin, LCD_DATA_REG);			// Write to data register

	if(lcd->mode == LCD_4_BIT_MODE)
	{
		lcd_write(lcd, data >> 4, LCD_NIB);
		lcd_write(lcd, data & 0x0F, LCD_NIB);
	}

	else
	{
		lcd_write(lcd, data, LCD_BYTE);
	}

}

/**
 * Set len bits on the bus and toggle the enable line
 */
void lcd_write(Lcd_HandleTypeDef * lcd, uint8_t data, uint8_t len)
{
	for(uint8_t i = 0; i < len; i++)
	{
		HAL_GPIO_WritePin(lcd->data_port[i], lcd->data_pin[i], (data >> i) & 0x01);
	}

	HAL_GPIO_WritePin(lcd->en_port, lcd->en_pin, 1);
	HAL_Delay(1);
	HAL_GPIO_WritePin(lcd->en_port, lcd->en_pin, 0); 		// Data receive on falling edge
}



