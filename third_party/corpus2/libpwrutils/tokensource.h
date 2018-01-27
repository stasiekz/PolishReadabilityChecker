/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENCE, COPYING.LESSER and COPYING files for more details.
*/

#ifndef PWRNLP_TOKENSOURCE_H
#define PWRNLP_TOKENSOURCE_H

#include <boost/function.hpp>
#include <boost/range.hpp>

#include <iosfwd>
#include <vector>

namespace PwrNlp {

/**
 * A simple interface for classes that can be used as token sources, for
 * example token processing layers. This is actually a template since it
 * might be used with different types of Tokens, and avoiding code
 * duplication is good.
 */
template<typename T>
class TokenSourceTemplate
{
public:
	/// Constructor
	TokenSourceTemplate()
	{
	}

	/// Destructor
	virtual ~TokenSourceTemplate()
	{
	}

	/**
	 * Get the next token, or NULL if there are no more tokens.
	 * Caller takes ownership of the token.
	 */
	virtual T* get_next_token() = 0;

	/**
	 * Tokenization helper. Tokenizes the entire input, calling the sink
	 * functor or each token. The sink takes ownership of the token.
	 */
	void tokenize(boost::function<void (T*)> sink) {
		while (T* t = get_next_token()) {
			sink(t);
		}
	}
};

namespace detail {
	// Helper template class to get type T when all we have is a type T*
	// This bit does nothing by default...
	template<typename T>
	struct deptr
	{
	};

	// ...and this has a typedef for T when the template is used with T*
	template<typename T>
	struct deptr<T*>
	{
		typedef T type;
	};
	template<typename T>
	struct deptr<const T*>
	{
		typedef T type;
	};
	template<typename T>
	struct deptr<T* const>
	{
		typedef const T type;
	};
} /* end ns detail */

/**
 * Generic TokenSource wrapper for containers of tagger Token pointers,
 * e.g. a std::vector<Token*> or a boost::range of Token* iterators.
 *
 * The container should not be modified as long as it is being accesed
 * by a RangeSource wrapper.
 *
 * There is some mild template magic here to make it work with the
 * templated TokenSource. Basically when given a container of some Token*,
 * we want to end up with a TokenSource<Token>, hence the deptr.
 */
template<typename T>
class RangeSource : public TokenSourceTemplate<
		typename detail::deptr<typename T::value_type>::type>
{
public:
	/// convenience typedef
	typedef typename T::const_iterator const_iterator;

	/// Constructor from a range of type T, where T is iterable
	/// (i.e. has const_iterator and value_type typedefs and begin / end
	/// member functions.
	RangeSource(const T& range)
		: end_(range.end()), ptr_(range.begin())
	{
	}

	/// TokenSource override
	typename T::value_type get_next_token()
	{
		if (ptr_ != end_) {
			return *ptr_++;
		} else {
			return NULL;
		}
	}

private:
	/// The end iterator
	const_iterator end_;

	/// Cuurrent position iterator
	const_iterator ptr_;
};

/// Helper function to make a RangeSource from a range,
/// avoiding lenghty template instantiation
template<class T>
RangeSource<T>* make_range_source(const T& range)
{
	return new RangeSource<T>(range);
}

/// Helper function to make a RangeSource from two iterators,
/// avoiding lenghty template instantiation
template<class T>
RangeSource<boost::iterator_range<T> >* make_range_source(const T& b,
		const T& e)
{
	boost::iterator_range<T> range(b, e);
	return new RangeSource< boost::iterator_range<T> >(range);
}

} /* end ns PwrNlp */


#endif // PWRNLP_TOKENSOURCE_H
