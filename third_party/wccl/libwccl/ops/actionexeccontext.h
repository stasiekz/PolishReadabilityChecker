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

#ifndef LIBWCCL_OPS_ACTIONEXECCONTEXT_H
#define LIBWCCL_OPS_ACTIONEXECCONTEXT_H

#include <boost/noncopyable.hpp>
#include <libwccl/variables.h>
#include <libwccl/sentencecontext.h>

namespace Wccl {

/**
 * Class holding execution context of an action
 * i.e. state that the action should operate on.
 */
class ActionExecContext : boost::noncopyable {
public:
	ActionExecContext(
			SentenceContext& sentence_context,
			const boost::shared_ptr<Variables>& vars)
		: sentence_context_(sentence_context), vars_(vars)
	{
		BOOST_ASSERT(vars_);
	}

	/**
	 * @returns Context of a sentence the action is executed on.
	 */
	SentenceContext& sentence_context() const {
		return sentence_context_;
	}

	/**
	 * @returns Pointer to variables the operator should operate with.
	 * @note Variables should be accesible to modifications, but overall
	 * object should not get replaced, hence the const pointer.
	 */
	const boost::shared_ptr<Variables>& variables() const {
		return vars_;
	}

private:
	SentenceContext& sentence_context_;
	const boost::shared_ptr<Variables> vars_;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_ACTIONEXECCONTEXT_H
