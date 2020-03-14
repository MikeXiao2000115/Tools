#pragma once
#include <iostream>
#include <random>

#include <vector>

#include <algorithm>
#include <iterator>
#include <cstdint>
#include <string>
#include "alg.h"
#include "bitop.h"

namespace tools {
	class Int {
	public:
		using value_type = std::uint8_t;
		static_assert(std::is_integral_v<value_type>, "base type of Int should be integral type");
		static_assert(std::is_unsigned_v<value_type>, "base type of Int should be unsigned type");
	protected:
		std::vector<value_type> data;
		bool hiOne;
	private:
		Int(size_t count, value_type val) :hiOne(false), data(count, val) { }
	public:
		Int()noexcept:hiOne(false){}
		Int(const Int& other) noexcept : data(other.data), hiOne(other.hiOne) { }
		Int(Int&& other) noexcept : data(std::move(other.data)), hiOne(other.hiOne) { }
		//Warning: the value larger than the value_type would case cut off
		template<class T, typename = std::enable_if_t<std::is_convertible_v<T, value_type> && std::is_integral_v<T>>>
		Int(const T& val) : hiOne(val < 0) {
			if constexpr (std::is_signed_v<T>) {
				if (hiOne && static_cast<std::make_signed_t<value_type>>(val) != bitop::fullmask<value_type>)
					data.push_back(static_cast<value_type>(val));
				else if (!hiOne && val)
					data.push_back(static_cast<value_type>(val));
			}
			else
				if (static_cast<value_type>(val) != 0) data.push_back(static_cast<value_type>(val));
		}

		Int(std::initializer_list<value_type> ilist, bool neg = false) :hiOne(neg), data(std::move(ilist)) { shrink(); }
		template <class InputIt, class = alg::enable_if_InputItr<InputIt>>
		Int(InputIt first, InputIt last, bool neg=false) : hiOne(neg), data(std::move(first), std::move(last)) { shrink(); }

		Int& operator=(const Int& other) {
			hiOne = other.hiOne;
			data = other.data;
			return *this;
		}
		Int& operator=(Int&& other)noexcept {
			hiOne = std::move(other.hiOne);
			data = std::move(other.data);
			return *this;
		}
	protected:
		bool shrink(); //return true if the data size had updated 
	private:
		bool addOne(); //return true if the data size had updated 
		bool subOne(); //return true if the data size had updated 
		static int compare(const Int&, const Int&); // L>R:1 L=R:0 L<R:-1
		
		void reverse() {
			hiOne = !hiOne;
			for (auto& i : data)
				i = ~i;
		}
		Int& neg() {
			if (!hiOne) { reverse(); addOne(); }
			else { subOne(); reverse(); }
			return *this;
		}
	public:
		//bits op
		std::string to_bstring(size_t width = 0, bool zero = true)const {
			std::string str = hiOne ? "1...111 | " : "0...000 | ";
			for (auto i = data.size(); i < width; ++i)
				str += (hiOne ? "(" + std::string(sizeof(value_type) * 8 - 2, '1') + ")" : (zero ? "(" + std::string(sizeof(value_type) * 8 - 2, '0') + ")" : std::string(sizeof(value_type) * 8, ' '))) + " ";
			for (auto it = data.crbegin(), end = data.crend(); it != end; ++it)
				str += bitop::to_string<value_type>(*it, zero) + " ";
			return str;
		}
		bool zeroQ()const { return !hiOne && data.empty(); }
		Int operator~()const  {
			auto tmp = *this;
			tmp.reverse();
			return tmp;
		}
		size_t bitlength() {
			if (data.empty()) return 0;
			auto tmp = data.back();
			auto h = bitop::highestbit(hiOne ? (~tmp) : tmp);
			return (data.size() - 1) * bitop::bitlength<value_type>+bitop::highestbit<value_type>(hiOne ? (~tmp) : tmp) + 1;
		}
		Int& operator|=(const Int& other);
		Int& operator&=(const Int& other) {
			if (&other == this) return *this;//solve the problem of self-ref
			auto it = data.begin(), end = data.end();
			auto oit = other.data.cbegin(), oend = other.data.cend();
			while (it != end && oit != oend)
				*(it++) &= *(oit++);

			if (hiOne && it == end) data.insert(end, oit, oend);
			if (!other.hiOne && oit == oend) data.erase(it, end);

			hiOne = hiOne && other.hiOne;
			shrink();
			return *this;
		}
		Int& operator^=(const Int& other) {
			if (&other == this) { hiOne = false; data.clear(); return *this; }//solve the problem of self-ref
			auto it = data.begin(), end = data.end();
			auto oit = other.data.cbegin(), oend = other.data.cend();
			while (it != end && oit != oend)
				*(it++) ^= *(oit++);

			if (it == end) {
				it = data.insert(end, oit, oend);
				if (hiOne) {
					end = data.end(); 
					while (it != end) {
						*(it) = ~*(it);
						++it;
					}
				}
			}
			else if (other.hiOne) while (it != end) {
				*(it) = ~*(it);
				++it;
			}

			hiOne = (hiOne != other.hiOne);//(hiOne && !other.hiOne) || (!hiOne && other.hiOne);
			shrink();
			return *this;
		}
		Int& operator<<=(const size_t& shift) {
			if (shift) {
				auto S = (shift >> 3) / sizeof(value_type);
				auto s = shift % (sizeof(value_type) * 8);
				if (s) {
					data.insert(data.cbegin(), S + 1, 0);
					return this->operator>>=(sizeof(value_type) * 8 - s);
				}
				else {
					data.insert(data.cbegin(), S, 0);
					return *this;
				}
			}
			else return *this;
		}
		Int& operator>>=(const size_t& shift) {
			if (data.empty()) return *this;
			auto S = (shift >> 3) / sizeof(value_type);
			if (S >= data.size()) {
				data.clear();
				return *this;
			}
			data.erase(data.cbegin(), data.cbegin() + S);
			auto s_ = shift % (sizeof(value_type) * 8);
			if (s_) {
				auto ns_ = sizeof(value_type) * 8 - s_;
				auto hmask = bitop::fullmask<value_type> << s_;
				auto lmask = ~hmask;
				auto it = data.begin();
				for (auto nit = data.cbegin(), end = data.cend(); (++nit) != end; ++it)
					*it = ((hmask & *it) >> s_) | ((lmask & *nit) << ns_);
				*it = ((hmask & *it) >> s_);
				if (hiOne) *it |= lmask << ns_;
				//data.pop_back();
				shrink();
			}
			return *this;
		}

		Int operator|(const Int& other)const { auto tmp = *this; return std::move(tmp |= other); }
		Int operator&(const Int& other)const { auto tmp = *this; return std::move(tmp &= other); }
		Int operator^(const Int& other)const { auto tmp = *this; return std::move(tmp ^= other); }
		Int operator<<(const size_t& shift)const { auto tmp = *this; return std::move(tmp <<= shift); }
		Int operator>>(const size_t& shift)const { auto tmp = *this; return std::move(tmp >>= shift); }

		//one op
		Int& operator++() {
			addOne();
			return *this;
		}
		Int operator++(int) {
			auto tmp = *this;
			this->operator++();
			return tmp;
		}
		Int& operator--() {
			subOne();
			return *this;
		}
		Int operator--(int) {
			auto tmp = *this;
			this->operator--();
			return tmp;
		}
		Int operator-()const  {
			auto tmp = *this;
			return tmp.neg();
		}
	private:
			static Int inline _mult(const Int& L, const Int& R) { if (L.data.size() < R.data.size()) return mult(L, R); else return mult(R, L); }
			static Int mult(const Int& S, const Int& L) {
				Int tmp(L.data.size() + S.data.size(), 0);
				auto base_it = tmp.data.begin();
				for (const auto& a : S.data) {
					auto it = base_it++;
					for (const auto& b : L.data) {
						auto [h, l] = bitop::multunit<value_type>(a, b);
						bool up = ((*(it++) += l) < l);  //adding l to current it and check overflow
						if (up && !(++(*it))) *it = h; //if overflow and adding carry cause an other overflow
						else up = ((*it += h) < h);
						if (up) for (auto i = it + 1; true; ++i)
							if (*i == bitop::fullmask<value_type>)	*i = 0;
							else { ++(*i); break; }
					}
				}
				tmp.shrink();
				return tmp;
			}
			static std::pair<Int, Int> division(Int&& num, Int&& b) {//num = q*b+r
				/**
				if (num.data.size() < b.data.size()) return {0,num};
				size_t dif = (num.data.size() - b.data.size() + 2) * bitop::bitlength<value_type>;
				//b <<= dif;
				//auto nb = -b;
				Int q = 0;
				while (dif) {
					--dif;
					auto tmp = b << dif;
					if (tmp <= num) {
						num -=tmp;
						q |= (Int(1) <<= dif);
					}
				}
				q.shrink();
				return {q,num};
				/**/
				auto numlen = num.bitlength(), blen = b.bitlength();
				if (blen > numlen) return {0,num};
				size_t dif = numlen - blen;
				b <<= dif;
				auto nb = -b;
				Int q = 0;
				while (dif) {
					if (b <= num) {
						num += nb;
						q |= (Int(1) <<= dif);
					}
					b >>= 1;
					nb >>= 1;
					--dif;
				}
				q.shrink();
				return {q,num};
			}
	public:
		//two op
		Int& operator+=(const Int& other) {
			if (&other == this) { auto tmp = *this;	return this->operator+=(tmp); }//solve the problem of self-ref
			auto it = data.begin(), end = data.end();
			auto oit = other.data.cbegin(), oend = other.data.cend();
			bool up = false, hi;
			while (it != end && oit != oend)
				bitop::fulladder<value_type>(*it++, *oit++, up);
			if (it == end) {
				it = data.insert(it, oit, oend);
				end = data.end();
				hi = hiOne;
			}
			else
				hi = other.hiOne;
			
			if (hi) {
				if (!up) {
					up = true;
					for (; it != end; ++it)
						if (*it == 0) *it = bitop::fullmask<value_type>;
						else { --(*it); break; }
					if (it == end) up = false;
				}
			}
			else if (up) {
				for (; it != end; ++it)
					if (*it == bitop::fullmask<value_type>)	*it = 0;
					else { ++(*it); break; }
				if (it != end) up = false;
			}

			if (hiOne && other.hiOne) {
				if (!up) data.push_back(~static_cast<value_type>(1));
				hiOne = true;
			}
			else if (hiOne || other.hiOne)
				hiOne = !up;
			else {
				if (up) data.push_back(1);
				hiOne = false;
			}
			shrink();
			return *this;
		}
		Int& operator-=(const Int& other) { return this->operator+=(-other); }
		Int& operator*=(const Int& other) { return *this = *this * other; }
		Int& operator/=(const Int& other) { return *this = *this / other; }
		Int& operator%=(const Int& other) { return *this = *this % other; }

		friend inline Int operator+(const Int&, const Int&);
		friend inline Int operator-(const Int&, const Int&);
		friend inline Int operator*(const Int&, const Int&);
		friend inline Int operator*(const Int&, Int&&);
		friend inline Int operator*(Int&&, const Int&);
		friend inline Int operator*(Int&&, Int&&);
		friend inline Int operator/(const Int&, const Int&);
		friend inline Int operator%(const Int&, const Int&);

		friend inline std::pair<Int,Int> div(Int, Int);

		//cmp op
		friend inline bool operator==(const Int&, const Int&);
		friend inline bool operator!=(const Int&, const Int&);
		friend inline bool operator>=(const Int&, const Int&);
		friend inline bool operator<=(const Int&, const Int&);
		friend inline bool operator>(const Int&, const Int&);
		friend inline bool operator<(const Int&, const Int&);

		void print() {
			if (*this == 0) {
				std::cout << "{0}" << std::endl;
				return;
			}
			if (*this == -1) {
				std::cout << "-{1}" << std::endl;
				return;
			}
			auto tmp = *this;
			if (*this < 0) {
				tmp = (-tmp);
				std::cout << " - ";
			}
			std::cout << "{";
			bool first = true;
			for (const auto& i : tmp.data)
				if (first) {
					std::cout << static_cast<unsigned long long>(i);
					first = false;
				}
				else
					std::cout << ", " << static_cast<unsigned long long>(i);
			std::cout << "}" << std::endl;
		}
		template< class Generator >
		friend Int randomInt(Generator&, size_t, bool);
	};
	Int tools::operator+(const Int& L, const Int& R) { if (L.data.size() > R.data.size()) { auto tmp = L; return tmp += R; } else { auto tmp = R; return tmp += L; } }
	Int tools::operator-(const Int& L, const Int& R) { auto tmp = L; return tmp -= R; }
	Int tools::operator*(const Int& L, const Int& R) {
		if (L > 0) {
			if (R > 0) return Int::_mult(L, R);
			else return Int::_mult(L, -R).neg();
		}
		else {
			if (R > 0) return Int::_mult(-L, R).neg();
			else return Int::_mult(-L, -R);
		}
	}
	Int tools::operator*(const Int& L, Int&& R) {
		if (L > 0) {
			if (R > 0) return Int::_mult(L, R);
			else return Int::_mult(L, R.neg()).neg();
		}
		else {
			if (R > 0) return Int::_mult(-L, R).neg();
			else return Int::_mult(-L, R.neg());
		}
	}
	Int tools::operator*(Int&& L, const Int& R) {
		if (L > 0) {
			if (R > 0) return Int::_mult(L, R);
			else return Int::_mult(L, -R).neg();
		}
		else {
			if (R > 0) return Int::_mult(L.neg(), R).neg();
			else return Int::_mult(L.neg(), -R);
		}
	}
	Int tools::operator*(Int&& L, Int&& R) {
		if (L > 0) {
			if (R > 0) return Int::_mult(L, R);
			else return Int::_mult(L, R.neg()).neg();
		}
		else {
			if (R > 0) return Int::_mult(L.neg(), R).neg();
			else return Int::_mult(L.neg(), R.neg());
		}
	}

	std::pair<Int, Int> tools::div(Int num, Int div) {
		if (div == 0) throw std::domain_error("divide by zero");
		if (num >= 0) {
			if (div > 0) {
				return Int::division(std::move(num), std::move(div));
			}
			else {
				auto [q,r]= Int::division(std::move(num), std::move(div.neg()));
				return {q.neg(),r};
			}
		}
		else {
			if (div > 0) {
				auto [q, r] = Int::division(std::move(num.neg()), std::move(div));
				return {q.neg(),r.neg()};
			}
			else {
				auto [q, r] = Int::division(std::move(num.neg()), std::move(div.neg()));
				return {q,r.neg()};
			}

		}
	}
	Int tools::operator/(const Int& num, const Int& d) {
		auto [q, r] = div(Int(num), Int(d));
		return q;
	}
	Int tools::operator%(const Int& num, const Int& d) {
		auto [q, r] = div(Int(num), Int(d));
		return r;
	}
	inline bool tools::operator==(const Int& L, const Int& R) { return Int::compare(L, R) == 0; }
	inline bool tools::operator!=(const Int& L, const Int& R) { return Int::compare(L, R) != 0; }
	inline bool tools::operator>=(const Int& L, const Int& R) { return Int::compare(L, R) >= 0; }
	inline bool tools::operator<=(const Int& L, const Int& R) { return Int::compare(L, R) <= 0; }
	inline bool tools::operator>(const Int& L, const Int& R) {	return Int::compare(L, R) > 0; }
	inline bool tools::operator<(const Int& L, const Int& R) { return Int::compare(L, R) < 0; }

	template<class Generator>
	Int randomInt(Generator& g, size_t max_n = 3, bool neg = true) {
		std::uniform_int_distribution<unsigned long long> dis;
		Int res;
		size_t len = std::uniform_int_distribution<size_t>{0,max_n}(g);
		std::generate_n(std::back_insert_iterator(res.data), len, [&]() {return static_cast<Int::value_type>(dis(g)); });
		if (neg) {
			std::bernoulli_distribution d;
			res.hiOne = d(g);
		}
		else
			res.hiOne = false;
		res.shrink();
		return res;
	}

}
