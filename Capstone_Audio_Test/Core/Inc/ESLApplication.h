#ifndef INC_ESLAPPLICATION_H_
#define INC_ESLAPPLICATION_H_

#define KEY_START 0xF0
#define KEY_END 0xF1
#define KEY_HELP 0xF2
#define KEY_ENTER 0xF3
#define KEY_DELETE 0xF4
#define KEY_SPACEBAR 0xF5

void startUpScreen();

void processSpecialKey(char key, int correct);

//initializations
void startApplication();

//end
void endApplication();

//help user
void handleHelpFunction();

//handling for correct word entered
void handleCorrectWord();

//handling for incorrect word entered
void handleIncorrectWord();

#endif /* INC_ESLAPPLICATION_H_ */
