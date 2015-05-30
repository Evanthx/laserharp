#include "HarpNoteDetection.h"
#include <math.h>

const int pluckDelta = 30;


HarpNoteDetection::HarpNoteDetection()
{
}


HarpNoteDetection::~HarpNoteDetection()
{
}



void HarpNoteDetection::checkNotes(int reflectedLightValues[], bool pluckedNotes[]) {

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

	return;
}

