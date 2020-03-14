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
