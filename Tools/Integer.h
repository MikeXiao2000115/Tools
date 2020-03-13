#pragma once
#include <algorithm>
#include <vector>
#include <string>
#include "alg.h"
#include "bitop.h"

#pragma warning( push )
#pragma warning( disable : 4305 )
#pragma warning( disable : 4309 )
#pragma warning( disable : 26450 )
#pragma warning( disable : 26451 )
namespace tools {
	class Int {
	public:
		using value_type = unsigned char;
	protected:
		std::vector<value_type> data;
		bool hiOne;
	private:
		Int(size_t count, value_type val) :hiOne(false), data(count, val) { }
	public:
		Int()noexcept:hiOne(false){}
		Int(const Int& other) noexcept : data(other.data), hiOne(other.hiOne) { }
		Int(Int&& other) noexcept : data(std::move(other.data)), hiOne(other.hiOne) { }
		Int(value_type val) :hiOne(false) { if (val != 0) data.push_back(val); }
		Int(std::make_signed_t<value_type> val) :hiOne(val < 0) { if (hiOne && val != bitop::fullmask<value_type>) data.push_back(static_cast<value_type>(val)); else if (!hiOne && val) data.push_back(val); }

		Int(short val) :Int(static_cast<std::make_signed_t<value_type>>(val)) {}
		Int(unsigned short val) :Int(static_cast<value_type>(val)) {}
		Int(int val):Int(static_cast<std::make_signed_t<value_type>>(val)){}
		Int(unsigned int val) :Int(static_cast<value_type>(val)) {}

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
		bool shrink() {
			auto end = data.crend(), begin = data.crbegin();
			if (begin == end) return false;
			auto cmp = hiOne ? bitop::fullmask<value_type> : 0;
			auto it = begin;
			while (it != end && *it == cmp)
				++it;
			if (it != begin) {
				data.erase(it.base(), begin.base());
				return true;
			}
			else
				return false;
		}
	private:
		bool addOne() {
			for (auto it = data.begin(), end = data.end(); it != end; ++it)
				if (*it == bitop::fullmask<value_type>)	*it = 0;
				else {
					++(*it);
					if (hiOne && ++it == end) return shrink();
					return false;
				}
			if (hiOne) {
				hiOne = false;
				return shrink();
			}
			else {
				data.push_back(static_cast<value_type>(1));
				return true;
			}
		}
		bool subOne() {
			for (auto it = data.begin(), end = data.end(); it != end; ++it)
				if (*it == 0)	*it = bitop::fullmask<value_type>;
				else {
					--(*it);
					if (!hiOne && ++it == end) return shrink();
					return false;
				}
			if (!hiOne) {
				hiOne = true;
				return shrink();
			}
			else {
				data.push_back(~static_cast<value_type>(1));
				return true;
			}
		}
		static int compare(const Int& L, const Int& R) {// L>R:1 L=R:0 L<R:-1
			auto signedbool = [](bool b) {return b ? 1 : -1; };
			if (L.hiOne != R.hiOne) return signedbool(R.hiOne); //L.hiOne <-> !R.hiOne(-1)  !L.hiOne <-> R.hiOne(1)
			if (L.data.size() != R.data.size()) return L.hiOne ? signedbool(L.data.size() < R.data.size()) : signedbool(L.data.size() > R.data.size());
			for (auto l_it = L.data.crbegin(), r_it = R.data.crbegin(), end = L.data.crend(); l_it != end; ++l_it, ++r_it) {
				if (*l_it < *r_it) return -1;
				if (*l_it > * r_it) return 1;
			}
			return 0;
		}
		void reverse() {
			hiOne = !hiOne;
			for (auto& i : data)
				i = ~i;
		}
		Int& neg() {
			if (!hiOne) {
				reverse();
				addOne();
			}
			else {
				subOne();
				reverse();
			}
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
		Int operator~()const  {
			auto tmp = *this;
			tmp.reverse();
			return tmp;
		}
		Int& operator|=(const Int& other) {
			auto it = data.begin(), end = data.end();
			auto oit = other.data.cbegin(), oend = other.data.cend();
			while (it != end && oit != oend)
				*(it++) |= *(oit++);

			if (!hiOne && it == end) data.insert(end, oit, oend);
			if (other.hiOne && oit == oend) data.erase(it, end);

			hiOne = hiOne || other.hiOne;
			shrink();
			return *this;
		}
		Int& operator&=(const Int& other) {
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
				/*
				Int tmp;
				auto m = L.data.size();
				auto n = S.data.size();
				tmp.data.reserve(m + n);
				tmp.data.push_back(0);
				size_t base = 0, mul = 0;
				for (const auto& a : S.data) {
					mul = base;
					auto it = tmp.data.begin() + base++, end = tmp.data.end();
					for (const auto& b : L.data) {
						auto [h, l] = multunit(a, b);
						bool up = ((*it++ += l) < l);
						++mul;
						if (it == end) { it = tmp.data.insert(it, 0); end = tmp.data.end(); }
						if (up && !(++ *it)) *it = h;
						else up = ((*it += h) < h);
						if (up && tmp.addOne(it + 1)) {
							it = tmp.data.begin() + mul; end = tmp.data.end();
						}
					}
				}
				*/
				tmp.shrink();
				return tmp;
			}
			static std::pair<Int, Int> division(Int&& num, Int&& b) {//num = q*b+r
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
			}
	public:
		//two op
		Int& operator+=(const Int& other) {
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
						if (*it == 0)	*it = bitop::fullmask<value_type>;
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
				if (!up) data.push_back(bitop::fullmask<value_type> ^ static_cast<value_type>(1));
				hiOne = true;
			}
			else if (hiOne || other.hiOne) {
				if (up) hiOne = false;
				else hiOne = true;
			}
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
}
#pragma warning( pop )