#include "stdafx.h"
#include "../HarpLibrary.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace HarpUnitTests
{
	TEST_CLASS(HarpLibraryTest)
	{
	public:
		
		int ginsingNote0;
		int ginsingNote1;
		int firstNote;
		int secondNote;
		TEST_METHOD(TestPicknotesNone)
		{
			HarpLibrary harpLib;
			ginsingNote0 = 10;
			ginsingNote1 = 20;

			harpLib.pickNotes(ginsingNote0, ginsingNote1, -1, -1);
			Assert::AreEqual(ginsingNote0, -1);
			Assert::AreEqual(ginsingNote1, -1);

			//Test an illegal value
			harpLib.pickNotes(ginsingNote0, ginsingNote1, -1, 10);
			Assert::AreEqual(ginsingNote0, -1);
			Assert::AreEqual(ginsingNote1, -1);
		}

		TEST_METHOD(TestPicknotesOne)
		{
			HarpLibrary harpLib;
			ginsingNote0 = 10;
			ginsingNote1 = 20;
			harpLib.pickNotes(ginsingNote0, ginsingNote1, 10, -1);
			Assert::AreEqual(ginsingNote0, 10);
			Assert::AreEqual(ginsingNote1, -1);

			ginsingNote0 = 10;
			ginsingNote1 = 20;
			harpLib.pickNotes(ginsingNote0, ginsingNote1, 20, -1);
			Assert::AreEqual(ginsingNote0, -1);
			Assert::AreEqual(ginsingNote1, 20);

			ginsingNote0 = -1;
			ginsingNote1 = 20;
			harpLib.pickNotes(ginsingNote0, ginsingNote1, 20, -1);
			Assert::AreEqual(ginsingNote0, -1);
			Assert::AreEqual(ginsingNote1, 20);

			ginsingNote0 = 10;
			ginsingNote1 = -1;
			harpLib.pickNotes(ginsingNote0, ginsingNote1, 10, -1);
			Assert::AreEqual(ginsingNote0, 10);
			Assert::AreEqual(ginsingNote1, -1);

		}

		TEST_METHOD(TestPicknotesTwo)
		{
			HarpLibrary harpLib;
			ginsingNote0 = 10;
			ginsingNote1 = 20;
			harpLib.pickNotes(ginsingNote0, ginsingNote1, 10, 20);
			Assert::AreEqual(ginsingNote0, 10);
			Assert::AreEqual(ginsingNote1, 20);

			ginsingNote0 = 10;
			ginsingNote1 = 20;
			harpLib.pickNotes(ginsingNote0, ginsingNote1, 20, 10);
			Assert::AreEqual(ginsingNote0, 10);
			Assert::AreEqual(ginsingNote1, 20);
		}


		//For anomaly ...
		//Very low values were being picked up.
		//300 100 300 300 300 300 --- the 100 was being detected as a played note.
	};
}

