#include <Arduino.h>
#include <Adafruit_ADS1X15.h>
#include <Wire.h>
#include "Adafruit_AHTX0.h"
#include <Adafruit_BMP085.h>
#include <RTClib.h>
#include <MQUnifiedsensor.h>

#include "mqtt.h"


int16_t readMQ6() ;

#define trigPin 16
#define echoPin  17
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

#define Board  ("ESP-32") 
#define type "MQ-6" //MQ6
#define ADC_Bit_Resolution 16 // For arduino UNO/MEGA/NANO
#define RatioMQ6CleanAir 10 
#define Voltage_Resolution 5

MQUnifiedsensor MQ6(Board, type);

RTC_DS3231 rtc;
Adafruit_AHTX0 aht;
Adafruit_BMP085 bmp;
Adafruit_ADS1115 ads;
float factorEscala = 0.1875F;
void initMQ6(){
    MQ6.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ6.setA(2127.2); MQ6.setB(-2.526); // Configure the equation to to calculate CH4 concentration
  /*
    Exponential regression:
  GAS     | a      | b
  H2      | 88158  | -3.597
  LPG     | 1009.2 | -2.35
  CH4     | 2127.2 | -2.526
  CO      | 1000000000000000 | -13.5
  Alcohol | 50000000 | -6.017
  */
 MQ6.init();   
  /* 
    //If the RL value is different from 10K please assign your RL value with the following method:
    MQ6.setRL(10);
  */
  /*****************************  MQ CAlibration ********************************************/ 
  // Explanation: 
   // In this routine the sensor will measure the resistance of the sensor supposedly before being pre-heated
  // and on clean air (Calibration conditions), setting up R0 value.
  // We recomend executing this routine only on setup in laboratory conditions.
  // This routine does not need to be executed on each restart, you can load your R0 value from eeprom.
  // Acknowledgements: https://jayconsystems.com/blog/understanding-a-gas-sensor
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    float voltios = (readMQ6() * factorEscala)/1000.0;
    MQ6.externalADCUpdate(voltios);
    calcR0 += MQ6.calibrate(RatioMQ6CleanAir);
    Serial.print(".");
  }
  MQ6.setR0(calcR0/10);
  Serial.println("  done!.");
  
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply"); while(1);}
  /*****************************  MQ CAlibration ********************************************/ 
  MQ6.serialDebug(true);
}
void initAHT10(){
 Serial.println("AHT10 Sensor Test");

  if (!aht.begin()) {
    Serial.println("Couldn't find AHT10 sensor!");
    while (1);
  }
}


void initBme180(){
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
}
void initRTC(){
   if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}

void initADC(){
  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
  ads.setGain(GAIN_ONE);
}

void initHCSR04(){
   pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);
}





float readPressure() {
  return bmp.readPressure();
}

float readAltitude() {
  return bmp.readAltitude();
}

float readSealevelPressure() {
  return bmp.readSealevelPressure();
}

float readTemperature() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  return temp.temperature;
}

float readHumidity() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  return humidity.relative_humidity;
}

String readTime(){
  DateTime now = rtc.now();
    return now.timestamp();

}


int16_t readADC0() {
  return ads.readADC_SingleEnded(0);
}

int16_t readLightIntensity() {
  return map(ads.readADC_SingleEnded(1),1,30000,100,1);
}

int16_t readMQ6() {
  return ads.readADC_SingleEnded(2);
}

int16_t readADC3() {
  return ads.readADC_SingleEnded(3);
}

float convertToVolts(int16_t adcValue) {
  return ads.computeVolts(adcValue);
}

float readDistanceFromHCSR04(){
  long duration;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  float distanceCm = duration * SOUND_SPEED/2;

  return distanceCm;
} 

float readGasvalue(){
  short adc0 = ads.readADC_SingleEnded(0);
  float voltios = (adc0 * factorEscala)/1000.0;
  MQ6.externalADCUpdate(voltios); 
  MQ6.setA(1009.2); MQ6.setB(-2.35);

  return MQ6.readSensor();
}



void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);  // Wait for serial monitor to open
//   initAHT10();
//  initBme180();
//  initRTC();
//  initADC();
//  initHCSR04();
// initMQ6();
mqttInit();

}




void loop() {
mqttSendMessage();
mqttLoop();
}
