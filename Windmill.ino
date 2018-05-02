#include <LiquidCrystal.h>                  //includes the liquid crystal library
LiquidCrystal lcd(8,9,10,11,12,13);         //initializes the liquid crystal display and associates the LCD with the pins shown
// LCD setup
int displaySetting = 1;                     //intializes the displaySetting variable
const int MAX_DISPLAYS = 3;                 //initializes the MAX_DISPLAYS variable
const int buttonPin = 3;                    //initializes the buttonPin variable
unsigned long lastDisplaySwitch = millis(); //initializes the lastDisplaySwitch as an unsigned long variable
const int DISPLAY_DELAY = 250;              //initializes the DISPLAY_DELAY variable as a constant integer of 250
//Tachometer Setup
const int interruptPin = 2;                 //initializes the interruptPin as a constant integer

int breakCount = 0;                         //initializes the breakCount variable as an integer
unsigned long startTime = 0;                //initializes the startTime variable as an unsigned long
unsigned long endTime = 0;                  //initializes the endTime variable as an unsigned long
double rpm = 0;                             //initialzies the rpm variable as a double
int numberofBlades = 4;                     //initializes the numberofBlades variable as an integer of value 4
//Power Output
float nout, torqueOut, torqueIn, effM, pin, powerIn, powerOut; //Creates 7 variables of type float
//Efficiency
float eff;                                  //Creates a variable of type float named eff


void setup() {
  //LCD
  lcd.begin(16,2);                          //Tells the LCD screen to use 2 rows of 16 spaces for the screen
  lcd.display();                            //Tells the LCD to display

  pinMode(buttonPin, INPUT);                //Tells the computer which Arduino pin to interact with when using the button
  attachInterrupt(digitalPinToInterrupt(buttonPin), changeDisplaySetting, FALLING); //makes it to where the screen can change displays using a function defined later
  //Tachometer
  Serial.begin(9600);                       //Tells the program when to begin using the serial monitor
  pinMode( interruptPin, INPUT);            //Tells the computer which Arduino pin to use for the tachometer
  attachInterrupt (digitalPinToInterrupt (interruptPin), broken, FALLING); //makes it to where the screen can change what it is displaying


}

void loop() {
    //Tachometer
    if(breakCount >= 30) {                   //if statement stating that if the number of times the light was broken is greater than or equal to 30 the program should continue into the if statement
    endTime = millis();                      //Puts the endTime in milliseconds

    float rev;                               //intializes rev variable as a float
    float revTime;                           //initializes revTime variable as a float
    float totaltime;                         //initializes totaltime variable as a float
    rev = (float)breakCount / 4.0;           //casts breakCount as a float and calculates revolutions
    totaltime = ((float)endTime-(float)startTime)/60000.0;//calculates the total time while casting endTime and startTime as floats and converting to minutes from milliseconds
    rpm = (rev/(totaltime));                 //calculates the rpm
    breakCount = 0;                          //resets breakCount to 0

  //LCD
  delay(1000);                               //Delays the display showing a value so as to allow the display a chance to actually see the value it is meant to show
  displayLCD();                              //calls the displayLCD function

  //Power Output
  nout = 8*rpm;                              //calculates the nout variable by multiplying the ratio of the gears by the rpm
  torqueOut = (-.025*nout + 320.04);         //calculates the torque using a function found using the torque curves and converting to the correct units
  torqueIn = (torqueOut * nout)/ rpm;        //calculates the torque into the system
  if (torqueOut <= 48) {                     //if statement telling the program which function to use depending on the torque output
    effM = ((-.002)*(torqueOut*torqueOut))-(.4*torqueOut)+49.4; //calculations for the efficiency of the motor
  }
  if (torqueOut > 48) {                      //second function available for use
    effM = ((-.0004)*(torqueOut*torqueOut))+(.2*torqueOut)+.0008; //calculations for efficiency of the motor if the torque is greater than 48
  }
  powerIn = ((-.0004)*(torqueOut*torqueOut))+(.14*torqueOut)+.0007; //calculations for power in for the motor
  pin = ((torqueIn * .000098 * rpm)/9500)*1000;//calculations for the power in of the system
  powerOut = (effM/100 * powerIn)-1;              // calculations for power out

  //Efficiency
  eff = (powerOut/powerIn)*100;               //calculations for efficiency and conversion to a percentage
  }
}

void displayLCD()                             //function for displaying values on the LCD
{
  switch(displaySetting)                      //switch statement which creates a menu type output
  {
    case 1:                                   //first scenario in case statement
      lcd.clear();                            //clears the screen
      lcd.print(rpm);                         //prints the value of rpm
      lcd.print(" rpm");                      //prints the term "rpm" as units
      break;                                  //makes it so that the function doesn't continue past this point
    case 2:                                   //second scenario in case statement
      lcd.clear();                            //clears the screen
      lcd.print(powerOut);                    //prints the value of pout to the LCD
      lcd.print(" W");                        //prints the letter "W" to the LCD as units
      break;                                  //makes it so that the function doesn't continue past this point
    case 3:                                   //third scenario in case statement
      lcd.clear();                            //clears the screen
      lcd.print(eff);                         //prints the value of eff to the LCD
      lcd.print(" %");                        //prints the symbol "%" to the LCD
      break;                                  //makes it so that the function doesn't continue past this point
    default:                                  //if none of the other cases work this one is used
      lcd.clear();                            //clears the screen
      lcd.print("Uknown Setting!");           //prints the sentence "Uknown Setting!"
  }
}

void changeDisplaySetting()                   //function for changing the display setting
{
  if(lastDisplaySwitch + DISPLAY_DELAY < millis()) { //if statement setting up the condition
    lastDisplaySwitch = millis();             //sets lastDiplaySwitch to be in milliseconds
    Serial.println("Switching display");      //prints "Switching display" to the Serial monitor
    displaySetting++;                         //increments variable displaySetting
    if(displaySetting > MAX_DISPLAYS) {       //nested if statement
      displaySetting = 1;                     //initializes displaySetting to 1
    }
    displayLCD();                             //calls function displayLCD
  }
}

void broken() {                               //function for recording the break count
  if (breakCount == 0) {                      //if statement making sure no negative values are recorded
    startTime = millis();                     //sets the start time to be in milliseconds
  }
  breakCount++;                               //increments breakCount
}
