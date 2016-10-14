//APDS-9960 stuff
#include <Wire.h>
#include <SparkFun_APDS9960.h>

// Pins
#define APDS9960_INT    2 // Needs to be an interrupt pin

// Constants
SparkFun_APDS9960 apds = SparkFun_APDS9960();
uint16_t ambient_light = 0;
uint16_t red_light = 0;
uint16_t green_light = 0;
uint16_t blue_light = 0;
bool light_state = false;
bool hand_swiped = false;
int relay_pin = 8;
int isr_flag = 0;
int threshhold = 300;

void setup() {

  // Set interrupt pin as input
  pinMode(APDS9960_INT, INPUT);
  pinMode(relay_pin, OUTPUT);

  // Initialize Serial port
  Serial.begin(9600);
  Serial.println();
  Serial.println(F("--------------------------------"));
  Serial.println(F("Arduino Smart Lamp"));
  Serial.println(F("--------------------------------"));
  
  // Initialize interrupt service routine
  attachInterrupt(0, interruptRoutine, FALLING);

  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  }
  
  // Start running the APDS-9960 gesture sensor engine
  if ( apds.enableGestureSensor(true) ) {
    Serial.println(F("Sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during gesture sensor init!"));
  }
  if ( apds.enableLightSensor(false) ) {
    Serial.println(F("Light sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during light sensor init!"));
  }
  digitalWrite(relay_pin,HIGH);
  Serial.println("threshhold is: ");
  Serial.println(threshhold);
}

void loop(){
	if( isr_flag == 1 ) {
    detachInterrupt(0);
    handleGesture();
    isr_flag = 0;
    attachInterrupt(0, interruptRoutine, FALLING);
  }
	if(hand_swiped==false){
  int light = read_light();
  if((light < (threshhold -100)) && (hand_swiped == false)){
    digitalWrite(relay_pin,LOW);
    light_state = true;
  }
  else if((light >= (threshhold - 50)) && (hand_swiped == false)){
    digitalWrite(relay_pin,HIGH);
    light_state = false;
  }
  delay(1000);
	}
}

void interruptRoutine() {
  isr_flag = 1;
}

void handleGesture() {
    if ( apds.isGestureAvailable() ) {
    switch ( apds.readGesture() ) {
		case DIR_RIGHT:
        Serial.println("RIGHT");
		if(light_state == true){
			hand_swiped = true;
			digitalWrite(relay_pin,HIGH);
			light_state = false;
      Serial.println("light state false");
		}
		else if(light_state==false){
			hand_swiped=true;
			digitalWrite(relay_pin,LOW);
			light_state = true;
     Serial.println("light state true");
		}
		break;
		case DIR_LEFT:
			Serial.println(F("LEFT, back to auto mode."));
			hand_swiped = false;
			break;
	}
	}
}
int read_light(){
  if (  !apds.readAmbientLight(ambient_light) ||
        !apds.readRedLight(red_light) ||
        !apds.readGreenLight(green_light) ||
        !apds.readBlueLight(blue_light) ) {
    Serial.println("Error reading light values");
  } else {
    return ambient_light;
    Serial.println("Ambient: ");
    Serial.println(ambient_light);
  }
}









