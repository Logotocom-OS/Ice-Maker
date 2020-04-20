#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// IO pins
//AC
const char Compresser = 2;
const char CompresserReverseValve = 3;
const char IceDispenseForword = 4;
const char IceDispenseBackward = 5;
const char IceMotor = 6;

//DC
const char IceWaterPump = 10;
const char WaterDispensePump = 11;
const char Fan = 12;
const char WaterSolinoid = 13;

//Sensors
const char EndstopA = 'A0';
const char EndstopB = 'A1';
const char IceFullSensor = 'A2';
const char IceButton = 'A3';
const char WaterButton = 'A4';
const char PowerButton = 'A5';
const char WaterLowSensor = 'A6';
const char LargerIce = 'A7';
const char SmallerIce = 'A8';

//Other Outputs
const int backlightLED = 14;

//vars
bool compressorRunning  = 0;
bool compressorReverse = 0;
bool dispensingWater = 0;
bool dispensingIce = 0;
int iceCycleStage= 3;
/*
 * 0 = idle
 * 1 = cycle tray
 * 2 = fill with water
 * 3 = make ice
 * 4 = reverse
 * 5 = cycle tray
 * 6 = reset
 */
int iceSize = 0;
// 0 = small, 1 = large
bool waterLow = 1;
bool powerOn = 0;
bool iceFull = 0;
int motorDirection = 3; 
//1 = forword, 2 = reverse, 0 = stopped, 3 = unknown
bool backlightOn = 0;

//display vars
int displayProgressPercentage = 0;
String statusMessage = "################"; //init with 16 values to skip memory issues


//timer length
const long waterFilltime = 1000;
const long LowIceTime = 10000;
const long HighIceTimne = 20000;
const long DisplayUpdateDelayTime = 500;

long iceTimer = HighIceTimne;

//timer state  1 = finished, 0 = disabled, 2 = Running, 3 = Start
long Ice = 0;
long Fill = 0;
long DisplayUpdateDelay = 0;

unsigned long IceStartTimeMS;
unsigned long FillStartTimeMS;
unsigned long DisplayUpdateStartTimeMS;
unsigned long currentMillis;


// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{ 
  lcd.begin(); //init the lcd
  lcd.print("Starting#########");
  // Initialize the serial port at a speed of 9600 baud
  Serial.begin(9600);
  
  int i = 0;
  while (i < 14){
    pinMode(i,OUTPUT);
    digitalWrite(i, HIGH);
    i = i + 1;
  }
}

void loop(){
  currentMillis = millis();
  decrementTimer();
  //updateSensors();
  cycle();
  setRelayState();
}
void cycle(){
  if (iceCycleStage == 1){
    
  }
  if (iceCycleStage == 2){ //filling with water
    if (Fill == 0){
      Fill = 3;
    }
    if (Fill == 1){
     //water pump off
     Fill = 0;
     iceCycleStage = iceCycleStage + 1;
    }
  }
  if (iceCycleStage == 3){ //making ice
    if (Ice == 0){
      Ice = 3;
    }
    compressorReverse = 0;
    compressorRunning = 1;
    if (Ice == 1){
      iceCycleStage = iceCycleStage + 1;
      Ice = 0;
    }
  }
  if (iceCycleStage == 4){
    compressorReverse = 1;
  }
  if (iceCycleStage == 5){ 
    
  }
  if (iceCycleStage == 6){ //reset
    
  }
}

void decrementTimer(){
  if (Ice == 2 && IceStartTimeMS - currentMillis >= iceTimer){
    Ice = 1;
  }
  if (Fill == 2 && FillStartTimeMS - currentMillis >= waterFilltime){
    Fill = 1;
  }
  if (DisplayUpdateDelay == 2 && DisplayUpdateStartTimeMS - currentMillis >= DisplayUpdateDelayTime){
    DisplayUpdateDelay = 1;
  }
}
void setRelayState(){
  if (compressorRunning){
    digitalWrite(Compresser, LOW);
    digitalWrite(Fan, LOW);
  }
  else{
    digitalWrite(Compresser, HIGH);
    digitalWrite(Fan, HIGH);
  }
  if (dispensingWater){
    digitalWrite(WaterDispensePump, LOW);
  }
  else{
    digitalWrite(WaterDispensePump, HIGH);
  }
  if (dispensingIce){
    digitalWrite(IceDispenseForword, LOW);
  }
  else{
    digitalWrite(IceDispenseForword, HIGH);
  }
  if (waterLow){
    digitalWrite(WaterSolinoid, LOW);
  }
  else{
    digitalWrite(WaterSolinoid, HIGH);
  }
  if (backlightOn){
    digitalWrite(backlightLED, LOW);
  }
  else{
    digitalWrite(backlightLED,HIGH);
  }
  if (compressorReverse){
    digitalWrite(CompresserReverseValve, LOW);
  }
  else{
    digitalWrite(CompresserReverseValve,HIGH);
  }
}

void updateSensors(){
  if (analogRead(IceFullSensor == 1000)){
    iceFull = 1;
  }
  else iceFull = 0;
  if (analogRead(WaterLowSensor) == 1000){
    waterLow = 1;
  }
  else waterLow = 0;
  if (analogRead(IceButton) == 1000){
    dispensingIce = 1;
  }
  else dispensingIce = 0;
  if (analogRead(LargerIce) == 1000){
    iceSize = 1;
  }
    if (analogRead(SmallerIce) == 1000){
    iceSize = 0;
  }
  if (analogRead(WaterButton) == 1000){
    dispensingWater = 1;
  }
  else dispensingWater = 0;
}
