/*
    Copyright (C) 2011 Adam Wardyński, Tomasz Śniatowski, Paweł Kędzia,
    Adam Radziszewski, Bartosz Broda
    Part of the WCCL project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

#ifndef LIBWCCL_OPS_EXPRESSION_H
#define LIBWCCL_OPS_EXPRESSION_H

#include <libcorpus2/tagset.h>
#include <boost/noncopyable.hpp>
#include <sstream>

namespace Wccl {

/**
 * Abstract base class for WCCL expressions
 */
class Expression : boost::noncopyable
{
public:
	/**
	 * @returns String representation of the expression.
	 */
	virtual std::string to_string(const Corpus2::Tagset& tagset) const = 0;
	/**
	 * @returns String representation of the expression that does not
	 * require a tagset.
	 * @note Might be incomplete and/or contain internal info.
	 */
	std::string to_raw_string() const;

	/**
	 * Writes the raw form of the expression directly to a stream.
	 * @note The written form is raw, may be incomplete and/or contain
	 * internal info
	 * @returns As usual, the stream written to, to allow chaining.
	 */
	friend std::ostream& operator<<(std::ostream& ostream, const Expression& expression);

	/**
	 * Virtual destructor, so if delete is ever called on a base-class-type
	 * of pointer, the destruction is forwarded to proper destructor
	 * in a derived class, if a special destructor was needed for that class.
	 */
	virtual ~Expression() {};

protected:
	virtual std::ostream& write_to(std::ostream& ostream) const = 0;
};


//
//  ----- Implementation -----
//

inline
std::ostream& operator <<(std::ostream& ostream, const Expression& expression) {
	return expression.write_to(ostream);
}

inline
std::string Expression::to_raw_string() const {
	std::ostringstream ss;
	ss << *this;
	return ss.str();
}

} /* end ns Wccl */

#endif // LIBWCCL_OPS_EXPRESSION_H
