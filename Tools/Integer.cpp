#include "pch.h"
#include "framework.h"

#include "Integer.h"

bool tools::Int::shrink() {
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

bool tools::Int::addOne() {
	for (auto it = data.begin(), end = data.end(); it != end; ++it)
		if (*it == bitop::fullmask<value_type>)	*it = 0;
		else {
			++(*it);
			if (hiOne && ++it == end) return shrink();
			else return false;
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

bool tools::Int::subOne() {
	for (auto it = data.begin(), end = data.end(); it != end; ++it)
		if (*it == 0)	*it = bitop::fullmask<value_type>;
		else {
			--(*it);
			if (!hiOne && ++it == end) return shrink();
			else return false;
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

int tools::Int::compare(const Int& L, const Int& R) {
	auto signedbool = [](bool b) {return b ? 1 : -1; };
	if (L.hiOne != R.hiOne) return signedbool(R.hiOne); //L.hiOne <-> !R.hiOne(-1)  !L.hiOne <-> R.hiOne(1)
	if (L.data.size() != R.data.size()) return L.hiOne ? signedbool(L.data.size() < R.data.size()) : signedbool(L.data.size() > R.data.size());
	for (auto l_it = L.data.crbegin(), r_it = R.data.crbegin(), end = L.data.crend(); l_it != end; ++l_it, ++r_it) {
		if (*l_it < *r_it) return -1;
		if (*l_it > * r_it) return 1;
	}
	return 0;
}

tools::Int& tools::Int::operator|=(const Int& other) {
	if (&other == this) return *this;//solve the problem of self-ref
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

tools::Int& tools::Int::operator&=(const Int& other) {
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

tools::Int& tools::Int::operator^=(const Int& other) {
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

tools::Int& tools::Int::operator<<=(const size_t& shift) {
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

tools::Int& tools::Int::operator>>=(const size_t& shift) {
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

tools::Int tools::Int::mult(const Int& S, const Int& L) {
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

std::pair<tools::Int, tools::Int> tools::Int::division(Int&& num, Int&& b) {//num = q*b+r
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

tools::Int& tools::Int::operator+=(const Int& other) {
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