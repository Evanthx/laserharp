#include <GinSing.h>
#include <GinSingDefs.h>
#include <GinSingMaster.h>
#include <GinSingPoly.h>
#include <GinSingPreset.h>
#include <GinSingSerial.h>
#include <GinSingSerialMacros.h>
#include <GinSingSynth.h>
#include <GinSingVoice.h>

#include <Wire.h>
#include <GinSing.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

#include "HarpNoteChoice.h"
#include "HarpNoteDetection.h"

//When debugging I wanted more information. So ... set this boolean to true
//to get more stuff printed to the console. When it's true the console dumps LOTS of
//great stuff - but the code CRAWLS and the laser harp isn't great.
const boolean debug = false;

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Connect a stepper motor with 200 steps per revolution (1.8 degree)
// to motor port #2 (M3 and M4)
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2);

GinSing  GS;
#define rcvPin  4  // this is the pin used for receiving    ( can be either 4 or 12 )
#define sndPin  3  // this is the pin used for transmitting ( can be either 3 or 11 ) 
#define ovfPin  2  // this is the pin used for overflow control ( can be either 2 or 10 )
GinSingPoly * poly;

#define GINSING0 0                                      
#define GINSING1 1   

const int lightSensorPin = 0;

const int stepSize = 1;
const int delayBetweenSteps = 15;
const int numberNotes = 7;

GSNote ginsingNotes[numberNotes + 1];

const int buttonApin = 9;
const int buttonBpin = 8;

HarpNoteChoice harpNoteChoice;
HarpNoteDetection harpNoteDetector;

boolean pluckedNotes[numberNotes];
int reflectedLightValues[numberNotes];

void setup()
{
	harpNoteDetector.setNumNotes(numberNotes);
	Serial.begin(9600);

	//For the light sensor
	analogReference(EXTERNAL);

	pinMode(buttonApin, INPUT_PULLUP);
	pinMode(buttonBpin, INPUT_PULLUP);

	AFMS.begin();  // create with the default frequency 1.6KHz
	myMotor->setSpeed(250);

	//Get some initial values for each light string
	for (int i = 0; i < numberNotes; i++) {
		reflectedLightValues[i] = analogRead(lightSensorPin);
		pluckedNotes[i] = false;
	}

	setupGinSing();
}

//This is the code required to get GinSing ready to go.
void setupGinSing() {
	GS.begin(rcvPin, sndPin, ovfPin);               // start the device (required) and enter preset mode
	//For preset mode - change this when going to poly mode
	poly = GS.getPoly();
	poly->begin();                                    // enter presetmode

	poly->setWaveform(GINSING0, SAWTOOTH);
	poly->setWaveform(GINSING1, SAWTOOTH);

	setupGinsingNotes();
}

//These are the notes I want to play with GinSing.
void setupGinsingNotes() {
	ginsingNotes[0] = C_4;
	ginsingNotes[1] = D_4;
	ginsingNotes[2] = E_4;
	ginsingNotes[3] = F_4;
	ginsingNotes[4] = G_4;
	ginsingNotes[5] = A_4;
	ginsingNotes[6] = B_4;
}

//This code is used to take the sonar reading and convert
//that into something to change the GinSing note being played.
int findMultiplier(int height) {
	/*
	Over 170 = nothing
	140 = high           80 - 5
	125 = normal  64 - 4
	110 = low            48 - 3
	92 = very low       32 - 2
	74 = lower          16 - 1
	62 bottom out
	*/
	if (height > 135) return 5;
	if (height > 119) return 4;
	if (height > 100) return 3;
	if (height > 80) return 2;
	//My speaker gets weird at the low values. So I commented that one out.
//	if (height > 65) return 1;
	return 0;
}

//Read the sonar unit and figure out if the
//notes should move up or down 
void checkSonar() {
	int height = analogRead(1);

	if (debug) {
		Serial.print("Height:");
		Serial.println(height);
	  }

	if (height > 170) {
		return;
	}
	int multiplier = findMultiplier(height);
	int baseValue = 16 * multiplier;

	ginsingNotes[0] = (GSNote)baseValue;
	ginsingNotes[1] = (GSNote)(baseValue + 1);
	ginsingNotes[2] = (GSNote)(baseValue + 2);
	ginsingNotes[3] = (GSNote)(baseValue + 3);
	ginsingNotes[4] = (GSNote)(baseValue + 4);
	ginsingNotes[5] = (GSNote)(baseValue + 5);
	ginsingNotes[6] = (GSNote)(baseValue + 6);
}

//These two are the notes that Ginsing is currently playing.
int ginsingNote0 = -1;
int ginsingNote1 = -1;

//FirstNote and SecondNote are the notes we want to be playing.
void playNote(int firstNote, int secondNote) {

	if (debug) {
		Serial.print("Playing notes ");
		Serial.print(firstNote);
		Serial.print(" and ");
		Serial.print(secondNote);
	}

	//Pick the notes to be played and which channel to play them on
	harpNoteChoice.pickNotes(ginsingNote0, ginsingNote1, firstNote, secondNote);

	if (ginsingNote0 == -1) {
		poly->release(GINSING0);
	}
	else {
		poly->setNote(GINSING0, ginsingNotes[ginsingNote0]);
		poly->trigger(GINSING0);
	}

	if (ginsingNote1 == -1) {
		poly->release(GINSING1);
	}
	else {
		poly->setNote(GINSING1, ginsingNotes[ginsingNote1]);
		poly->trigger(GINSING1);
	}

}

//Move the motor one step. Sleep, then take a light reading. The sleep
//gives the sensor time to actually report the new reading.
int stepTheMotorAndGetLightReading(int directionToStep) {
	myMotor->step(stepSize, directionToStep, DOUBLE);
	delay(delayBetweenSteps);
	int currentLightReading = analogRead(lightSensorPin);
	return currentLightReading;
}

//Is debug on? If so print some data.
void checkNotes(int reflectedLightValues[], boolean pluckedNotes[]) {
	if (debug) {
		for (int i = 0; i < numberNotes; i++) {
			Serial.print(i);
			Serial.print("=");
			Serial.print(reflectedLightValues[i]);
			Serial.print(" - ");
		}
	}

	//So ... are any light readings different from any others?
	//To find the anomaly, see how different each string is to every other string.
	//One or two strings should stand out. Those are the plucked strings.
	harpNoteDetector.checkNotes(reflectedLightValues, pluckedNotes);

	//So what strings are plucked? Note that if more than two are plucked,
	//that counts as an error ... if that's the case, just keep playing the current notes
	//and hope the player gets his act straight.
	int firstNote = -1;
	int secondNote = -1;
	if (harpNoteDetector.getNotes(firstNote, secondNote, pluckedNotes)) {
		if (debug) {  
			if (firstNote >= 0) {
				Serial.print("==============PLUCKED 1: ");
				Serial.println(firstNote);
			}
			if (secondNote >= 0) {
				Serial.print("==============PLUCKED 2: ");
				Serial.println(secondNote);
			}
		}
		playNote(firstNote, secondNote);
	}
	else if (debug) {
		Serial.println("getNotes returned false - more than three notes were counted as plucked.");
	}
	return;
}

//Is a button pressed? If so move the motor a bit. This lets the user adjust the laser fan so it's pointing upwards properly.
void checkButtons() {
	if (digitalRead(buttonApin) == LOW) {
		myMotor->step(stepSize, BACKWARD, DOUBLE);
	}

	if (digitalRead(buttonBpin) == LOW) {
		myMotor->step(stepSize, FORWARD, DOUBLE);
	}
}


void loop()
{

	//There must be at least a handful notes for the code below to work right.
	if (numberNotes < 5) {
		return;
	}

	//Run the laser forward, read all values, and see what is there. Note that this pretty much uses one
	//less note than requested - but the START position counts as a spot. So moving it numberNotes makes that many
	//strings plus the start string.

	//It's already read the zero item. So read array items 1 through 7.
	for (int i = 1; i < numberNotes; i++) {
  		reflectedLightValues[i] = stepTheMotorAndGetLightReading(FORWARD);
		checkNotes(reflectedLightValues, pluckedNotes);
	}

	checkButtons();
        checkSonar();

	//It just read item 7. So going backwards, read items 6 through zero.
	for (int i = numberNotes - 2; i >= 0; i--) {
		reflectedLightValues[i] = stepTheMotorAndGetLightReading(BACKWARD);
		checkNotes(reflectedLightValues, pluckedNotes);
	}

	checkButtons();
        checkSonar();

}








