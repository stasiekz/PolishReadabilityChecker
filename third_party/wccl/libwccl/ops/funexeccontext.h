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

#ifndef LIBWCCL_OPS_FUNEXECCONTEXT_H
#define LIBWCCL_OPS_FUNEXECCONTEXT_H

#include <boost/noncopyable.hpp>
#include <libwccl/variables.h>
#include <libwccl/sentencecontext.h>
#include <libwccl/ops/actionexeccontext.h>

namespace Wccl {

/**
 * Class holding execution context of a functional operator
 * i.e. state that the function should operate on.
 */
class FunExecContext : boost::noncopyable
{
public:
	FunExecContext( const SentenceContext& sentence_context,
					const boost::shared_ptr<Variables>& vars)
		: sentence_context_(sentence_context), vars_(vars)
	{
	}

	FunExecContext(const ActionExecContext& action_exec_context)
		: sentence_context_(action_exec_context.sentence_context()),
		  vars_(action_exec_context.variables())
	{
		// this is intentionally left implicit :)
	}

	/**
	 * @returns Context of a sentence the operator is applied to.
	 * @note Functional operators should not change sentence state,
	 * hence the const reference.
	 */
	const SentenceContext& sentence_context() const {
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
	const SentenceContext& sentence_context_;
	const boost::shared_ptr<Variables> vars_;
};

} /* end ns Wccl */

#endif // LIBWCCL_OPS_FUNEXECCONTEXT_H
