#pragma once
class HarpNoteDetection
{
public:
	HarpNoteDetection();
	~HarpNoteDetection();

	void setNumNotes(int notes) { numberNotes = notes; }
	void checkNotes(int reflectedLightValues[], bool pluckedNotes[]);
	bool getNotes(int& note1, int& note2, bool pluckedNotes[]);

private:
	int numberNotes;


};

