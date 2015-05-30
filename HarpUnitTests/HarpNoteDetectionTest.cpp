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

		TEST_METHOD(NoNotesPlucked)
		{
			for (int i = 0; i < numberNotes; i++) {
				pluckedNotes[i] = true;
			}

			harpNotes.checkNotes(reflectedLightValues, pluckedNotes);
			for (int i = 0; i < numberNotes; i++) {
				Assert::IsFalse(pluckedNotes[i]);
			}
		}

	};
}