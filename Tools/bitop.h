#pragma once
#include <algorithm>
#include <cstdint>
#pragma warning( push )
#pragma warning( disable : 4293 )
#pragma warning( disable : 4305 )
#pragma warning( disable : 4309 )
#pragma warning( disable : 26450 )
#pragma warning( disable : 26451 )

namespace tools {
	namespace bitop {
		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr base inline abitat(const size_t& pos, bool one = true) { return pos >= 8 * sizeof(base) ? 0 : (one ? (static_cast<base>(1) << pos) : static_cast<base>(0)); }

		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr base inline set(const base& num, const size_t& pos, bool one = true) {
			if (one) return num | abitat<base>(pos);
			else return num & ~abitat<base>(pos);
		}
		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr base inline clear(const base& num, const size_t& pos) {
			return set(num, pos, false);
		}
		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		void inline toggle(base& num, size_t pos) {
			num ^= abitat<base>(pos);
		}
		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		bool inline get(const base& num, size_t pos) {
			return num & abitat<base>(pos);
		}

		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		inline constexpr std::make_unsigned_t<base> fullmask = (~static_cast<std::make_unsigned_t<base>>(0));
		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		inline constexpr std::make_unsigned_t<base> highmask = (fullmask<base> << (4 * sizeof(base)));
		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		inline constexpr std::make_unsigned_t<base> lowmask = ~highmask<base>;

		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		inline constexpr base nbitsmask(const size_t& N) { return (N >= 8 * sizeof(base)) ? fullmask<base> : ~(fullmask<base> << N); }
		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		inline constexpr std::make_unsigned_t<base> mask(const size_t& a, const size_t& b){ return nbitsmask<base>(a) ^ nbitsmask<base>(b); }

		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr base inline sethigh(const base& num, const base& to) { return (num & lowmask<base>) | ((to & lowmask) << (4 * sizeof(base))); }
		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr base inline setlow(const base& num, const base& to) { return (num & highmask<base>) | (to & lowmask); }

		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr std::make_unsigned_t<base> inline gethigh(const std::make_unsigned_t<base>& num) { return (num & highmask<std::make_unsigned_t<base>>) >> (4 * sizeof(std::make_unsigned_t<base>)); }
		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr base inline getlow(const base& num) { return num & lowmask<base>; }
		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr base inline get(const base& num, const size_t& a, const size_t& b) { return (num & mask<base>(a, b)) >> std::min(a, b); }

		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		inline constexpr base bitlength = sizeof(base) << 3;

		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		inline std::string to_string(base val, bool showzero = true, bool sep = false) {
			std::string str = "";
			for (int i = bitlength<base>-1; i >= 0; --i)
				str += ((get<base>(val, i) == 1) ? std::string("1") : (showzero ? std::string("0") : std::string(" "))) + ((sep&&i%4==0)?std::string(" "): std::string(""));
			return str;
		}
		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr void inline fulladder(base& num, const base& add, bool& up) {
			if (up && !(++num)) num = add;
			else up = ((num += add) < add);
		}
		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr inline std::pair<base, base> multunit(const base& a, const base& b) {
			base tmp = bitop::gethigh<base>(b) * bitop::getlow<base>(a);
			base low = bitop::getlow<base>(a) * bitop::getlow<base>(b);
			base high = bitop::gethigh<base>(a) * bitop::gethigh<base>(b);

			bool up = false;
			fulladder<base>(tmp, bitop::gethigh<base>(a) * bitop::getlow<base>(b), up);
			if (up) high += static_cast<base>(1) << (4 * sizeof(base));
			up = false;
			fulladder<base>(low, bitop::getlow<base>(tmp) << (4 * sizeof(base)), up);
			fulladder<base>(high, bitop::gethigh<base>(tmp), up);
			return {high,low};
		}

		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		inline constexpr base set(const base& num, const base& to, const size_t& a, const size_t& b) {
			auto m = std::min(a, b);
			if (m > sizeof(base) * 8)
				return num;
			else
				return ((to & nbitsmask<base>(std::max(a, b) - m)) << m) | (num & ~mask<base>(a, b));
		}

		template<class base, bool one = true, size_t from = static_cast<size_t>(0ull), size_t to = sizeof(base)*8, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr inline size_t lowestbit(const base& num) {
			if constexpr (from == to - 1)
				return from;
			else if constexpr(one){
				if ((mask<base>(from, (from + to)/2)&num)!=0)
					return lowestbit<base,one, from, (from + to)/2 > (num);
				else
					return lowestbit<base,one, (from + to)/2, to > (num);
			}
			else {
				if ((mask<base>(from, (from + to) / 2)& (~num)) != 0)
					return lowestbit<base,one, from, (from + to) / 2 >(num);
				else
					return lowestbit<base,one, (from + to) / 2, to >(num);
			}
		}
		template<class base, bool one = true, size_t from = static_cast<size_t>(0ull), size_t to = sizeof(base) * 8, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr inline size_t highestbit(const base& num) {
			if constexpr (from == to - 1)
				return from;
			else if constexpr(one) {
				if ((mask<base>((from + to) / 2, to)& num) != 0)
					return highestbit<base, one, (from + to) / 2, to >(num);
				else
					return highestbit<base, one, from, (from + to) / 2 >(num);

			}
			else {
				if ((mask<base>((from + to) / 2, to)& (~num)) != 0)
					return highestbit<base, one, (from + to) / 2, to >(num);
				else
					return highestbit<base, one, from, (from + to) / 2 >(num);

			}
		}

		template<class base, size_t i = 0, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr inline base reverse(const base& num) {
			static const char tab[] = {	0x0, 0x8, 0x4, 0xc,	0x2, 0xa, 0x6, 0xe,	0x1, 0x9, 0x5, 0xd,	0x3, 0xb, 0x7, 0xf};
			if constexpr (i >= sizeof(base) * 4)
				return static_cast<base>(0);
			else {
				auto b = sizeof(base) * 8 - i - 4;
				return set(set((base)reverse<base, i + 4>(num), (base)tab[get(num, i, i + 4)], b, b + 4), (base)tab[get(num, b, b + 4)], i, i + 4);
			}
		}

		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr inline base reverse(const base& num, const size_t& size, size_t i = 0) {
			static const uint8_t tab4b[] = {0x0, 0x8, 0x4, 0xc,	0x2, 0xa, 0x6, 0xe,	0x1, 0x9, 0x5, 0xd,	0x3, 0xb, 0x7, 0xf};
			static const uint8_t tab2b[] = {0x0, 0x2, 0x1, 0x3};
			base res = 0;
			while (2 * i < size) {
				if ((i << 1) + 8 <= size) {
					auto j = size - i - 4;
					res=set(res, static_cast<base>(tab4b[get(num, i, i + 4)]), j, j + 4);
					res=set(res, static_cast<base>(tab4b[get(num, j, j + 4)]), i, i + 4);
					i += 4;
				}
				else if ((i << 1) + 4 <= size) {
					auto j = size - i - 2;
					res=set(res, static_cast<base>(tab2b[get(num, i, i + 2)]), j, j + 2);
					res=set(res, static_cast<base>(tab2b[get(num, j, j + 2)]), i, i + 2);
					i += 2;
				}
				else {
					auto j = size - i - 1;
					res = set(res, j, get(num, i));
					res = set(res, i, get(num, j));
					++i;
				}
			}
			return res;
		}
		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr inline bool quickreverse(base& num, const size_t& size, size_t i = 0) {
			static const uint8_t tab4b[] = {0x0, 0x8, 0x4, 0xc,	0x2, 0xa, 0x6, 0xe,	0x1, 0x9, 0x5, 0xd,	0x3, 0xb, 0x7, 0xf};
			static const uint8_t tab2b[] = {0x0, 0x2, 0x1, 0x3};
			base res = 0;
			while (2 * i < size) {
				if ((i << 1) + 8 <= size) {
					auto j = size - i - 4;
					res = set(res, static_cast<base>(tab4b[get(num, i, i + 4)]), j, j + 4);
					res = set(res, static_cast<base>(tab4b[get(num, j, j + 4)]), i, i + 4);
					i += 4;
				}
				else if ((i << 1) + 4 <= size) {
					auto j = size - i - 2;
					res = set(res, static_cast<base>(tab2b[get(num, i, i + 2)]), j, j + 2);
					res = set(res, static_cast<base>(tab2b[get(num, j, j + 2)]), i, i + 2);
					i += 2;
				}
				else {
					auto j = size - i - 1;
					res = set(res, j, get(num, i));
					res = set(res, i, get(num, j));
					++i;
				}
				if (res < num) return false;
			}
			num = res;
			return true;
		}

	}

}
#pragma warning( pop )