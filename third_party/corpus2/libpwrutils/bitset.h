/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libcorpus2 project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENCE, COPYING.LESSER and COPYING files for more details.
*/

#ifndef PWRNLP_BITSET_H
#define PWRNLP_BITSET_H

#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <bitset>
#include <boost/functional/hash.hpp>
#include <boost/pending/lowest_bit.hpp>
#include <climits>

#ifdef _MSC_VER
#include <intrin.h>
#pragma intrinsic(_BitScanForward)
#endif

namespace PwrNlp {

using std::bitset;

static const size_t ulong_bits = sizeof(unsigned long) * CHAR_BIT;

typedef bitset<ulong_bits> word_bitset;

template<size_t S>
std::bitset<S> filled_bitset()
{
	std::bitset<S> b = std::bitset<S>();
	b.flip();
	return b;
}

/**
 * Count set bits in a integral type.
 * http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
 */
template <typename T> inline
int count_bits_set(T v)
{
	v = v - ((v >> 1) & (T)~(T)0/3);                              // temp
	v = (v & (T)~(T)0/15*3) + ((v >> 2) & (T)~(T)0/15*3);         // temp
	v = (v + (v >> 4)) & (T)~(T)0/255*15;                         // temp
	return (T)(v * ((T)~(T)0/255)) >> (sizeof(T) - 1) * CHAR_BIT; // count
}

template <size_t S> inline
size_t count_bits_set(const std::bitset<S>& b)
{
	return b.count();
}

/**
 * Get index of lowest set bit in an integral type
 */
inline size_t lowest_bit(const unsigned long long& t)
{
	if (t <= 0) return static_cast<size_t>(-1);
	return boost::lowest_bit(t);
}

inline size_t lowest_bit(const unsigned long& t)
{
#ifndef _MSC_VER
	if (t <= 0) return static_cast<size_t>(-1);
	return boost::lowest_bit(t);
#else
	unsigned long index = 0;
	if(_BitScanForward(&index, t)) return index;
	return static_cast<size_t>(-1);
#endif
}

template <size_t S> inline
size_t lowest_bit(const bitset<S>& b)
{
#ifdef __GNUC__
	return b._Find_first();
#elif _MSC_VER
	for(size_t w = 0; w <= S / ulong_bits; ++w) {
		unsigned long index = 0;
		if(_BitScanForward(&index, b._Getword(w))) {
			return index + w * PwrNlp::ulong_bits;
		}
	}
	return static_cast<size_t>(-1);
#else
	if(b.none()) return static_cast<size_t>(-1);
	
	const bitset<S> mask(std::numeric_limits<unsigned long>::max());
	bitset<S> c(b);
	unsigned long offset = 0;
	unsigned long ul = (c & mask).to_ulong(); 
	while(ul == 0) {
		c >>= PwrNlp::ulong_bits;
		offset += PwrNlp::ulong_bits;
		ul = (c & mask).to_ulong(); 
	}
	return lowest_bit(ul) + offset;
#endif
}

template<> inline
size_t lowest_bit(const word_bitset& b)
{
	return lowest_bit(b.to_ulong());
}

/// Helper iterator class for iterating through set bits
template<typename T>
struct set_bits_iterator
{
	typedef T value_type;
	typedef std::forward_iterator_tag iterator_category;
	typedef int difference_type;
	typedef const T *pointer;
	typedef const T &reference;
	set_bits_iterator(const set_bits_iterator &i): i_(i.i_), c_(i.c_) {}
	set_bits_iterator(const T& i) : i_(i), c_(0) {
		adv();
	}

	set_bits_iterator &operator++() {
		adv(); return *this;
	}
	set_bits_iterator operator++(int) {
		set_bits_iterator c(*this);
		c.adv();
		return c;
	}
	bool operator==(const set_bits_iterator &i) const {
		return i_ == i.i_ && c_ == i.c_;
	}
	bool operator!=(const set_bits_iterator &i) const {
		return i_ != i.i_ || c_ != i.c_;
	}
	const T &operator*() const { return c_; }

private:
	void adv() {
		c_.reset();
		if (i_.any()) {
			c_.set(lowest_bit(i_));
			i_ ^= c_;
		}
	}

	T i_;
	T c_;
};

/**
 * Function that returns a foreach-compatible iterator range that allows
 * iterating through the set bits of a bitset. It only makes sense to read
 * from the returned range.
 *
 * Example usage: \code
 * BOOST_FOREACH(const bitset<32>& b, my_bitset) {
 *    foo_with(b);
 * }
 * \endcode
 */
template<size_t S>
boost::iterator_range< set_bits_iterator< std::bitset<S> > > set_bits(
		const bitset<S>& bs)
{
	return boost::iterator_range< set_bits_iterator< std::bitset<S> > >(
			bs, bitset<S>()
		);
}

} /* end ns PwrNlp */

namespace std {

template<size_t S> inline
size_t hash_value(bitset<S> b)
{
	size_t seed = 0;
	const bitset<S> mask(std::numeric_limits<unsigned long>::max());
	while (b.any()) {
		boost::hash_combine(seed, (b & mask).to_ulong());
		b >>= PwrNlp::ulong_bits;
	}
	return seed;
}

template<> inline
size_t hash_value(bitset<PwrNlp::ulong_bits> b)
{
	size_t seed = 0;
	boost::hash_combine(seed, b.to_ulong());
	return seed;
}

template<size_t S> inline
bool operator<(bitset<S> left, bitset<S> right)
{
	const bitset<S> mask(std::numeric_limits<unsigned long>::max());
	while (left.any()) {
		unsigned long l1 = (left & mask).to_ulong();
		unsigned long r1 = (right & mask).to_ulong();
		if (l1 < r1) {
			return true;
		} else if (l1 > r1) {
			return false;
		}
		left >>= PwrNlp::ulong_bits;
		right >>= PwrNlp::ulong_bits;
	}
	return right.any();
}

template<> inline
bool operator<(bitset<PwrNlp::ulong_bits> left, bitset<PwrNlp::ulong_bits> right)
{
	return left.to_ulong() < right.to_ulong();
}

} /* end ns std */

#endif // PWRNLP_BITSET_H
