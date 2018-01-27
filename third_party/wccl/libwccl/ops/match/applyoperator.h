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

#ifndef LIBWCCL_OPS_MATCH_APPLYOPERATOR_H
#define LIBWCCL_OPS_MATCH_APPLYOPERATOR_H

#include <libwccl/ops/functions/bool/predicates/logicalpredicate.h>

namespace Wccl {
namespace Matching {

class ConjConditions;
class MatchAction;
/**
 * Operator that realizes "apply" functionality for match rules
 */
class ApplyOperator : public Expression
{
public:
	typedef LogicalPredicate::BoolFunctionPtrVector BoolFunctionPtrVector;
	/**
	  * @param matches Accessor to the "$m:_M" variable
	  * @param match_conditions "match" sections of apply; conditions for finding matches
	  * @param actions "actions" section of apply, should not be empty; actions to apply on matches
	  * @param conditions "cond" section of apply, empty by default; conditions for applying the actions
	  */
	ApplyOperator(
			const VariableAccessor<Match>& matches,
			const boost::shared_ptr<const ConjConditions>& match_conditions,
			const boost::shared_ptr<const std::vector<boost::shared_ptr<MatchAction> > >& actions,
			const boost::shared_ptr<const BoolFunctionPtrVector>& apply_conditions
				= boost::shared_ptr<const BoolFunctionPtrVector>(new BoolFunctionPtrVector()));

	/**
	 * @returns Name of the operator.
	 */
	std::string name() const {
		return "apply";
	}

	/**
	 * @returns String representation of the Action
	 */
	std::string to_string(const Corpus2::Tagset& tagset) const;

	/**
	 * Executes the apply operator on given context.
	 * It iterates current sentence position applying match operator
	 * and if match is found and conditions are met, executes actions.
	 */
	virtual void execute(const ActionExecContext &context) const;

protected:
	/**
	 * Writes string representation of the operator to
	 * an output stream.
	 * @returns Stream written to.
	 * @note May be incomplete and/or containt internal info.
	 */
	std::ostream& write_to(std::ostream& ostream) const;

private:
	const VariableAccessor<Match> matches_;
	const boost::shared_ptr<const ConjConditions> match_conditions_;
	const boost::shared_ptr<const std::vector<boost::shared_ptr<MatchAction> > > actions_;
	const boost::shared_ptr<const std::vector<boost::shared_ptr<Function<Bool> > > > apply_conditions_;
};

} /* end ns Matching */
} /* end ns Wccl */

#endif // LIBWCCL_OPS_MATCH_APPLYOPERATOR_H
