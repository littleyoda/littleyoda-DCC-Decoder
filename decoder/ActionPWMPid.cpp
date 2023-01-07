/*
 * ActionPWMPid.cpp
 *
 * 
 *  TODO Frequenz
 * 
 *  Created on: 09.12.2016
 *      Author: sven
 */

#include <Arduino.h>
#include "ActionPWMPid.h"
#include "Consts.h"
#include "Utils.h"
#include "Logger.h"

// temp variable for analog read test
int areadvalue;  // read  -> move inside ISR
volatile float avvalue; // average
float voltvalue; //scaled analog read value
const int oversampling=50 ;  //number of oversamplings

// variables for interrupt
bool PWMInterrupt = false;
hw_timer_t *My_timer = NULL;
volatile byte SpeedPWMcnt = 0;
byte SpeedPWMtarget = 0;  //128 is 50% PWM, 22 langsam 230 schnell
bool forward=true;
bool backward=false;
volatile int poti=0;

//variable for main task 
int LEDinternal = 2;
int SpeedPWMvalue = 0;
unsigned long last_output_time = 0;
unsigned long current_time ;
int OutputTimeConst = 300;  //3 Hz
int output_time;

//variable for speed controller
unsigned long last_speed_time = 0;
int speedControllerTimeConst = 5;
int delta_time;
double totalPWMerror = 0;
double lastPWMerror = 0;
float Kp=0.5;
float Ki=0.01;
float Kd=0;
int maxPWM = 254;
int minPWM = 0;
float kvolt = 10;

void SpeedPWM() {  //  ISR: SpeedPWMcnt is type byte !!!!
	double temppoti;

	SpeedPWMcnt++;  // count
	if (!SpeedPWMcnt){
		digitalWrite(33, LOW);  // Pin not configurable yet -> enable pin of H-Bridge for PWM
	}

	if (SpeedPWMcnt==(0xFF^SpeedPWMvalue)){
		areadvalue = 0;
		temppoti =0;
    	for (int i = 0;i<oversampling;i++){
			if      (forward)  areadvalue += analogRead(36);
			else if (backward) areadvalue += analogRead(39);
			{
				/* code */
			}
			
		}
//		avvalue=(areadvalue/oversampling); das gibt einen Co-Processor Error
//--------------------------------------------------------------
		for (int j = 0;j<16;j++){
			temppoti += analogRead(35);	// temporär !!!!!!!!!!
		}
		poti=temppoti;
// -------------------------------------------------------------
		digitalWrite(33, HIGH);  // Pin not configurable yet
	}
}

//Task1code: the Speed controller
void Task1code( void * pvParameters ) {
	delay(1000) ; // wait before write
    Serial.print("speedController runs on core ");
    Serial.println(xPortGetCoreID());

	delay(10000);  //some delay (until framework has completely stared)

  for(;;){

	if (!PWMInterrupt) {  // PWM Interrupt association and configuration
		PWMInterrupt = true;
		// configure interrupt
		
		My_timer = timerBegin(0, 80, true);  // each core has it's own timers :-)
		timerAttachInterrupt(My_timer, &SpeedPWM, true);
		timerAlarmWrite(My_timer, 80, true);  //1000000 for 1 second => 80 ~ 12,7 kHz (50HZ*255)
		timerAlarmEnable(My_timer);
	}

	current_time=millis();  //
//	current_time=esp_timer_get_time();

	int delta_time = current_time - last_speed_time; //delta time interval for speedController
	int output_time = current_time - last_output_time; //delta time for last debug output

	if (delta_time>speedControllerTimeConst){  //the mini PID controller
		double error = SpeedPWMtarget*10 - (areadvalue/oversampling);  // factor ~10 for better quality  
					// 2540-2000 (bei 16V) 1280-1800 (bei 16V,50%)
		totalPWMerror += error; //accumalates the error - integral term

		if (totalPWMerror >= maxPWM*kvolt) totalPWMerror = maxPWM*kvolt;
		else if (totalPWMerror <= minPWM*kvolt) totalPWMerror = minPWM*kvolt;

		double delta_error = error - lastPWMerror; //difference of error for derivative term

		SpeedPWMvalue = (((Kp*error) + ((Ki*speedControllerTimeConst)*totalPWMerror) + ((Kd/speedControllerTimeConst)*delta_error))/10); //PID control compute
		
		if (SpeedPWMvalue > maxPWM) SpeedPWMvalue = maxPWM;
		if (SpeedPWMvalue < minPWM) SpeedPWMvalue = minPWM;

		lastPWMerror = error;
		last_speed_time = current_time;

	}
	if (output_time>OutputTimeConst){
		last_output_time=current_time;
		// show activity of main task, no other use and will be deleted once PID controller is integrated
		digitalWrite(LEDinternal, !digitalRead(LEDinternal));

		//*
		Serial.print ("  SpeedPWMvalue:  ");
		Serial.print(SpeedPWMvalue);
		Serial.print ("  totalPWMerror: ");
		Serial.print(totalPWMerror);
		Serial.print ("  average  ");
    	Serial.print(areadvalue/oversampling);
		Serial.print ("  SpeedPWtarget:  ");
		Serial.print(SpeedPWMtarget);
		Serial.print ("  lastPWMerror  ");
		Serial.print(lastPWMerror);
		Serial.println();
		//
		
// --------------------------------------------------------------
	//	SpeedPWMvalue = (poti*16/3000);
	//	if (SpeedPWMvalue>254){SpeedPWMvalue=254;} 
		SpeedPWMtarget = (poti*16/3000);
		if (SpeedPWMtarget>254){SpeedPWMtarget=254;} 
// --------------------------------------------------------------    	
	}
	delay(2);  // give the IDLE task some time to feed the watchdog :-)
}

    	
}



	TaskHandle_t Task1;


ActionPWMPid::ActionPWMPid() {
	Logger::getInstance()->addToLog(LogLevel::INFO, "Starting PWM(PID) ...  ");
	  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "SpeedController",     /* name of task. */
                    2000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */
    delay(500); 

}

ActionPWMPid::~ActionPWMPid() {
}



void ActionPWMPid::setSettings(String key, String value) {
	Logger::log(LogLevel::TRACE, "SetSettings " + key + "/" + value);
	if (key.equals("sd")) {
		setSpeed(value.toInt());
		// int s = PWMRANGE * abs(value.toInt()) / 100;
		// setSpeedInProcent(s);
	} else if (key.equals("freq")) {
		// ignore
	}
}





void ActionPWMPid::setSpeed(long l) {
	SpeedPWMtarget = abs(l);
	forward= ( l >= 0);
	backward= ( l < 0);
}
