#include "ESLApplication.h"

extern int screenColumn;
extern int screenRow;
extern char current_word[32] ;  // Global word buffer with space for 32 characters

void processSpecialKey(char key, int correct){
	switch (key){
		case KEY_START:
			// Handle KEY_START
			startApplication();
			break;

		case KEY_END:
			// Handle KEY_END
			endApplication();
			break;

		case KEY_HELP:
			// Handle KEY_HELP
			handleHelpFunction();
			break;

		case KEY_ENTER:
			// Handle KEY_ENTER
			if(correct){
				//handle correct entry
				handleCorrectWord();
			}
			else{
				//handle incorrect entry
				handleIncorrectWord();
			}

			break;

		default:
			break;
	}

}

//initializations
void startApplication(){

}

//end
void endApplication(){

}

void handleHelpFunction(){

}

//handling for correct word entered
void handleCorrectWord(){

}

//handling for incorrect word entered
void handleIncorrectWord(){

}

