/*  Arduino Virtual pet
    Embedded Systems Programming Coursework
    Created by Vishwaswaroop P Bennur
    Dated 17 April 2019
*/
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include <TimeLib.h>
#include <EEPROM.h>
#include "bitmaps.h" //get Characters from the C header file name "bitmaps.h"
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

#define STILL 0
#define MENU 1
#define FEED 2
#define PLAY 3
#define GROW 5
#define OLD_OR_NEW 6

int ds = 0;
int happiness = 2;
int fullness = 3;
unsigned int age;
unsigned int age1;
int state;

boolean programJustStarted = true;
int row = 0;
int column = 0;
unsigned int happinessTimer;
unsigned int timeSinceHappinessReduced;
unsigned int timeSinceFullnessReduced;
unsigned int fullnessTimer;
unsigned int timeSinceAgeChanged;
int character;

void setup() {

  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.createChar(2, rightpackman);
  lcd.createChar(1, packman2);
  lcd.createChar(3, smiley);
  lcd.createChar(4, food);
  lcd.createChar(5, leftpackman);
  state =  MENU;
  character = 2;//

}


String inMins(int seconds) {

  //this function returns time in minutes:seconds format
  int mins = seconds / 60;
  int secs = seconds % 60;
  char strtime[7];
  sprintf(strtime, "%02u:%02u", mins, secs);
  return strtime;

}



void liferules(unsigned int age) {


  if (age >= 5 & age <= 35)
    ds = 1;

  timeSinceHappinessReduced = age - happinessTimer;
  timeSinceFullnessReduced = age - fullnessTimer;

  if (age >= 5 & (happiness > 0 | fullness > 0)) {
    //This condition is used to periodically reduce happiness  and fullness

    if (((age - 5) % 7) == 0 & happiness > 0 & timeSinceHappinessReduced >= 1 & age > 5 ) {

      //Happiness is reduced once every seven seconds
      happiness--;
      happinessTimer = age ;

    }


    if (((age - 5) % 11) == 0 & fullness > 0 &  age > 5 &  timeSinceFullnessReduced >= 1) {

      //Fullness is reduced once every eleven seconds
      fullness--;
      fullnessTimer = age;
      if (fullness == 0)
        happiness = 0;
    }
  }

}

bool thereIsSavedPet() {

  //This function checks whether there is a saved pet

  if (EEPROM.read(3) == 0) //Since the age is stored in the third postion of EEPROM
    return false;

  return true;

}


void writeAgeToEEPROM(unsigned int age) {
  /*This function writes age to EEPROM.
     Since each EEPROM position can save one byte only,
     mulitple positions are used to save the age,
     which is later added when the user wants to use these values
  */

  if (age <= 200)
    EEPROM.write(3, age);

  else if (age <= 400) {
    EEPROM.write(3, 200);
    EEPROM.write(4, (age - 200));
  }

  else {
    EEPROM.write(3, 200);
    EEPROM.write(4, 200);
    EEPROM.write(5, (age - 400));
  }
}


void loop() {

  if (age < 600) {
    uint8_t buttons = lcd.readButtons();

    switch (state) {

      case STILL:

        if ((millis() / 1000) - timeSinceAgeChanged >= 1) {
          age++;
          timeSinceAgeChanged = (millis() / 1000);
        }
        liferules(age);
        lcd.setCursor(0, 0);
        lcd.write(3);
        lcd.print("(L)");
        lcd.print(happiness);
        lcd.setCursor(0, 1);
        lcd.write(4);
        lcd.print("(D)-");
        lcd.print(fullness);
        lcd.setCursor(7, 0);
        lcd.print("DS(R)-");
        lcd.print(ds);
        lcd.setCursor(7, 1);
        lcd.print("AGE-");
        lcd.print(inMins(age));


        if (buttons == 1) { // SELECT

          state = MENU;
          lcd.clear();

        }

        if (buttons == 8 ) // UP
          state = STILL;

        if (buttons == 4  and ds >= 1) //DOWN
          state = FEED;

        if (buttons == 16 and ds >= 1) // LEFT
          state = PLAY;

        if (buttons == 2  and ds >= 1) // RIGHT
          state = GROW;

        break;

      case MENU:

        if (programJustStarted) {
          /*This conditon is satisfied only when once,
             i.e., just when the program starts.
          */
          state = OLD_OR_NEW;
          programJustStarted = false;

        }

        if (state == MENU) {

          lcd.setCursor(0, 0);
          lcd.print("MENU(S): ");
          lcd.print("SAVE-U");
          lcd.setCursor(0, 1);
          lcd.print("DF-L DLT-D RST-R ");

          if (buttons == 8) {

            // Pressing up causes the program to save the data and stop running.
            EEPROM.write(0, ds);
            EEPROM.write(1, happiness);
            EEPROM.write(2, fullness);
            writeAgeToEEPROM(age);
            age = 601;//stop running

          }

          if (buttons == 4) {
            lcd.clear();
            lcd.print("DELETING...");
            lcd.blink();
            delay(2000);
            for (int i = 0; i < 6; i++) {
              EEPROM.write(i, 0);
            }
            lcd.noBlink();
            lcd.setCursor(0, 1);
            lcd.print("Done");
            delay(1000);
            lcd.clear();

          }

          if (buttons == 2) {
            //Pressing right causes the program to reset the running program.
            lcd.clear();
            lcd.print("RESETTING...");
            lcd.blink();
            delay(2000);
            ds = 0;
            happiness = 2;
            fullness = 3;
            age = 0;
            lcd.noBlink();
            lcd.setCursor(0, 1);
            lcd.print("DONE");
            delay(1000);
            lcd.clear();
          }

          if (buttons == 16) {
            //Pressing left causes the program to switch to STILL state
            state = STILL;
            lcd.clear();
          }
        }
        break;


      case FEED:

        lcd.clear();

        if (fullness <= 3) {
          /* Creates an animation where the pet
              moves forward and eat the ham.
          */
          lcd.print("Feeding");
          lcd.blink();
          delay(2000);
          lcd.noBlink();
          lcd.clear();
          lcd.setCursor(15, 0);
          lcd.write(4);
          for (int i = 0; i < 16; i++) {

            lcd.setCursor(i, 0);
            if (i % 2 == 0)
              lcd.write(1);
            else
              lcd.write(2);
            lcd.setCursor(i - 1, 0);
            lcd.print(" ");
            delay(200);

          }

          if (fullness < 3)
            fullness++;

          if (fullness == 3) {
            fullness++;
            happiness = 0;
          }

          lcd.clear();
          lcd.write(2);
          lcd.print("-THANKS!");
          delay(2000);
        }

        else
          lcd.print("Im full");

        lcd.clear();
        state = STILL ;
        break;


      case PLAY:

        lcd.clear();

        if (happiness<2 & fullness >= 2) {

          lcd.print("LETS PLAY!");
          delay(1000);
          lcd.clear();
          age1 = (millis() / 1000) + 7;

          while (age1 - (millis() / 1000) != 0) {
            /*This loop provides the user a 15 second
               simulation where the user can move the pet
               around using butttons.
            */
            uint8_t playButtons = lcd.readButtons();
            lcd.setCursor(column, row);
            lcd.write(character);

            if (playButtons == 8) { //UP

              lcd.setCursor(column, row);
              lcd.print(" ");
              row = 0;

            }

            if (playButtons == 4 ) { //DOWN

              lcd.setCursor(column, row);
              lcd.print(" ");
              row = 1;

            }

            if (playButtons == 16 && column > 0) { //LEFT

              lcd.setCursor(column, row);
              lcd.print(" ");
              column--;
              character = 5;

            }

            if (playButtons == 2 and column < 15) { //RIGHT

              lcd.setCursor(column, row);
              lcd.print(" ");
              column++;
              character = 2;

            }
          }

          happiness++;
          lcd.clear();
          lcd.print("TIMES UP!");
          lcd.setCursor(0, 1);
          lcd.print("Pet is happy");
          delay(2000);

        }

        else if (happiness >= 2) {

          lcd.print("Too much playin'");
          delay(2000);

        }

        else {

          lcd.print("Not full yet");
          delay(2000);

        }

        lcd.clear();
        state = STILL;
        break;

      case GROW:
        lcd.clear();
        if (age >= 35 & happiness >= 1 & fullness >= 3) {
          /* An animation where beam is created which
               hits the pet and cause growth.
          */
          lcd.print("Releasing beam");
          lcd.setCursor(0, 1);
          lcd.print("of GROWTH!...");
          lcd.blink();
          delay(2000);
          lcd.noBlink();
          lcd.clear();
          lcd.setCursor(14, 0);
          lcd.write(2);

          for (int i = 0; i < 14; i++) {

            lcd.setCursor(i, 0);
            lcd.print("-");
            delay(200);
            lcd.setCursor(i, 0);
            lcd.print(" ");

          }

          lcd.setCursor(15, 0);
          lcd.write(2);
          delay(2000);
          ds = 2;
          lcd.clear();
          lcd.print("GROWING DONE!");
          lcd.write(2);
          lcd.write(2);
          delay(2000);

        }

        else {

          lcd.print("NOT OLD ENOUGH!");
          delay(2000);

        }

        lcd.clear();
        state = STILL;
        break;

      case OLD_OR_NEW:
        /* The program switches to this state only when
            the arudino is just turned on / resetted.
            It checks whether there is a saved pet or not.
            If there is a saved pet, it offers the user to either
            use a new pet or continue to old pet.
            If there is no saved pet, the program automatically
            switches to MENU state.
        */
        lcd.setCursor(0, 0);

        if (thereIsSavedPet()) {

          lcd.print("Retrieve - L");
          lcd.setCursor(0, 1);
          lcd.print("Go Back - R");

          if (buttons == 16) { // LEFT

            ds = EEPROM.read(0);
            age = EEPROM.read(3) + EEPROM.read(4) + EEPROM.read(5);
            happiness = EEPROM.read(1);
            fullness = EEPROM.read(2);
            state = MENU;
            lcd.clear();
            Serial.println(age);

          }

          if (buttons == 2) { //RIGHT
            state = MENU;
            lcd.clear();
          }

        }

        else {

          lcd.clear();
          lcd.print("No saved pet");
          lcd.setCursor(0, 1);
          lcd.print("Going back");
          delay(1000);
          state = MENU;
          lcd.clear();

        }
    }
  }

  else
    lcd.print(" ");
}

// reset age not working .
