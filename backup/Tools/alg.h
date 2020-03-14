#pragma once
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <functional>
#include <vector>
#include <tuple>
#include <stdexcept>
#include <cstdint>
#include "bitop.h"

namespace tools {
	namespace alg {
		template<class it, class T = void> using enable_if_InputItr = typename std::enable_if_t<std::is_convertible<typename std::iterator_traits<it>::iterator_category, std::input_iterator_tag>::value, T>;
		template<class it, class T = void> using enable_if_OutputItr = typename std::enable_if_t<std::is_convertible<typename std::iterator_traits<it>::iterator_category, std::output_iterator_tag>::value, T>;

		template<size_t N> class int_u { static_assert(true, "unavailable length");};
		template<> class int_u<8> { public: using type = std::uint8_t; };
		template<> class int_u<16> { public: using type = std::uint16_t; };
		template<> class int_u<32> { public: using type = std::uint32_t; };
		template<> class int_u<64> { public: using type = std::uint64_t; };
		template<size_t N> using int_u_t = typename int_u<N>::type;

		template<size_t N> class int_s { static_assert(true, "unavailable length"); };
		template<> class int_s<8> { public: using type = std::int8_t; };
		template<> class int_s<16> { public: using type = std::int16_t; };
		template<> class int_s<32> { public: using type = std::int32_t; };
		template<> class int_s<64> { public: using type = std::int64_t; };
		template<size_t N> using int_s_t = typename int_s<N>::type;

		template<class T, class W> T inline next_avg(const T& cur_avg, const W& total_weight, const T& cur_val, const W& cur_weight = 1){
			return (cur_avg * total_weight + cur_val) / (total_weight + cur_weight);
		}

		template <class T> inline constexpr std::make_unsigned_t<T> abs(const T& a) { if constexpr (std::is_signed_v<T>) return a > 0 ? a : -a; else return a; }
		template <class T> constexpr inline T& dis(const T& a, const T& b) { return std::max(a, b) - std::min(a, b); }

		/*******************************************************
		input_1 is length M
		input_2 is length N

		then output length is N
		out[n] = sum(m form 0 to M-1) input_1[m] * ((input_2)_N)[m-n]
		((input_2)_N) is expand input_2 with cycle of N
		*******************************************************/
		template <class InputIt1, class InputIt2, class OutIt, class T, class = enable_if_InputItr<InputIt1>, class = enable_if_InputItr<InputIt2>, class = enable_if_OutputItr<OutIt>>
		void convolution_cyclic(InputIt1 first_1, InputIt1 last_1, InputIt2 first_2, InputIt1 last_2, OutIt out, T init) {
			auto rend2 = std::make_reverse_iterator(first_2), rbegin2 = std::make_reverse_iterator(last_2);
			while (first_2 != last_2) {
				auto rit = std::make_reverse_iterator(++first_2);
				auto buf = init;
				for (auto it = first_1; it != last_1;++it) {
					buf += *it * *rit++;
					if (rit == rend2) rit = rbegin2;
				}
				*out++ = buf;
			}
		}
		template <class InputIt1, class InputIt2, class OutIt, class T, class = enable_if_InputItr<InputIt1>, class = enable_if_InputItr<InputIt2>, class = enable_if_OutputItr<OutIt>>
		void convolution_linear(InputIt1 first_1, InputIt1 last_1, InputIt2 first_2, InputIt1 last_2, OutIt out, T init) {
			auto rend1 = std::make_reverse_iterator(first_1);
			while (first_1 != last_1) {
				auto rit1 = std::make_reverse_iterator(++first_1);
				auto buf = init;
				for (auto it2 = first_2; rit1 != rend1 && it2 != last_2; buf += *it2++ * *rit1++);
				*out++ = buf;
			}
			auto rbegin1 = std::make_reverse_iterator(last_1);
			while (++first_2 != last_2) {
				auto it2 = first_2;
				auto buf = init;
				for (auto rit1 = rbegin1; rit1 != rend1 && it2 != last_2;)
					buf += *it2++ * *rit1++;
				*out++ = buf;
			}
		}


		/*******************************************************
		a template for FFT and NTT base on 2

		where:
			  |	w(	0	, 2m) =	 1
			  |	w(	2k	, 2m) =  w(k,  m)
			  |	w(k + m	, 2m) = -w(k, 2m)
			  |	w(a + b	, 2m) =  w(a, 2m) * w(b, 2m)
		which implies w(1, 2^l)^2 = w(2, 2^l) = w(1, 2^(l-1))

		The transform makes:
		data[j] = Sum data[i]*w(i*j,2^log2n)
						where 0<= i, j < 2^log2n

		request:	random acess to data[i]
					omega = w(1, 2^log2n)
		*******************************************************/
		template<class Itr, class rotation,  class T = typename std::iterator_traits<Itr>::value_type,class RM = typename std::function<rotation(rotation, rotation)>,class RT = typename std::function<T(rotation, T)>,class M=typename std::function<T(T, T)>,class P = typename std::function<T(T, T)>>
		void RotationSumTransform_base2(Itr data, rotation omega, size_t log2n,
										size_t k = 1, size_t shift = 0,
										RM rotate = std::multiplies{}, RT rotate_prod = std::multiplies{},
										M minus = std::minus{}, P add = std::plus{}) {
			std::vector<rotation> cache; //cache[i] = w(1, 2^(log2n - i))
			cache.reserve(log2n);
			cache.push_back(omega);
			for (auto i = 1; i < log2n; ++i)
				cache.push_back(rotate(cache.back(), cache.back()));
			auto rotate0 = rotate(cache.back(), cache.back()); //unit element of roation

			size_t N = 1ull << log2n; //number of elements

			//reverse bits op
			for (size_t j, i = 1; i < N; ++i)
				if (bitop::quickreverse(j = i, log2n))
					std::swap(data[k * i + shift], data[k * j + shift]);

			for (size_t level = 0; level < log2n; ++level) {
				const size_t inputnum = 1ull << level, outputnum = inputnum << 1;
				auto w = rotate0; auto& w_rotate_step = cache.back();
				for (size_t i = 0; i < inputnum; ++i) {
					for (size_t j = i; j < N; j += outputnum) {
						//BufferflyDiagram
						auto& a = data[k * j + shift];
						auto& b = data[k * (j + inputnum) + shift];
						auto tmp = rotate_prod(w, b);
						b = minus(a, tmp);
						a = add(a, tmp);
					}
					w = rotate(w, w_rotate_step);
				}
				cache.pop_back();
			}
		}

		//return pair [q, r] s.t. num = q*div + r where 0<=r<abs(b)
		template <class T> std::pair<std::make_signed_t<T>, std::make_unsigned_t<T>> inline constexpr math_div(const T& num, const T& div) {
			if (!div)  throw std::invalid_argument{"divide by zero"};
			if constexpr (std::is_unsigned_v<T>) return {num / div,num % div};
			else {
				auto a = abs(num), b = abs(div);
				auto q = a / b;
				auto r = a % b;
				if (num < 0) { q = -q - 1; r = b - r; }
				if (div < 0) q = -q;
				return {q,r};
			}
		}
		
		//return the minium bits to store n
		template<class T> inline constexpr size_t log2n(const T& n) {
			auto l = bitop::highestbit<T, true>(n);
			if (n > 1ull << l)++l;
			return l;
		}

		//return (x, y, d) s.t. d = gcd(a, b) and d = ax + by
		template <class T, bool safe = true>
		std::tuple<std::make_signed_t<T>, std::make_signed_t<T>, std::make_unsigned_t<T>> constexpr inline exgcd(const T& a, const T& b) {
			if constexpr (safe) if (!a && !b) throw std::invalid_argument{"calculating gcd(0, 0)"}; //safe test
			if (a < b) { auto [y, x, d] = exgcd<T>(b, a); return std::make_tuple(x, y, d); } //forward to ensure a >= b
			// a >= b
			if (!b) return std::make_tuple(1, 0, a);
			std::make_unsigned_t<T> r_2 = alg::abs(a), r_1 = alg::abs(b);
			std::make_signed_t<T> x_ = 1, y_ = 0, x = 0, y = 1; // r_2 = ax_ + by_  r_1 = ax  + by

			while (true) {
				auto r = r_2 % r_1;
				if (!r) return std::make_tuple(x, y, r_1);
				auto q = r_2 / r_1;
				r_2 = r_1;
				r_1 = r;

				std::swap(x, x_); x -= q * x_;
				std::swap(y, y_); y -= q * y_;
			}
		}


		namespace mod {
			template<class T, typename = std::enable_if_t<std::is_integral_v<T>&&std::is_unsigned_v<T>>> auto inline constexpr plus(const T mod) {
#ifdef _DEBUG
				if (mod ==0 ) throw std::invalid_argument{"mod cannot be 0"};
#endif
				return [mod](T a, const T& b) constexpr ->T{
#ifdef _DEBUG
					if (a >= mod) throw std::out_of_range{"a cannot large or equal to mod"};
					if (b >= mod) throw std::out_of_range{"b cannot large or equal to mod"};
#endif
					a += b;
					return (a < b || a >= mod) ? a - mod : a;
				};
			}
			template<class T, typename = std::enable_if_t<std::is_integral_v<T>&&std::is_unsigned_v<T>>> auto inline constexpr neg(const T mod) {
#ifdef _DEBUG
				if (mod == 0) throw std::invalid_argument{"mod cannot be 0"};
#endif
				return [mod](const T& a) constexpr->T {
#ifdef _DEBUG
					if (a >= mod) throw std::out_of_range{"a cannot large or equal to mod"};
#endif
					return a ? mod - a : 0;
				};
			}
			template<class T, typename = std::enable_if_t<std::is_integral_v<T>&&std::is_unsigned_v<T>>> auto inline constexpr minus(const T& mod) {
				const auto pl = plus(mod); const auto ne = neg(mod);
				return [ne, pl](const T& a, const T& b) constexpr->T {return pl(a, ne(b)); };
			}
			template<class T, typename = std::enable_if_t<std::is_integral_v<T>&&std::is_unsigned_v<T>>> auto inline constexpr mult(const T mod) {
				const auto plu = plus(mod); const T c = 1ull << sizeof(T) * 4;
				return [mod,plu,c](const T& a, const T& b)constexpr->T {
#ifdef _DEBUG
					if (a >= mod) throw std::out_of_range{"a cannot large or equal to mod"};
					if (b >= mod) throw std::out_of_range{"b cannot large or equal to mod"};
#endif
					if ((a < c && b < c) || (log2n(a) + log2n(b) < sizeof(T) * 8))
						return a * b % mod;
					else {
						T res = bitop::gethigh<T>(a) * bitop::gethigh<T>(b) % mod;
						for (size_t i = 0; i < sizeof(T) * 4; ++i) res = plu(res, res); //res *= 2^(4*sizeof(U))
						res = plu(res, bitop::getlow<T>(a) * bitop::gethigh<T>(b) % mod);
						res = plu(res, bitop::gethigh<T>(a) * bitop::getlow<T>(b) % mod);
						for (size_t i = 0; i < sizeof(T) * 4; ++i) res = plu(res, res); //res *= 2^(4*sizeof(U))
						return plu(res, bitop::getlow<T>(a) * bitop::getlow<T>(b) % mod);
					}
				};
			}
			template<class T, typename = std::enable_if_t<std::is_integral_v<T>&&std::is_unsigned_v<T>>> auto inline constexpr inv(const T mod) {
#ifdef _DEBUG
				if (mod == 0) throw std::invalid_argument{"mod cannot be 0"};
#endif
				return [mod](T in)constexpr->T {
					T M = mod; std::make_signed_t<T> y_ = 0, y = 1;
#ifdef _DEBUG
					if (!in) throw std::invalid_argument{"0 don't have inverse"};
#endif
					while (true) {
						auto r = M % in;
						if (!r) {
#ifdef _DEBUG
							if (in != 1) throw std::invalid_argument{"no inverse as 'mod' and 'a' is not co-prime"};
#endif
							return static_cast<T>(y > 0 ? y : mod + y);
						}
						std::swap(y, y_); y -= (M / in) * y_;
						M = in; in = r;
					}
				};
			}
			template<class T, typename = std::enable_if_t<std::is_integral_v<T>&&std::is_unsigned_v<T>>> auto inline constexpr div(const T& mod) {
				const auto inverse = inv(mod);
				const auto multply = mult(mod);
				return [multply, inverse](const T& a, const T& b)constexpr {return multply(a, inverse(b)); };
			}
			template<class T, typename = std::enable_if_t<std::is_integral_v<T>&&std::is_unsigned_v<T>>> auto inline constexpr check_s(const T mod) {
#ifdef _DEBUG
				if (mod == 0) throw std::invalid_argument{"mod cannot be 0"};
#endif
				return [mod](const std::make_signed_t<T>& a)constexpr->T {return static_cast<T>(a > 0 ? a % mod : (mod - (-a) % mod)); };
			}
			template<class T, typename = std::enable_if_t<std::is_integral_v<T>&&std::is_unsigned_v<T>>> auto inline constexpr check_u(const T mod) {
#ifdef _DEBUG
				if (mod == 0) throw std::invalid_argument{"mod cannot be 0"};
#endif
				return [mod](const T& a)constexpr->T {return a % mod; };
			}
			template<class T, typename = std::enable_if_t<std::is_integral_v<T>&&std::is_unsigned_v<T>>> auto inline constexpr pow(const T mod) {
				const auto mul = mult(mod);
				return [mul, mod](T a, size_t exp)->T {
					T res = 1;
#ifdef _DEBUG
					if (a >= mod) throw std::out_of_range{"a cannot large or equal to mod"};
#endif
					while (exp) {
						if (exp & 1) res = mul(res, a);
						a = mul(a, a);
						exp >>= 1;
					}
					return res;
				};
			}
			template<class T, typename = std::enable_if_t<std::is_integral_v<T>&&std::is_unsigned_v<T>>> auto inline constexpr CRT_3(const T& m1, const T& m2, const T& m3) {
#ifdef _DEBUG
				if (m1> 1ull << sizeof(T) * 4) throw std::overflow_error{"m1 is too large"};
				if (m2> 1ull << sizeof(T) * 4) throw std::overflow_error{"m2 is too large"};
				if (m3> 1ull << sizeof(T) * 4) throw std::overflow_error{"m3 is too large"};
				if (m1 == 1) throw std::overflow_error{"mod 1 is meaning less"};
				if (m2 == 1) throw std::overflow_error{"mod 1 is meaning less"};
				if (m3 == 1) throw std::overflow_error{"mod 1 is meaning less"};

#endif
				T M = m1 * m2;
				auto mul_M = mult(M);
				auto inv1 = mul_M(m2, inv(m1)(check_u(m1)(m2)));
				auto inv2 = mul_M(m1, inv(m2)(check_u(m2)(m1)));
				auto inv3 = inv(m3)(check_u(m3)(M));
				return [M, mul_M, inv1, inv2, inv3, m1, m2, m3](const T& a1, const T& a2, const T& a3)constexpr->std::pair<T, T> {
#ifdef _DEBUG
					if (a1 >= m1) throw std::out_of_range{"a1 cannot large or equal to m1"};
					if (a2 >= m2) throw std::out_of_range{"a2 cannot large or equal to m2"};
					if (a3 >= m3) throw std::out_of_range{"a3 cannot large or equal to m3"};
#endif
					auto A = plus(M)(mul_M(inv1, a1), mul_M(inv2, a2));
					auto k = mult(m3)(inv3, minus(m3)(a3, check_u(m3)(A)));
					auto [h, l] = bitop::multunit(k, M);
					bool up = false;
					bitop::fulladder(l, A, up);
					if (up) ++h;
					return std::make_pair(h, l);
				};
			}
		}

		/*****************************************************************************
		calculate convolution of a and b emplace (both a and b would be destoried)
		the result would take mod of M
		where M = c * 2^k + 1 should be a prime and g is the primitive root under M
		*****************************************************************************/
		template<class T, bool remove_zero = true, class vec = typename std::vector<T>, typename = std::enable_if_t<std::is_integral_v<T>&&std::is_unsigned_v<T>>>
		void NTT_convolution(vec& a, vec& b, T g, size_t c, size_t k) {
			//calculate the cover range
			auto logn = log2n(a.size() + b.size() - 1);
			size_t len = 1ull << logn;
			if (k < logn) throw std::overflow_error{"k is not large enough"};
			T mod = static_cast<T>((c << k) + 1); // M = c * 2^k + 1
			T alpha = mod::pow(mod)(g, c << (k - logn)), ialpha = mod::inv(mod)(alpha), Ninv = alg::mod::inv(mod)(len); //calculate a_n base on the length
			auto mul = mod::mult(mod); auto mis = mod::minus(mod); auto add = mod::plus(mod); auto check = mod::check_u(mod); //get the mult, minus, plus and check method under mod
			std::transform(a.cbegin(), a.cend(), a.begin(), check); //ensure the value of a inside mod
			std::transform(b.cbegin(), b.cend(), b.begin(), check); //ensure the value of a inside mod
			a.resize(len, 0); b.resize(len, 0); // filling 0 and resize a, b to length
			alg::RotationSumTransform_base2(a.begin(), alpha, logn, 1, 0, mul, mul, mis, add);// NTT on a
			alg::RotationSumTransform_base2(b.begin(), alpha, logn, 1, 0, mul, mul, mis, add);// NTT on b
			std::transform(a.cbegin(), a.cend(), b.cbegin(), a.begin(), [mul](auto a, auto b) {return mul(a, b); });// mult a, b directly
			alg::RotationSumTransform_base2(a.begin(), ialpha, logn, 1, 0, mul, mul, mis, add);// inverse NTT
			std::transform(a.cbegin(), a.cend(), a.begin(), [Ninv, mul](auto c) {return mul(c, Ninv); });// rescale the result
			//removing the using less 0
			if constexpr (remove_zero) {
				auto itnz = a.cbegin();
				for (auto it = itnz, end = a.cend(); it != end; ++it)
					if (*it)itnz = it;
				a.erase(itnz + 1, a.cend());
			}
		}
		/*****************************************************************************
		calculate convolution of a and b emplace (both a and b would be destoried)
		the result would take mod of M
		where M = c * 2^k + 1 should be a prime and g is the primitive root under M

		may calculate convolution directly if size of a and b are small
		*****************************************************************************/
		template<class T = std::uint64_t, bool remove_zero = true, class vec = typename std::vector<T>, typename = std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>>>
		void auto_ntt_convolution(vec& a, vec& b, T g = 5, size_t c = 3, size_t k = 30) {
			auto logn = log2n(a.size() + b.size() - 1);
			size_t len = 1ull << logn;

			if (a.size() * b.size() <= logn * len * 70) {
				vec c;
				c.reserve(a.size() + b.size() - 1);
				convolution_linear(a.cbegin(), a.cend(), b.cbegin(), b.cend(), std::back_insert_iterator(c), static_cast<T>(0));
				a = std::move(c);
			}
			else
				NTT_convolution<T, remove_zero>(a, b, g, c, k);
		}
	}
}