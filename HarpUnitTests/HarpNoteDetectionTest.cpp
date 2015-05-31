#include "stdafx.h"
#include "CppUnitTest.h"
#include "../HarpNoteDetection.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace HarpUnitTests
{
	TEST_CLASS(HarpNoteDetectionTest)
	{
	public:

		static const int numberNotes = 7;
		bool pluckedNotes[numberNotes + 1];
		int reflectedLightValues[numberNotes + 1];

		HarpNoteDetection harpNotes;

		TEST_METHOD_INITIALIZE(methodName)
		{
			harpNotes.setNumNotes(numberNotes);
			for (int i = 0; i < numberNotes + 1; i++) {
				pluckedNotes[i] = false;
				reflectedLightValues[i] = 105;
			}

			reflectedLightValues[2] = 100;
			reflectedLightValues[4] = 109;
			reflectedLightValues[5] = 107;

		}

		void checkNoNotesPlucked() {
			for (int i = 0; i < numberNotes; i++) {
				Assert::IsFalse(pluckedNotes[i]);
			}
		}

		void checkOneNotePlucked(int theNote) {
			checkTwoNotesPlucked(theNote, -1);
		}

		void checkTwoNotesPlucked(int theNote1, int theNote2) {
			for (int i = 0; i < numberNotes; i++) {
				if (i == theNote1 || i == theNote2){
					Assert::IsTrue(pluckedNotes[i]);
				}
				else {
					Assert::IsFalse(pluckedNotes[i]);
				}
			}
		}

		TEST_METHOD(NoNotesPlucked)
		{
			for (int i = 0; i < numberNotes; i++) {
				pluckedNotes[i] = true;
			}

			harpNotes.checkNotes(reflectedLightValues, pluckedNotes);
			checkNoNotesPlucked();
		}

		TEST_METHOD(PluckCase1) {
			reflectedLightValues[2] = 139;
			harpNotes.checkNotes(reflectedLightValues, pluckedNotes);
			checkOneNotePlucked(2);
		}

		TEST_METHOD(PluckCase2) {
			reflectedLightValues[2] = 212;
			harpNotes.checkNotes(reflectedLightValues, pluckedNotes);
			checkOneNotePlucked(2);
		}

		TEST_METHOD(PluckCase3) {
			reflectedLightValues[2] = 212;
			reflectedLightValues[6] = 155;
			harpNotes.checkNotes(reflectedLightValues, pluckedNotes);
			checkTwoNotesPlucked(2, 6);
		}

		TEST_METHOD(PluckCase4) {
			reflectedLightValues[0] = 142;
			harpNotes.checkNotes(reflectedLightValues, pluckedNotes);
			checkOneNotePlucked(0);
		}

		TEST_METHOD(PluckCase5) {
			reflectedLightValues[6] = 142;
			harpNotes.checkNotes(reflectedLightValues, pluckedNotes);
			checkOneNotePlucked(6);
		}

		TEST_METHOD(PluckCase6) {
			//One value comes in very low. Shouldn't count!
			reflectedLightValues[2] = 42;
			harpNotes.checkNotes(reflectedLightValues, pluckedNotes);
			checkNoNotesPlucked();
		}

		TEST_METHOD(PluckCase7) {
			//One value comes in very low, one high. Get it right!
			reflectedLightValues[2] = 42;
			reflectedLightValues[6] = 142;
			harpNotes.checkNotes(reflectedLightValues, pluckedNotes);
			checkOneNotePlucked(6);
		}

		TEST_METHOD(GetNotesCase1) {
			int note1, note2;
			Assert::IsTrue(harpNotes.getNotes(note1, note2, pluckedNotes));
			Assert::AreEqual(-1, note1);
			Assert::AreEqual(-1, note2);
		}

		TEST_METHOD(GetNotesCase2) {
			int note1, note2;
			pluckedNotes[2] = true;
			Assert::IsTrue(harpNotes.getNotes(note1, note2, pluckedNotes));
			Assert::AreEqual(2, note1);
			Assert::AreEqual(-1, note2);
		}

		TEST_METHOD(GetNotesCase3) {
			int note1, note2;
			pluckedNotes[0] = true;
			pluckedNotes[6] = true;
			Assert::IsTrue(harpNotes.getNotes(note1, note2, pluckedNotes));
			Assert::AreEqual(0, note1);
			Assert::AreEqual(6, note2);
		}



	};
}