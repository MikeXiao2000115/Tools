#include <algorithm>
#include <iostream>
#include <chrono>
#include <ratio>
#include <thread>
#include <complex>
#include <cmath>
#include <iterator>
#include <string>
#include <iomanip>
#include <Windows.h>
#include "arrvec.h"
#include "Integer.h"
#include "bitop.h"
#include "alg.h"
#include <random>
#include <numeric>


using namespace tools;
using namespace std;

std::random_device rd; std::mt19937 gen{rd()};
template<class T> T rand_of() {
	if constexpr (std::is_integral_v<T>)
		return std::uniform_int_distribution<T>{std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max()}(gen);
	else
		return std::uniform_real_distribution<T>{std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max()}(gen);
}

template<class itp>
inline decltype(std::cout)& print_itr(itp first, itp last, std::string sep) { std::copy(first, last, std::ostream_iterator<long long>(std::cout, ", ")); return std::cout; }
template<class T>
inline decltype(std::cout)&  print_itr(T c, std::string sep=", ") { std::copy(std::cbegin(c), std::cend(c), std::ostream_iterator<long long>(std::cout, ", ")); return std::cout;}

const double Pi = 3.1415926535897932385;

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
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now - std::chrono::hours(24));
	std::cout << "24 hours ago, the time was "	<< std::put_time(std::localtime(&now_c), "%F %T") << '\n';
	while (true) {
		auto test = randomInt(gen, 5);
		auto test2 = randomInt(gen, 5);
		test *= test2;
		cout << test.to_bstring() << endl;
		system("pause");
	}
	std::uniform_int_distribution<> dis{0,255};
	std::uniform_int_distribution<> dis_n{1000,100000};
	std::uniform_real_distribution<> dis_r{0,10};

	std::uniform_int_distribution<unsigned short> dis_s{0,(unsigned short)(-1)};
	std::uniform_int_distribution<unsigned int> dis_i{0,(unsigned int)(-1)};
	std::uniform_int_distribution<unsigned long> dis_l{0,(unsigned long)(-1)};
	std::uniform_int_distribution<unsigned long long> dis_ll{0,(unsigned long long)(-1)};
/*
	while (true) {
		auto m = dis_s(gen);
		if (!m) continue;
		system("cls");

		auto ck = alg::mod::check_u(m);
		auto a = ck(dis_s(gen));
		auto b = ck(dis_s(gen));

		auto A = static_cast<unsigned long long>(a);
		auto B = static_cast<unsigned long long>(b);
		auto M = static_cast<unsigned long long>(m);

		unsigned short res;
		unsigned long long ans;

		cout << "a = " << setw(7) << a << "\tb = " << setw(7) << b << "\tm = " << setw(7) << m; if (a >= 1ul << sizeof(unsigned short) * 4 && b >= 1ul << sizeof(unsigned short) * 4)cout << "\t!!!notice!!!" << endl; else cout << endl;
		cout << "a+b = " << (res = alg::mod::plus(m)(a, b)) << " = " << (ans = (A + B) % M) << " (mod m)"; if (res == ans)cout << endl; else { cout << "Error" << endl; return 0; }
		cout << "-a = " << (res = alg::mod::neg(m)(a)) << " (mod m)"; if (alg::mod::plus(m)(res, a) == 0)cout << endl; else { cout << "Error" << endl; return 0; }
		cout << "-b = " << (res = alg::mod::neg(m)(b)) << " (mod m)"; if (alg::mod::plus(m)(res, b) == 0)cout << endl; else { cout << "Error" << endl; return 0; }
		cout << "a-b = " << (res = alg::mod::minus(m)(a, b)) << " (mod m)"; if (alg::mod::plus(m)(res, b) == a)cout << endl; else { cout << "Error" << endl; return 0; }
		cout << "a*b = " << (res = alg::mod::mult(m)(a, b)) << " = " << (ans = (A * B) % M) << " (mod m)"; if (res == ans)cout << endl; else { cout << "Error" << endl; return 0; }
		if (std::gcd(b, m) == 1 && b){
			cout << "a/b = " << (res = alg::mod::div(m)(a, b)) << " (mod m)"; if (alg::mod::mult(m)(res, b) == a)cout << endl; else { cout << "Error" << endl; return 0; }
			cout << "1/b = " << (res = alg::mod::inv(m)(b)) << " (mod m)"; if (alg::mod::mult(m)(res, b) == 1)cout << endl; else { cout << "Error" << endl; return 0; }
		}
		if (std::gcd(a, m) == 1 && a){
			cout << "b/a = " << (res = alg::mod::div(m)(b, a)) << " (mod m)"; if (alg::mod::mult(m)(res, a) == b)cout << endl; else { cout << "Error" << endl; return 0; }
			cout << "1/a = " << (res = alg::mod::inv(m)(a)) << " (mod m)"; if (alg::mod::mult(m)(res, a) == 1)cout << endl; else { cout << "Error" << endl; return 0; }
		}
		//system("pause");
	}
	*/

	double alpha = 0, gamma = 0, gamma2 = 0;
	for (int i = 0; i < 25;++i) {
		//system("cls");
		vector<alg::int_u_t<64>>a, b, c;
		std::generate_n(back_insert_iterator(a), dis_n(gen) + 1, [&] {return dis_s(gen) % 100; });
		std::generate_n(back_insert_iterator(b), dis_n(gen) + 1, [&] {return dis_s(gen) % 100; });

		auto logn = alg::log2n(a.size() + b.size() - 1);
		size_t len = 1ull << logn;
		cout << "a: " << setw(7) << a.size() << "\tb: " << setw(7) << b.size() << "\tlog2n: " << setw(3) << logn << flush;
		//cout << "a = ";	copy(a.cbegin(), a.cend(), ostream_iterator<alg::int_u_t<64>>(cout, ", "));	cout << endl;
		//cout << "b = "; copy(b.cbegin(), b.cend(), ostream_iterator<alg::int_u_t<64>>(cout, ", ")); cout << endl;

		
		{
			const auto tis(std::chrono::high_resolution_clock::now());
			c.reserve(len);
			alg::convolution_linear(a.cbegin(), a.cend(), b.cbegin(), b.cend(), back_insert_iterator(c), 0ull);
			const auto tic(std::chrono::high_resolution_clock::now());

			//cout << "c1 = "; copy(c.cbegin(), c.cend(), ostream_iterator<alg::int_u_t<64>>(cout, ", "));
			std::chrono::duration<double, std::ratio_multiply<std::chrono::duration<double>::period, std::nano>> dur(tic - tis);
			std::cout << "\t" << fixed << setprecision(3) <<setw(9)<< dur.count() / 1000000 << "ms alpha: " << fixed << setprecision(3) << dur.count() / (a.size() * b.size()) << flush;
			alpha = alg::next_avg(alpha, i, dur.count() / (a.size() * b.size()));
		}
		c.clear();
		{
			const auto tis(std::chrono::high_resolution_clock::now());
			c = a;
			auto B = b;
			alg::auto_ntt_convolution(c, B);
			const auto tic(std::chrono::high_resolution_clock::now());

			//cout << "c2 = "; copy(c.cbegin(), c.cend(), ostream_iterator<alg::int_u_t<64>>(cout, ", "));
			std::chrono::duration<double, std::ratio_multiply<std::chrono::duration<double>::period, std::nano>> dur(tic - tis);
			std::cout << "\t\t" << fixed << setprecision(3) << setw(7) << dur.count() / 1000000 << "ms" << flush;
		}
		{
			const auto tis(std::chrono::high_resolution_clock::now());

			alg::NTT_convolution<alg::int_u_t<64>>(a, b, 5, 3, 30);

			const auto tic(std::chrono::high_resolution_clock::now());
			//cout << "c3 = "; copy(a.cbegin(), a.cend(), ostream_iterator<alg::int_u_t<64>>(cout, ", "));
			std::chrono::duration<double, std::ratio_multiply<std::chrono::duration<double>::period, std::nano>> dur(tic - tis);
			std::cout << "\t" << fixed << setprecision(3) << setw(7) << dur.count() / 1000000 << "ms gamma: " << fixed << setprecision(3) << dur.count() / (len * logn) << endl;
			gamma2 = alg::next_avg(gamma2, i, dur.count() / (len * logn));
		}
		//system("pause");
		
	}
	cout << "alpha: " << alpha << "\tgamma: " << gamma << "\tgamma2: " << gamma2 << endl;
	return 0;

}
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
