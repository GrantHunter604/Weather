/*
 * Name: Grant Hunter
 * Class: CPS 210 Tues-Thurs 11-12:15
 * Date: 2/28/17
 * Homework: Raspberry Pi Project
 * Description: Read temperture sensor and write to correspoding files
 */

//inclusions
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//define for different devices
#define TEMP_PIN 7
#define BUTPIN 3

//initialize temperature array, average temperatures, and counters
int temp[5] = {0, 0, 0, 0, 0};
float intAvgTemp = 0;
float fAvgTemp = 0;
int tempCounter = 0;
int once = 0;
int buttonCntr = 0;

void readTemp(){
	//allocate and initalize variables
	int currentState = HIGH;
	int counter = 0;
	int innerCounter = 0, i;
	double t, h, tc;

	temp[0] = temp[1] = temp[2] = temp[3] = temp[4] = 0;

	//setup the pins for reading data and accessing button
	pinMode(TEMP_PIN, OUTPUT);
	digitalWrite(TEMP_PIN, HIGH);
	delay(10);
	digitalWrite(TEMP_PIN, LOW);
	delay(18);
	digitalWrite(TEMP_PIN, HIGH);
	delayMicroseconds(40);
	pinMode(TEMP_PIN, INPUT);
	pinMode(BUTPIN, INPUT);
	pullUpDnControl(BUTPIN, PUD_UP);

	//if the button is pressed
	if(!digitalRead(BUTPIN)){
		if(buttonCntr==2){
			buttonCntr = 0;
		}
		else{
			buttonCntr++;
		}
	}

	//get data from the sensor a maximum of 85 times
	for(i = 0; i < 85; i++){
		counter = 0;
		while(digitalRead(TEMP_PIN) == currentState){
			counter++;
			delayMicroseconds(1);
			if(counter == 255){
				break;
			}
		}
		currentState = digitalRead(TEMP_PIN);

		if(counter == 255){
			break;
		}

		//do not read from the first 3 iterations
		if((i >= 4) && (i%2==0)){
			temp[innerCounter/8] <<= 1;
			if(counter > 16){
				temp[innerCounter/8] |= 1;
			}
			innerCounter++;
		}
	}

	//only write to files if data is good
	if((innerCounter >= 40) && (temp[4] == ((temp[0]+temp[1]+temp[2]+temp[3]) & 0xFF)) && ((((float)(temp[2])*256.0 + (float)temp[3])/10.0) > 10 && (((float)(temp[2])*256.0 + (float)temp[3])/10.0) < 37)){
		//convert to Fahrenheit and Celsius
		h = (double)temp[0] * 256.0 + (double)temp[1];
		h/=10.0;
		tc = temp[2]*256.0 + temp[3];
		tc/=10.0;
		t = tc*(9.0/5.0)+32.0;
		tempCounter++;

		//write to weatherdisplay.py
		FILE *f2 = fopen("/home/pi/workspace/Weather/WeatherDisplay.txt", "w");
		if(buttonCntr==0){
			fprintf(f2, "%.1lfF", t);
		}
		else if(buttonCntr==1){
			fprintf(f2, "%.1lfC", tc);
		}
		else{
			fprintf(f2, "%.1lf%%", h);
		}
		fclose(f2);

		intAvgTemp += t;
		//1 min passes
		if(tempCounter == 120){
			fAvgTemp = intAvgTemp / (float)(tempCounter);
			//write to weatheralert.py if temp is above or below extremes
			//only writes one time until the average is between the two extremes
			if((fAvgTemp >= 74.0 || fAvgTemp <= 68.0) && !once){
				FILE *f = fopen("/home/pi/workspace/Weather/WeatherAlert.txt", "w");
				fprintf(f, "%lf", fAvgTemp);
				fclose(f);
				once = 1;
			}
			//if between two extremes and has written to file once
			else if((fAvgTemp < 74.0 && fAvgTemp > 68.0) && once){
				once = 0;
			}
			tempCounter = 0;
			fAvgTemp = 0.0;
			intAvgTemp = 0.0;
		}
	}
}

int main(void){
	//setup wiringPi
	wiringPiSetup();
	while(1){
		readTemp();
		//get data every 0.5 seconds
		delay(500);
	}
	return 0;
}
