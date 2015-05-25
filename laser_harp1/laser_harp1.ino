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

boolean debug = false;

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

int lightSensorPin = 0;

const int stepSize = 1;
const int delayBetweenSteps = 15;  
const int numberNotes = 7;  
const int pluckDelta = 30;

int reflectedLightValues[numberNotes + 1];
boolean pluckedNotes[numberNotes + 1];
GSNote ginsingNotes[numberNotes + 1];

int buttonApin = 9;
int buttonBpin = 8;

void setup()
{
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
  }

  setupGinSing();
}

//This is the code required to get GinSing ready to go.
void setupGinSing() {
  GS.begin ( rcvPin , sndPin , ovfPin );               // start the device (required) and enter preset mode
  //For preset mode - change this when going to poly mode
  poly = GS.getPoly();
  poly->begin ();                                    // enter presetmode

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
  if (height > 65) return 1;
  return 0;
}

//Read the sonar unit and figure out if the
//notes should move up or down 
void checkSonar() {
  int height = analogRead(1);

  //if (debug) {
    Serial.print("Distance:");
    Serial.println(height);
 // }

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


int ginsingNote0 = -1;
int ginsingNote1 = -1;

void playNote(int firstNote, int secondNote) {

  if (debug) {
    Serial.print("Playing notes ");
    Serial.print(firstNote);
    Serial.print(" and ");
    Serial.print(secondNote);
  }

  if (firstNote == -1 && secondNote == -1) {
    poly->release ( GINSING0 ); 
    poly->release ( GINSING1 ); 
    return;
  }

  //Now - at least one note is getting played. Is it already being played?
  //If so leave it alone. Otherwise assign it to one of the Ginsing notes.
  int newNote0 = -1;
  int newNote1 = -1;

  //First note. Is it already being played? Keep it.
  if (firstNote == ginsingNote0) {
    newNote0 = firstNote;
    firstNote = -1;
  } else if (firstNote == ginsingNote1) {
    newNote1 =  firstNote;
    firstNote = -1;
  }

  //Is the second note being played already?
  if (secondNote != -1) {
    if (secondNote == ginsingNote0) {
      newNote0 =  secondNote;
      secondNote = -1;
    } 
    else if (secondNote == ginsingNote1) {
      newNote1 =  secondNote;
      secondNote = -1;
    }
  }

  //So are there any NEW notes? Stick them somewhere.
  if (firstNote != -1) {
    if (newNote0 == -1) {
      newNote0 = firstNote;
    } 
    else {
      newNote1 = firstNote;
    }
  }

  if (secondNote != -1) {
    if (newNote0 == -1) {
      newNote0 = secondNote;
    } 
    else {
      newNote1 = secondNote;
    }
  }

  //Awesome. Now play or release notes.
  ginsingNote0 = newNote0;
  ginsingNote1 = newNote1;

  if (ginsingNote0 == -1) {
    poly->release ( GINSING0 ); 
  } 
  else {
    poly->setNote    ( GINSING0 , ginsingNotes[ginsingNote0] );  
    poly->trigger ( GINSING0 );        
  }

  if (ginsingNote1 == -1) {
    poly->release ( GINSING1 ); 
  } 
  else {
    poly->setNote    ( GINSING1 , ginsingNotes[ginsingNote1] );  
    poly->trigger ( GINSING1 );        
  }

}




int stepTheMotorAndGetLightReading(int directionToStep) {
  myMotor->step(stepSize, directionToStep, DOUBLE); 
  delay(delayBetweenSteps);   
  int currentLightReading = analogRead(lightSensorPin);
  return currentLightReading;
}


void reportNotes() {
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

  for (int testString = 0; testString < numberNotes; testString++) {
    pluckedNotes[testString] = false;

    //We are looking at one string. Compare its reading to every other string. Is the difference over the threshhold?
    //Comparing to yourself just returns a zero, so allow it (that's simpler!)
    int counter = 0;
    for (int comparisonString = 0; comparisonString < numberNotes; comparisonString++) {
      int difference = abs(reflectedLightValues[testString] - reflectedLightValues[comparisonString]);
      if (difference > pluckDelta) {
        counter++;
      }
    }

    if (counter > 3) {
      pluckedNotes[testString] = true;
    }
  }

  //Now - are three or more plucked? If so - wipe out the results. 
  int numPlucked = 0;
  for (int testString = 0; testString < numberNotes; testString++) {
    if (pluckedNotes[testString]) {
      numPlucked++;
    }
  }

  if (numPlucked >= 3) {
    for (int testString = 0; testString < numberNotes; testString++) {
      pluckedNotes[testString] = false;
    }
  }
  
  if (debug) {
    Serial.println("");
    for (int i = 0; i < numberNotes; i++) {
      Serial.print(i);
      Serial.print("a=");
      Serial.print(pluckedNotes[i]);
      Serial.print(" - ");
    }
  }
  
  //Now who is above the average? 
  int firstNote = -1;
  int secondNote = -1;

  for (int i = 0; i < numberNotes; i++) {
    if (pluckedNotes[i]) {
      if (debug) {
        Serial.print("------PLUCKED:");
        Serial.println(i);
      }
      
      //There can't be more than two notes. So get them both stashed.
      if (firstNote == -1) {
        firstNote = i;
      } 
      else {
        secondNote = i;        
      }      

    }
  }

  playNote(firstNote, secondNote); 
  
  return;
}



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

  //return;
  //There must be at least a handful notes for the code below to work right.
  if (numberNotes < 5) {
    return;
  }

  int reading = analogRead(lightSensorPin);
  //Run the laser forward, read all values, and see what is there. Note that this pretty much uses one
  //less note than requested - but the START position counts as a spot. So moving it numberNotes makes that many
  //strings plus the start string.

  //It's already read the zero item. So read array items 1 through 7.
  for (int i = 1; i < numberNotes; i++) {
    reflectedLightValues[i] = stepTheMotorAndGetLightReading(FORWARD);
    checkSonar();
    reportNotes();
  }

  //It just read item 7. So going backwards, read items 6 through zero.
  for (int i = numberNotes-2; i >= 0; i--) {
    reflectedLightValues[i] = stepTheMotorAndGetLightReading(BACKWARD);
    checkSonar();
    reportNotes();
  }
    
  checkButtons();
  
}








