#include "HarpNoteChoice.h"

HarpNoteChoice::HarpNoteChoice()
{
}


HarpNoteChoice::~HarpNoteChoice()
{
}

void HarpNoteChoice::pickNotes(int& ginsingNote0, int& ginsingNote1, const int& firstNote, const int& secondNote) {

	//Not playing any notes? Stop and done.
	if (firstNote == -1 && secondNote == -1) {
		ginsingNote0 = -1;
		ginsingNote1 = -1;
		return;
	}

	//Double check that it's being called correctly ...
	//If only one note is being passed in, it MUST be the first!
	if (firstNote == -1) {
		ginsingNote0 = -1;
		ginsingNote1 = -1;
		return;
	}

	//Are we playing one note or two?
	if (secondNote == -1) {
		//is someone already playing this note? If so, they keep it.
		if (ginsingNote0 == firstNote) {
			ginsingNote1 = -1;
		}
		else if (ginsingNote1 == firstNote) {
			ginsingNote0 = -1;
		}
		else {
			ginsingNote0 = firstNote;
			ginsingNote1 = -1;
		}
	}
	else {
		//Two notes. it's either a direct assignment or a switch.
		if (ginsingNote0 == firstNote || ginsingNote1 == secondNote) {
			//At least one matched. Make sure they both do.
			ginsingNote0 = firstNote;
			ginsingNote1 = secondNote;
		}
		else {
			//The didn't match. Reverse them.
			ginsingNote1 = firstNote;
			ginsingNote0 = secondNote;
		}
	}

}
