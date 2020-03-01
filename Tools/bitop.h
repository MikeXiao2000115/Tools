#pragma once
#include <algorithm>
#pragma warning( push )
#pragma warning( disable : 4293 )
#pragma warning( disable : 4305 )
#pragma warning( disable : 4309 )
#pragma warning( disable : 26450 )
#pragma warning( disable : 26451 )

namespace tools {
	namespace bitop {
		template<class base, size_t pos, typename = std::enable_if_t<std::is_integral_v<base>>>
		inline constexpr base bitmask = static_cast<base>(1) << pos;

		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr base inline bitat(const size_t& pos) { return static_cast<base>(1) << pos; }

		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr base inline set(const base& num, const size_t& pos, bool one = true) {
			if (one) return num | bitat<base>(pos);
			else return num & ~bitat<base>(pos);
		}
		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr base inline clear(const base& num, const size_t& pos) {
			return set(num, pos, false);
		}
		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		void inline toggle(base& num, size_t pos) {
			num ^= bitat<base>(pos);
		}
		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		bool inline get(const base& num, size_t pos) {
			return num & bitat<base>(pos);
		}

		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		inline constexpr std::make_unsigned_t<base> fullmask = (~static_cast<std::make_unsigned_t<base>>(0));
		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		inline constexpr std::make_unsigned_t<base> highmask = (fullmask<base> << (4 * sizeof(base)));
		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		inline constexpr std::make_unsigned_t<base> lowmask = ~highmask<base>;

		template<class base, size_t N, typename = std::enable_if_t<std::is_integral_v<base>>>
		inline constexpr std::make_unsigned_t<base> cnbitsmask = ~(fullmask<base> << N);
		template<class base, size_t from = 0, size_t to = 0, typename = std::enable_if_t<std::is_integral_v<base>>>
		inline constexpr std::make_unsigned_t<base> cmask = (cnbitsmask<base, from> ^ cnbitsmask<base, to>);

		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		inline constexpr base nbitsmask(const size_t& N) { return ~(fullmask<base> << N); }


		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr base inline sethigh(const base& num, base to) { return (num & lowmask<base>) | ((to & lowmask) << (4 * sizeof(base))); }

		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr base inline setlow(const base& num, base to) { return (num & highmask<base>) | (to & lowmask); }

		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr std::make_unsigned_t<base> inline gethigh(const std::make_unsigned_t<base>& num) { return (num & highmask<std::make_unsigned_t<base>>) >> (4 * sizeof(std::make_unsigned_t<base>)); }

		template<class base, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr base inline getlow(const base& num) { return num & lowmask<base>; }

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

		template<class base, bool one = true, size_t from = static_cast<size_t>(0ull), size_t to = sizeof(base)*8, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr inline size_t lowestbit(const base& num) {
			if constexpr (from == to-1)
				return from;
			else if constexpr(one){
				if ((cmask<base,from, (from + to)/2>&num)!=0)
					return lowestbit<base,one, from, (from + to)/2 > (num);
				else
					return lowestbit<base,one, (from + to)/2, to > (num);
			}
			else {
				if ((cmask<base, from, (from + to) / 2>& (~num)) != 0)
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
				if ((cmask<base, (from + to) / 2, to> & num) != 0)
					return highestbit<base, one, (from + to) / 2, to >(num);
				else
					return highestbit<base, one, from, (from + to) / 2 >(num);

			}
			else {
				if ((cmask<base, (from + to) / 2, to>& (~num)) != 0)
					return highestbit<base, one, (from + to) / 2, to >(num);
				else
					return highestbit<base, one, from, (from + to) / 2 >(num);

			}
		}
		template<class base, size_t n = 0, typename = std::enable_if_t<std::is_integral_v<base>>>
		constexpr inline base reverse(const base& num) {
			if constexpr (n > sizeof(base) * 4)
				return static_cast<base>(0);
			else
				return reverse<base, n + 1>(num) | (get<base>(num, n) ? (static_cast<base>(1) << (bitlength<base>-n - 1)) : 0) | (get<base>(num, bitlength<base>-n - 1) ? (static_cast<base>(1) << n) : 0);
		}
	}

}
#pragma warning( pop )