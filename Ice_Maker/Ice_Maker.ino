#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

/*
* TODO: Store current state in eeprom
*
*
*/


// IO pins
//AC
const char Compresser = 22;
const char CompresserReverseValve = 24;
const char IceDispenseForword = 28;
const char IceDispenseBackward = 30;
const char IceMotor = 26;

//DC
const char IceWaterPump = 27;
const char WaterDispensePump = 29;
const char Fan = 23;
const char WaterSolinoid = 25;

//Sensors
const char EndstopA = A0;
const char EndstopB = A1;
const char IceFullSensor = A2;
const char IceButton = A3;
const char WaterButton = A4;
const char PowerButton = A5;
const char WaterLowSensor = A6;
const char LargerIce = A7;
const char SmallerIce = A8;

//Other Outputs
const int backlightLED = 14;



//vars
bool compressorRunning  = 0;
bool compressorReverse = 0;
bool dispensingWater = 0;
bool dispensingIce = 0;
int iceCycleStage= 1;
/*
 * 0 = idle
 * 1 = cycle tray
 * 2 = fill with water
 * 3 = make ice
 * 4 = reverse
 * 5 = cycle tray
 * 6 = reset
 */
int iceSize = 1;
// 0 = small, 1 = large
bool waterLow = 1;
bool waterFilling = 0;
bool powerOn = 0;
bool iceFull = 0;
int motorDirection = 3; 
//1 = forword, 2 = reverse, 0 = unknown
bool motorOn = 0;
bool backlightOn = 0;

bool endstopApressed = 0;
bool endstopBpressed = 0;


//display vars
int displayProgressPercentage = 100;
String statusMessage = "               "; //init with 16 values to skip memory issues


//timer length (MS)
const long waterFilltime = 10000;
const long LowIceTime = 600000;
const long HighIceTimne = 750000;
const long DisplayUpdateDelayTime = 500;
const long reverseTime = 10000;
long iceTimer = HighIceTimne;

//timer state  1 = finished, 0 = disabled, 2 = Running, 3 = Start
long Ice = 0;
long Fill = 0;
long DisplayUpdateDelay = 1;
long reverse = 0;

//timer start times
unsigned long IceStartTimeMS;
unsigned long FillStartTimeMS;
unsigned long DisplayUpdateStartTimeMS;
unsigned long reverseTimeStartMS;
unsigned long currentMillis;


// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
byte ProgressBarCustomChar[] = {B00000,B00000,B11111,B11111,B11111,B11111,B00000,B00000};
byte SeperatorcustomChar[] = {B01110,B01110,B01110,B01110,B01110,B01110,B01110,B01110};

void setup(){ 
  lcd.begin(); //init the lcd
  lcd.createChar(0, ProgressBarCustomChar);  
  lcd.createChar(1,SeperatorcustomChar);
  int i = 0;
  while (i < 54){
    pinMode(i,OUTPUT);
    digitalWrite(i, HIGH);
    i = i + 1;
  }
  pinMode(EndstopA,INPUT_PULLUP);
  pinMode(EndstopB,INPUT_PULLUP);
  pinMode(WaterLowSensor,INPUT_PULLUP);
  Serial.begin(9600);
}

void loop(){

  currentMillis = millis();
  updateProgressBar();
  updateDisplay();
  motorController();
  startTimers();
  decrementTimer();
  updateSensors();
  cycle();
  setRelayState();
}

void motorController(){
  if (motorDirection == 1 && motorOn == 1 && endstopApressed){
    motorOn = 0;
  }
  if (motorDirection == 2 && motorOn == 1 && endstopBpressed){
    motorOn = 0;
  }
    if (endstopApressed){
      motorDirection = 2;
    }
    if (endstopBpressed){
      motorDirection = 1;    
    }
}
void startTimers(){
  if (Ice == 3){
    Ice = 2;
    IceStartTimeMS = currentMillis;
  }
  if (Fill == 3){
    Fill = 2;
    FillStartTimeMS = currentMillis;
  }
  if (DisplayUpdateDelay == 3){
    DisplayUpdateDelay = 2;
    DisplayUpdateStartTimeMS = currentMillis;
  }
  if (reverse == 3){
    reverse = 2;
    reverseTimeStartMS = currentMillis;
  }
  
}
void updateDisplay(){
  if (DisplayUpdateDelay == 1){
    DisplayUpdateDelay = 3;
    //lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(statusMessage);

    //remove existing characters
    int statusLength = statusMessage.length();
    if (statusLength < 16){
      int spaces = 16 - statusLength;
      while (spaces != 0){
        lcd.write(20);//write blank space
        spaces = spaces - 1;
      }
    }
    //Ice Size
    lcd.setCursor(10,1);
    lcd.write(1); //prints a filled character
    if (iceSize == 0){
      lcd.print("Large");
    }
    else lcd.print ("Small");



    //progress bar
    if (iceCycleStage > 1){
      lcd.setCursor(0,1);
      int progress = (displayProgressPercentage / 20);
      while (progress > 0){
        lcd.write(0);
        progress = progress - 1;
      }
      lcd.setCursor(6,1);
      lcd.print(displayProgressPercentage);
      lcd.print("%");
      
    }
  }
}
void updateProgressBar(){
  displayProgressPercentage = ((((float)currentMillis - (float)IceStartTimeMS) / (float)iceTimer) * 100) + 1;
  if (displayProgressPercentage > 100){
    displayProgressPercentage = 100;
  }
  if (iceCycleStage != 3){
    displayProgressPercentage = 0;
  }
}

void cycle(){
  if (iceCycleStage == 0){
    statusMessage = "Idle";
  }
  if (iceCycleStage == 1){ //moving tray
    statusMessage = "Moving Tray";
    if (endstopApressed && !motorOn){
      iceCycleStage = iceCycleStage + 1; 
    }
    else {
      motorOn = 1;
    }

  }
  if (iceCycleStage == 2){ //filling with water
    statusMessage = "Filling with water";
    if (Fill == 0){
      Fill = 3;
      waterFilling = 1;
    }
    if (Fill == 1){
      waterFilling = 0;
      Fill = 0;
      iceCycleStage = iceCycleStage + 1;
    }
  }
  if (iceCycleStage == 3){ //making ice
    if (Ice == 0){
      Ice = 3;
      statusMessage = "Making Ice";
    }
    compressorRunning = 1;
    if (Ice == 1){
      iceCycleStage = iceCycleStage + 1;
      Ice = 0;
    }
  }
  if (iceCycleStage == 4){
    statusMessage = "Moving Tray";
    if (endstopBpressed && !motorOn){
      iceCycleStage = iceCycleStage + 1; 
    }
    else {
      motorOn = 1;
    }
  }
  if (iceCycleStage == 5){ 
    statusMessage = "Dropping Ice";
    
    if (reverse == 0){
      reverse = 3;
      compressorReverse = 1;
    }
    if (reverse == 1){
      reverse = 0;
      compressorReverse = 0;
      compressorRunning = 0;
      iceCycleStage = iceCycleStage + 1;     
    }
  }
  if (iceCycleStage == 6){ //push ice
     statusMessage = "Moving Tray";
    if (endstopApressed && !motorOn){
      iceCycleStage = iceCycleStage + 1; 
    }
    else {
      motorOn = 1;
    }
  }
  if (iceCycleStage == 7){
    statusMessage = "Done";
  }
}

void decrementTimer(){
  if (Ice == 2 && currentMillis - IceStartTimeMS >= iceTimer){
    Ice = 1;
  }
  if (Fill == 2 && currentMillis - FillStartTimeMS >= waterFilltime){
    Fill = 1;
  }
  if (DisplayUpdateDelay == 2 && currentMillis - DisplayUpdateStartTimeMS >= DisplayUpdateDelayTime){
    DisplayUpdateDelay = 1;
  }
  if (reverse == 2 && currentMillis - reverseTimeStartMS >= reverseTime){
    reverse = 1;
  }
  
}
void setRelayState(){
    digitalWrite(Compresser, !compressorRunning);
    digitalWrite(Fan, !compressorRunning);
    digitalWrite(WaterDispensePump, !dispensingWater);
    digitalWrite(IceDispenseForword, !dispensingIce);
    //digitalWrite(WaterSolinoid, !waterLow);
    digitalWrite(backlightLED, !backlightOn);
    digitalWrite(CompresserReverseValve, !compressorReverse);
    digitalWrite(IceWaterPump, !waterFilling);
    digitalWrite(IceMotor,!motorOn);
}

void updateSensors(){
  //iceFull = !digitalRead(IceFullSensor);
  waterLow = !digitalRead(WaterLowSensor);
  //dispensingIce = !digitalRead(IceButton);
  if (!digitalRead(LargerIce)){
    iceSize = 1;
  }
  if (!digitalRead(SmallerIce)){
    iceSize = 0;
  }
  //dispensingWater = !digitalRead(WaterButton);
  endstopApressed = !digitalRead(EndstopA);
  endstopBpressed = !digitalRead(EndstopB);
}
