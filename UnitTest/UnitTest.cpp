#include "pch.h"
#include "CppUnitTest.h"


#include "../Tools/Integer.h"
#include <random>
#include <string>
#include <chrono>
#include <ratio>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(UnitTest)
	{
		std::random_device rd; std::mt19937 gen{rd()};
		TEST_CLASS_INITIALIZE(ClassInitialize) {
			//Logger::WriteMessage("In Class Initialize");
		}
		TEST_METHOD(compare) {
			tools::Int a = 0;
			tools::Int b = -1;
			tools::Int c = 1;
			tools::Int d;
			Assert::IsTrue(a == a);
			Assert::IsTrue(a >= a);
			Assert::IsTrue(a <= a);
			d = a;
			Assert::IsTrue(d == a);
			Assert::IsTrue(d >= a);
			Assert::IsTrue(d <= a);

			Assert::IsTrue(b == b);
			Assert::IsTrue(b >= b);
			Assert::IsTrue(b <= b);
			d = b;
			Assert::IsTrue(d == b);
			Assert::IsTrue(d >= b);
			Assert::IsTrue(d <= b);

			Assert::IsTrue(c == c);
			Assert::IsTrue(c >= c);
			Assert::IsTrue(c <= c);
			d = c;
			Assert::IsTrue(d == c);
			Assert::IsTrue(d >= c);
			Assert::IsTrue(d <= c);

			Assert::IsTrue(b < a);
			Assert::IsTrue(b <= a);
			Assert::IsTrue(b != a);
			Assert::IsTrue(a > b);
			Assert::IsTrue(a >= b);

			Assert::IsTrue(b < c);
			Assert::IsTrue(b <= c);
			Assert::IsTrue(b != c);
			Assert::IsTrue(c > b);
			Assert::IsTrue(c >= b);

			Assert::IsTrue(a < c);
			Assert::IsTrue(a <= c);
			Assert::IsTrue(a != c);
			Assert::IsTrue(c > a);
			Assert::IsTrue(c >= a);
		}
	};
}
