#pragma once
class HarpNoteDetection
{
public:
	HarpNoteDetection();
	~HarpNoteDetection();

	void setNumNotes(int notes) { numberNotes = notes; }
	void checkNotes(int reflectedLightValues[], bool pluckedNotes[]);

private:
	int numberNotes;


};

