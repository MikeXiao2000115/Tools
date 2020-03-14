#include "pch.h"
#include "CppUnitTest.h"

#include "../Tools/Integer.h"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <ratio>
#include <complex>
#include <cmath>
#include <iterator>
#include <string>
#include <iomanip>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace integertest
{
	TEST_CLASS(integertest)
	{
	public:
		std::random_device rd; std::mt19937 gen{rd()};
		TEST_CLASS_INITIALIZE(ClassInitialize) {
		}
		TEST_METHOD(compare)
		{
			tools::Int a = 0;
			tools::Int b = -1;
			tools::Int c = 1;
			tools::Int d;
			Assert::IsTrue(a==a);
			Assert::IsTrue(a>=a);
			Assert::IsTrue(a<=a);
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
		TEST_METHOD(addOne_subOne) {
			std::uniform_int_distribution<unsigned short> dis_s;
			std::bernoulli_distribution d;
			double n = 0;
			const auto tis(std::chrono::high_resolution_clock::now());
			for (size_t rep = 0; rep < 4; ++rep) {
				auto a = tools::randomInt(gen, 12);
				auto b = a;
				long long c = 0;
				for (size_t i = 0, end = dis_s(gen); i < end*5; ++i) {
					if (d(gen)) {
						if (d(gen)) ++a;
						else a++;
						++c;
					}
					else {
						if (d(gen)) --a;
						else a--;
						--c;
					}
					++n;
				}
				while (c > 0) {
					if (d(gen)) --a;
					else a--;
					--c;
					++n;
				}
				while (c < 0) {
					if (d(gen)) ++a;
					else a++;
					++c;
					++n;
				}
				Assert::IsTrue(a == b);
			}
			const auto tic(std::chrono::high_resolution_clock::now());
			std::chrono::duration<double, std::ratio_multiply<std::chrono::duration<double>::period, std::nano>> dur(tic - tis);
			Logger::WriteMessage(("avg. time of ++/--: "+std::to_string(dur.count()/n)+"ns.").c_str());
		}
		TEST_METHOD(adding_val) {
			for (size_t rep = 0; rep < 4; ++rep) {
				auto a = tools::randomInt(gen, 15);
				auto b = tools::randomInt(gen, 3, false);
				tools::Int c = a;
				a += b;
				while (b > 0) {
					++c;
					--b;
				}
				Assert::IsTrue(a == c);
			}
		}
		TEST_METHOD(adding_per) {
			const auto tis(std::chrono::high_resolution_clock::now());
			for (size_t rep = 0; rep < 1024; ++rep) {
				auto a = tools::randomInt(gen, 12);
				auto b = tools::randomInt(gen, 12);
				auto c = tools::randomInt(gen, 12);

				Assert::IsTrue(a + b == b + a);
				Assert::IsTrue(a + 0 == a);
				Assert::IsTrue(0 + a == a);

				Assert::IsTrue(b + c == c + b);
				Assert::IsTrue(b + 0 == b);
				Assert::IsTrue(0 + b == b);

				Assert::IsTrue(c + b == b + c);
				Assert::IsTrue(c + 0 == c);
				Assert::IsTrue(0 + c == c);

				Assert::IsTrue((a + b) + c == a + (b + c));
			}
			const auto tic(std::chrono::high_resolution_clock::now());
			std::chrono::duration<double, std::ratio_multiply<std::chrono::duration<double>::period, std::nano>> dur(tic - tis);
			Logger::WriteMessage(("avg. time of +: " + std::to_string(dur.count() / (1024.0 * 16)) + "ns.").c_str());
		}
		TEST_METHOD(bitop) {
			for (size_t rep = 0; rep < 1024; ++rep) {
				auto a = tools::randomInt(gen, 53);
				auto b = tools::randomInt(gen, 34);

				Assert::IsTrue(~(~a) == a);
				Assert::IsTrue(~(~b) == b);
				Assert::IsTrue(~(~b & ~a) == (a | b));
				Assert::IsTrue((a ^ b) == ((~a & b) | (a & ~b)));

				Assert::IsTrue(((a << rep) >> rep) == a);
				Assert::IsTrue(((b << rep) >> rep) == b);
			}
		}
		TEST_METHOD(mult_val_adding) {
			for (size_t rep = 0; rep < 4; ++rep) {
				auto a = tools::randomInt(gen, 15);
				auto b = tools::randomInt(gen, 2, false);
				tools::Int c = 0;
				auto d = a * b;
				while (b > 0) {
					c += a;
					--b;
				}
				Assert::IsTrue(d == c);
			}
		}
		TEST_METHOD(mult_val_quick) {
			for (size_t rep = 0; rep < 4; ++rep) {
				auto a = tools::randomInt(gen, 15);
				auto b = tools::randomInt(gen, 8, false);
				tools::Int c = 0;
				auto d = a * b;
				while (b > 0) {
					if ((b & 1) != 0) c += a;
					a += a;
					//auto tmp = a;
					//a += tmp;
					b >>= 1;
				}
				//Logger::WriteMessage((d.to_bstring() + "\n").c_str());
				Assert::IsTrue(d == c);
			}
		}
		TEST_METHOD(mult_per) {
			const auto tis(std::chrono::high_resolution_clock::now());
			for (size_t rep = 0; rep < 1024; ++rep) {
				auto a = tools::randomInt(gen, 12);
				auto b = tools::randomInt(gen, 12);
				auto c = tools::randomInt(gen, 12);

				Assert::IsTrue(a * b == b * a);
				Assert::IsTrue(a * 1 == a);
				Assert::IsTrue(1 * a == a);

				Assert::IsTrue(b * c == c * b);
				Assert::IsTrue(b * 1 == b);
				Assert::IsTrue(1 * b == b);

				Assert::IsTrue(c * b == b * c);
				Assert::IsTrue(c * 1 == c);
				Assert::IsTrue(1 * c == c);

				Assert::IsTrue((a * b) * c == a * (b * c));
			}
			const auto tic(std::chrono::high_resolution_clock::now());
			std::chrono::duration<double, std::ratio_multiply<std::chrono::duration<double>::period, std::nano>> dur(tic - tis);
			Logger::WriteMessage(("avg. time of *: " + std::to_string(dur.count() / (1024.0 * 16)) + "ns.").c_str());

		}
		TEST_METHOD(div) {
			for (size_t rep = 0; rep < 1024; ++rep) {
				auto a = tools::randomInt(gen, 12);
				auto b = tools::randomInt(gen, 12);
				if (b.zeroQ()) ++b;
				Assert::IsTrue(a == (a / b) * b + a % b);
			}
		}
		TEST_METHOD(self_ref) {
			for (size_t rep = 0; rep < 1024; ++rep) {
				auto a = tools::randomInt(gen, 12);
				{
					auto t = a, s = a;
					t |= t;
					s |= a;
					Assert::IsTrue(t == s);
				}
				{
					auto t = a, s = a;
					t &= t;
					s &= a;
					Assert::IsTrue(t == s);
				}
				{
					auto t = a, s = a;
					t ^= t;
					s ^= a;
					Assert::IsTrue(t == s);
				}
				{
					auto t = a, s = a;
					t += t;
					s += a;
					Assert::IsTrue(t == s);
				}
				{
					auto t = a, s = a;
					t *= t;
					s *= a;
					Assert::IsTrue(t == s);
				}
				{
					auto t = a, s = a;
					t -= t;
					s -= a;
					Assert::IsTrue(t == s);
				}
				if (a == 0)++a;
				{
					auto t = a, s = a;
					t /= t;
					s /= a;
					Assert::IsTrue(t == s);
				}
				{
					auto t = a, s = a;
					t %= t;
					s %= a;
					Assert::IsTrue(t == s);
				}
			}

		}
	};
}
