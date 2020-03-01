#include <algorithm>
#include <iostream>
#include <chrono>
#include <ratio>
#include <thread>
#include <string>
#include <iomanip>
#include <Windows.h>
#include "arrvec.h"
#include "Integer.h"
#include "bitop.h"
#include <random>


using namespace tools;

template<class itp>
inline decltype(std::cout)& print_itr(itp first, itp last, std::string sep) { std::copy(first, last, std::ostream_iterator<long long>(std::cout, ", ")); return std::cout; }
template<class T>
inline decltype(std::cout)&  print_itr(T c, std::string sep=", ") { std::copy(std::cbegin(c), std::cend(c), std::ostream_iterator<long long>(std::cout, ", ")); return std::cout;}

unsigned long long q120(long long n) {
	//return Sum of r_max for a from 3 to n
	auto a = (n - 1) / 2;
	return ((a * (a + 1) * (2 * a + 1) / 3) << 2) + ((n % 2) ? -a * (a + 1) : 3 * a * (a + 1));
}

template<class con, class funct>
void _print_con(con b, funct func, std::string name) {
	std::cout << name << ".size()=" << b.size() << "\n" << name << " = {\n ";
	for (int i = 0; i < b.size();)
		std::cout << "[" << std::setw(3) << i << " ]: " << func(b[i]) << (++i < b.size() ? "\n " : "");
	std::cout << "\n}" << std::endl;
}
#define PrintCon(con,toString) _print_con(con,toString,#con)

double timing(size_t u, size_t blocks_num) {
	const size_t block_size = 1ull << u;

	const size_t n = blocks_num * block_size;
	std::vector<unsigned long long> num(block_size, 0);
	std::vector<unsigned long long> prime;
	prime.reserve(n);

	const auto tis(std::chrono::high_resolution_clock::now());
	unsigned long long last_back;
	prime.push_back(2);
	std::vector<std::thread> thr(blocks_num - 1);
	for (size_t block_id = 0; block_id < blocks_num; ++block_id) {
		std::fill(num.begin(), num.end(), 0);

		for (size_t id = 1; id < block_id; ++id)
			thr[id - 1] = std::move(std::thread([id, &prime, &num, &block_id, &block_size]() {
			for (const auto& p : prime) {
				auto idp = id * p;
				auto tmp = idp + (block_size - 1) * p / block_size == block_id + p;
				if (tmp) num[(block_size - 1) * p % block_size] = p;
				if (idp > block_id) break;
				for (size_t k = 0; k < block_size - 1; ++k) {
					auto kpb = k * p / block_size;
					if (kpb > block_id) break;
					if (idp + kpb == block_id) num[p * k % block_size] = p;

					if ((id * block_size + k + 1) % p == 0)
						break;
				}

			}
			return; }));
		for (const auto& p : prime) {
			for (size_t k = 2; k < block_size - 1; ++k) {
				auto kpb = k * p / block_size;
				if (kpb > block_id) break;
				if (kpb == block_id) num[p * k % block_size] = p;

				if ((k + 1) % p == 0)
					break;
			}

		}

		for (size_t id = 1; id < block_id; ++id) thr[id - 1].join();

		if (block_id) {
			for (const auto& p : prime) {
				auto a = (block_size - 1) * p / block_size;
				auto b = p * (block_id - 1);
				if (a > block_id || b > block_id) break;
				if (a + b == block_id)
					num[(block_size - 1) * p % block_size] = p;
				if (last_back == p)	break;
			}
			for (size_t k = 0; k < block_size; ++k)
				if (!num[k]) prime.push_back(static_cast<unsigned long long>(block_id * block_size + k));
		}
		else {
			unsigned long long min_prime;
			for (size_t k = 2; k < block_size - 1; ++k) {
				min_prime = num[k];
				for (const auto& p : prime) {
					if (k * p < block_size) num[k * p] = p;
					else break;
					if (min_prime == p)	break;
				}
				if (!num[k + 1]) prime.push_back(static_cast<unsigned long long>(k + 1));
			}
		}

		last_back = num.back();
	}

	const auto tic(std::chrono::high_resolution_clock::now());
	std::chrono::duration<double, std::ratio_multiply<std::chrono::duration<double>::period, std::nano>> dur(tic - tis);
	return dur.count();
}

int main() {
	std::random_device rd;
	std::mt19937 gen{rd()};
	std::uniform_int_distribution<> dis{0,255};
	std::uniform_int_distribution<> dis_n{0,15};

	/*
	unsigned long s;

	for (auto count = 0; count < 10000; ++count) {
		std::uniform_int_distribution<unsigned long> d{0,bitop::fullmask<unsigned long>};
		s = d(gen);
		std::cout << "lowest  0: " << std::string(32 - bitop::lowestbit<unsigned long, false>(s), ' ') << "v\n            " << bitop::to_string(s) << std::endl;
		std::cout << "lowest  1: " << std::string(32 - bitop::lowestbit<unsigned long, true>(s), ' ') << "v\n            " << bitop::to_string(s) << std::endl;
		std::cout << "highest 0: " << std::string(32 - bitop::highestbit<unsigned long, false>(s), ' ') << "v\n            " << bitop::to_string(s) << std::endl;
		std::cout << "highest 1: " << std::string(32 - bitop::highestbit<unsigned long, true>(s), ' ') << "v\n            " << bitop::to_string(s) << std::endl;
		std::cout << std::endl;
		std::cout << "normal :    " << bitop::to_string(s, true, true) << std::endl;
		std::cout << "reverse:    " << bitop::to_string(bitop::reverse(s), true, true) << std::endl;
		system("pause");
		system("cls");
	}
	return 0;

	*/
	//Int A = -Int({150, 11, 222, 10, 119, 52, 137, 146, 47, 138, 78, 102,  238, 120, 0});
	//Int B = -Int({125, 187, 197, 142, 98, 82, 198, 105, 42, 137, 228, 231});
	//
	//A.print();
	//B.print();
	//(A * B).print();
	//
	//return 0;

	for (auto count = 0; count < 2048; ++count) {
		//std::cout << "test no." << (count + 1) << ":" << std::endl;
		std::vector<int> init;
		std::generate_n(std::back_inserter(init), dis_n(gen), [&] {return dis(gen); });
		Int a(init.begin(), init.end(), dis(gen) > 127); init.clear();
		std::generate_n(std::back_inserter(init), dis_n(gen), [&] {return dis(gen); });
		Int b(init.begin(), init.end(), dis(gen) > 127); init.clear();
		std::generate_n(std::back_inserter(init), dis_n(gen), [&] {return dis(gen); });
		//Int c(init.begin(), init.end(), dis(gen) > 127); init.clear();
		if (b == 0)++b;
#define T(exp) std::cout << std::setw(7) << #exp << ": " << (exp).to_bstring(7, true) << std::endl;
#define B(exp) std::cout << std::setw(7) << #exp << ": " << std::boolalpha <<(exp) << std::endl;

		std::cout << "{" << std::endl;
		a.print();
		std::cout << "," << std::endl;
		b.print();
		std::cout << "," << std::endl;
		(a + b).print();
		std::cout << "," << std::endl;
		(a - b).print();
		std::cout << "," << std::endl;
		(a * b).print();
		std::cout << "," << std::endl;
		(a / b).print();
		std::cout << "," << std::endl;
		(a % b).print();
		std::cout << "}," << std::endl;
	}
	return 0;

	/*/
	using testing = arrvec<std::pair<int, int>, 5>;
	auto print_pair_int = [](std::pair<int, int> b)->std::string {return "(" + std::to_string(b.first) + ", " + std::to_string(b.second) + ")"; };

	testing b;

	for (int i = 0; i < 5; i++)
		b.emplace_back(i,i);

	PrintCon(b, print_pair_int);
	for (auto it = b.begin(); it < b.end(); it += 5) {
		it = b.insert(it, {{10,10},{11,11},{12,12},{13,13}});
	}

	PrintCon(b, print_pair_int);
	for (auto it = b.begin(); ++it < b.end();) {
		it = b.erase(it);
	}

	PrintCon(b, print_pair_int);

	//system("pause");
	return 0;
	arrvec<int, 10> a = {1,2,3,4,5,6};
	auto print_int = [](auto a) {return std::to_string(a); };
	PrintCon(a, print_int);

	a.emplace_back(100);
	a.emplace_back(101);
	a.emplace_back(102);
	a.emplace_back(103);
	a.emplace_back(104);
	a.emplace_back(105);
	a.emplace_back(106);
	a.emplace_back(107);

	std::cout << std::endl;
	PrintCon(a, print_int);
	std::cout << std::endl;

	for (int i = 0; i < 5; ++i)
		std::cout << a.pop_take_back().value() << std::endl;

	std::cout << std::endl;
	PrintCon(a, print_int);
	std::cout << std::endl;

	for (int i = 0; i < 20; ++i) {
		if (!a.empty())
			std::cout << a.back() << ", ";
		a.pop_back();
	}
	std::cout << std::endl;

	std::cout << std::endl;
	PrintCon(a, print_int);


	/*prime
	{
		const size_t u = 12;
		const size_t blocks_num = 1ull << 9;

		double base, testing;

		system("pause");
		return 0;
		std::vector<unsigned long long> p1, p2;
		{
			const size_t n = blocks_num * (1ull << u);
			std::vector<unsigned long long> num(n, 0);
			std::vector<unsigned long long> prime;
			prime.reserve(n);

			const auto tis(std::chrono::high_resolution_clock::now());
			unsigned long long min_prime;
			for (size_t i = 2; i < n; ++i) {
				if (!num[i])
					prime.push_back(static_cast<unsigned long long>(i));
				min_prime = num[i];
				for (auto it = prime.begin(), end = prime.end(); it != end; ++it) {
					if (i * (*it) < n) //make branch prediction happy?
						num[i * (*it)] = *it;
					else
						break;
					if (min_prime == *it)
						break;
				}
			}

			const auto tic(std::chrono::high_resolution_clock::now());
			std::chrono::duration<double, std::ratio_multiply<std::chrono::duration<double>::period, std::nano>> dur(tic - tis);
			std::cout << "used: " << (base = (dur.count() / prime.size())) << "ns/prime" << std::endl;
			std::cout << "used: " << dur.count() / 1000000.0 << "ms" << std::endl;
			std::cout << "found: " << prime.size() << " primes" << std::endl;
			std::cout << "last prime: " << prime.back() << " primes" << std::endl;
			std::cout << std::endl;

			p1 = std::move(prime);
		}
		{
			const size_t block_size = 1ull << u;

			const size_t n = blocks_num * block_size;
			std::vector<unsigned long long> num(block_size, 0);
			std::vector<unsigned long long> prime;
			prime.reserve(n);

			const auto tis(std::chrono::high_resolution_clock::now());
			unsigned long long last_back;
			prime.push_back(2);
			std::vector<std::thread> thr(blocks_num - 1);
			for (size_t block_id = 0; block_id < blocks_num; ++block_id) {
				std::fill(num.begin(), num.end(), 0);

				for (size_t id = 1; id < block_id; ++id)
					thr[id - 1] = std::move(std::thread([id, &prime, &num, &block_id, &block_size]() {
					for (const auto& p : prime) {
						auto idp = id * p;
						auto tmp = idp + (block_size - 1) * p / block_size == block_id + p;
						if (tmp) num[(block_size - 1) * p % block_size] = p;
						if (idp > block_id) break;
						for (size_t k = 0; k < block_size - 1; ++k) {
							auto kpb = k * p / block_size;
							if (kpb > block_id) break;
							if (idp + kpb == block_id) num[p * k % block_size] = p;

							if (!(id * block_size + k + 1) % p)
								break;
						}

					}
					return; }));
				for (const auto& p : prime) {
					for (size_t k = 2; k < block_size - 1; ++k) {
						auto kpb = k * p / block_size;
						if (kpb > block_id) break;
						if (kpb == block_id) num[p * k % block_size] = p;

						if (!(k + 1) % p)
							break;
					}

				}

				for (size_t id = 1; id < block_id; ++id) thr[id - 1].join();

				if (block_id) {
					for (const auto& p : prime) {
						auto a = (block_size - 1) * p / block_size;
						auto b = p * (block_id - 1);
						if (a > block_id || b > block_id) break;
						if (a + b == block_id)
							num[(block_size - 1) * p % block_size] = p;
						if (last_back == p)	break;
					}
					for (size_t k = 0; k < block_size; ++k)
						if (!num[k]) prime.push_back(static_cast<unsigned long long>(block_id * block_size + k));
				}
				else {
					unsigned long long min_prime;
					for (size_t k = 2; k < block_size - 1; ++k) {
						min_prime = num[k];
						for (const auto& p : prime) {
							if (k * p < block_size) num[k * p] = p;
							else break;
							if (min_prime == p)	break;
						}
						if (!num[k + 1]) prime.push_back(static_cast<unsigned long long>(k + 1));
					}
				}

				last_back = num.back();
			}
			{
				const auto tic(std::chrono::high_resolution_clock::now());
				std::chrono::duration<double, std::ratio_multiply<std::chrono::duration<double>::period, std::nano>> dur(tic - tis);
				std::cout << "used: " << (testing = (dur.count() / prime.size())) << "ns/prime" << std::endl;
				std::cout << "used: " << dur.count() / 1000000000.0 << "s" << std::endl;
				std::cout << "found: " << prime.size() << " primes" << std::endl;
				std::cout << "last prime: " << prime.back() << " primes" << std::endl;
				std::cout << std::endl;

				p2 = std::move(prime);
			}
		}
		std::cout << std::boolalpha << "same result: " << std::equal(p1.begin(), p1.end(), p2.begin()) << std::endl;
		std::cout << "new/base: " << testing / base << " (" << testing / base * 100 << "%)" << std::endl;
	}
	/**/
}
/*
{
	const size_t block_size = 1ull << u;
	const size_t n = blocks_num * block_size;
	std::vector<std::vector<unsigned long long>> num(blocks_num, std::vector<unsigned long long>((1ull << u), 0));
	std::vector<unsigned long long> prime;
	prime.reserve(n);

	const auto tis(std::chrono::high_resolution_clock::now());
	unsigned long long min_prime;

	for (size_t block_id = 0; block_id < blocks_num; ++block_id) {
		for (size_t k = block_id ? 0 : 2; k < (1ull << u); ++k) {

			// current range:	[block * (1ull << u), (block + 1) * (1ull << u))
			// mapped to num:	[0, (1ull << u))

			if (!num[block_id][k])
				prime.push_back(block_id * block_size + k);
			min_prime = num[block_id][k];

			for (auto it = prime.begin(), end = prime.end();
				 it != end;
				 ++it) {
				auto k_shift = (k * (*it)) / block_size;
				if ((block_id * (*it) < blocks_num) && (k_shift < blocks_num - block_id * (*it)))
					num[block_id * (*it) + k_shift][(k * (*it)) % block_size] = *it;
				else
					break;
				if (min_prime == *it)
					break;
			}

		}
	}
	const auto tic(std::chrono::high_resolution_clock::now());
	std::chrono::duration<double, std::ratio_multiply<std::chrono::duration<double>::period, std::nano>> dur(tic - tis);
	std::cout << "used: " << dur.count() / prime.size() << "ns/prime" << std::endl;
	std::cout << "used: " << dur.count() / 1000 << "us" << std::endl;
	std::cout << "found: " << prime.size() << " primes" << std::endl;
	std::cout << "last prime: " << prime.back() << " primes" << std::endl;
	std::cout << std::endl;

	p2 = std::move(prime);
}
*/

/*
				if (id) {
					for (auto it = prime.begin(), end = prime.end(); it != end; ++it) {
						auto tmp = (id - 1) * (*it) + (block_size - 1) * (*it) / block_size == block_id;
						auto shift = (block_size - 1) * (*it) % block_size;
						if (tmp) num[shift] = *it;

						if (!(id * block_size) % (*it))
							break;
					}

				}
				for (auto it = prime.begin(), end = prime.end(); it != end; ++it) {

				for (size_t k = id ? 1 : 3; k < block_size; ++k) {
						auto tmp = id * (*it) + (k - 1) * (*it) / block_size == block_id;
						auto shift = (k - 1) * (*it) % block_size;
						if (tmp) num[shift] = *it;
						if (!(id * block_size + k) % (*it))
							break;
					}

				}

*/
/*
	{
		const size_t block_size = 1ull << u;

		const size_t n = blocks_num * block_size;
		std::vector<unsigned long long> num(block_size, 0);
		std::vector<unsigned long long> prime;
		prime.reserve(n);

		const auto tis(std::chrono::high_resolution_clock::now());
		unsigned long long last_back;
		prime.push_back(2);
		for (size_t block_id = 0; block_id < blocks_num; ++block_id) {
			std::fill(num.begin(), num.end(), 0);

			for (size_t id = 0; id < block_id; ++id) {
				for(const auto& p :prime) {
					auto idp = id * p;
					if (id) {
						auto tmp = id * p + (block_size - 1) * p/ block_size == block_id + p;
						if (tmp) num[(block_size - 1) * p % block_size] = p;
						if (idp > block_id) break;
					}
					for (size_t k = id ? 0 : 2; k < block_size - 1; ++k) {
						auto kpb = k * p / block_size;
						if (kpb > block_id) break;
						if (idp + kpb == block_id) num[p * k % block_size] = p;

						if (!(id * block_size + k + 1) % p)
							break;
					}

				}
			}

			if (block_id) {
				for (const auto& p : prime) {
					auto a = (block_size - 1) * p / block_size;
					auto b = p * (block_id - 1);
					if (a > block_id || b > block_id) break;
					if (a + b == block_id);
						num[(block_size - 1) * p % block_size] = p;
					if (last_back == p)	break;
				}
				for (size_t k = 0; k < block_size; ++k)
					if (!num[k]) prime.push_back(static_cast<unsigned long long>(block_id * block_size + k));
			}
			else {
				unsigned long long min_prime;
				for (size_t k = 2; k < block_size - 1; ++k) {
					min_prime = num[k];
					for (const auto& p : prime) {
						if (k * p < block_size) num[k * p] = p;
						else break;
						if (min_prime == p)	break;
					}
					if (!num[k + 1]) prime.push_back(static_cast<unsigned long long>(k + 1));
				}
			}

			last_back = num.back();
		}
		{
			const auto tic(std::chrono::high_resolution_clock::now());
			std::chrono::duration<double, std::ratio_multiply<std::chrono::duration<double>::period, std::nano>> dur(tic - tis);
			std::cout << "used: " << (testing=(dur.count() / prime.size())) << "ns/prime" << std::endl;
			std::cout << "used: " << dur.count() / 1000 << "us" << std::endl;
			std::cout << "found: " << prime.size() << " primes" << std::endl;
			std::cout << "last prime: " << prime.back() << " primes" << std::endl;
			std::cout << std::endl;

			p2 = std::move(prime);
		}
	}
*/