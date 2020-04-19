#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Relay pins
//AC
const char Compresser = 2;
const char CompresserReverse = 3;
const char IceDispenseForword = 4;
const char IceDispenseBackward = 5;
const char IceMotor = '6';

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
const int backlightLED = '14';

//vars
bool compressorRunning  = 0;
bool dispensingWater = 0;
bool dispensingIce = 0;
int iceCycleStage= 0;
/*
 * 0 = idle
 * 1 = cycle tray
 * 2 = fill with water
 * 3 = make ice
 * 4 = reverse
 * 5 = cycle tray
 */
int iceSize = 0;
// 0 = small, 1 = large
bool waterLow = 0;
bool powerOn = 0;
bool fanOn = 0;
bool iceFull = 0;
int motorDirection = 3;
//1 = forword, 2 = reverse, 0 = stopped, 3 = unknown
bool backlightOn = 0;




//timer length
const int waterFilltime = 1000;
const int LowIceTime = 10000;
const int HighIceTimne = 20000;

//timers
int Ice = 0;
int Fill = 0;


// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{ 
  lcd.begin();
  lcd.print("Starting");
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
  setRelayState();
  updateSensors();
}
void setRelayState(){
  if (compressorRunning){
    digitalWrite(Compresser, LOW);
  }
  else{
    digitalWrite(Compresser, HIGH);
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
  if (fanOn){
    digitalWrite(Fan, LOW);
  }
  else{
    digitalWrite(Fan, HIGH);
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
}
