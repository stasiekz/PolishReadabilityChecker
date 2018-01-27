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

#include <boost/test/unit_test.hpp>
#include <boost/bind.hpp>

#include <libcorpus2/tagsetmanager.h>

#include <libwccl/ops/functions/bool/varsetter.h>
#include <libwccl/values/bool.h>
#include <libwccl/values/tset.h>
#include <libwccl/values/strset.h>
#include <libwccl/ops/functions/constant.h>

using namespace Wccl;

BOOST_AUTO_TEST_SUITE(varsetter)

struct VarSetFix
{
	VarSetFix()
		: sc(boost::make_shared<Corpus2::Sentence>()),
		  cx(sc, boost::make_shared<Variables>()),
		  tagset(Corpus2::get_named_tagset("kipi"))
	{
	}
	SentenceContext sc;
	FunExecContext cx;
	Corpus2::Tagset tagset;
};

struct VarSetBoolFix : public VarSetFix
{
	VarSetBoolFix() 
		: VarSetFix(),
		  true_value(true),
		  false_value(false),
		  true_constant(new Constant<Bool>(true_value)),
		  false_constant(new Constant<Bool>(false_value))
	{
		cx.variables()->put("True_bool", true_value);
		cx.variables()->put("False_bool", false_value);
	}
	Bool true_value;
	Bool false_value;
	boost::shared_ptr<Function<Bool> > true_constant;
	boost::shared_ptr<Function<Bool> > false_constant;
};

BOOST_FIXTURE_TEST_CASE(bool_apply, VarSetBoolFix)
{
	VariableAccessor<Bool> acc_t = cx.variables()->create_accessor<Bool>("True_bool");
	VarSetter<Bool> var_setter(acc_t, false_constant);
	BOOST_CHECK(cx.variables()->get<Bool>("True_bool")->get_value());
	BOOST_CHECK(var_setter.apply(cx)->get_value());
	BOOST_CHECK(!false_constant->apply(cx)->get_value());
	BOOST_CHECK(!cx.variables()->get<Bool>("True_bool")->get_value());
	VarSetter<Bool> var_setter_2(acc_t, true_constant);
	BOOST_CHECK(var_setter_2.apply(cx)->get_value());
	BOOST_CHECK(cx.variables()->get<Bool>("True_bool")->get_value());
}

BOOST_FIXTURE_TEST_CASE(bool_apply_acc_destruct, VarSetBoolFix)
{
	boost::shared_ptr<VarSetter<Bool> > var_setter;
	{
		VariableAccessor<Bool> acc_f = cx.variables()->create_accessor<Bool>("False_bool");
		var_setter.reset(new VarSetter<Bool>(acc_f, true_constant));
	}
	BOOST_CHECK(var_setter->apply(cx)->get_value());
	BOOST_CHECK(cx.variables()->get<Bool>("False_bool")->get_value());
}

struct VarSetPositionFix : public VarSetFix
{
	VarSetPositionFix()
		: VarSetFix(),
		  pos_69(69),
		  nowhere(Position::Nowhere),
		  pos_69_constant(new Constant<Position>(pos_69)),
		  nowhere_constant(new Constant<Position>(nowhere))
	{
		cx.variables()->put("Nowhere", nowhere);
		cx.variables()->put("End", Position(Position::End));
		cx.variables()->put("Pos_69", pos_69);
	}

	Position pos_69;
	Position nowhere;
	boost::shared_ptr<Function<Position> > pos_69_constant;
	boost::shared_ptr<Function<Position> > nowhere_constant;
};

BOOST_FIXTURE_TEST_CASE(set_position, VarSetPositionFix)
{
	VariableAccessor<Position> acc = cx.variables()->create_accessor<Position>("Nowhere");
	VarSetter<Position> var_setter(acc, pos_69_constant);
	BOOST_CHECK(cx.variables()->get<Position>("Nowhere")->equals(nowhere));
	BOOST_CHECK(var_setter.apply(cx)->get_value());
	BOOST_CHECK(cx.variables()->get<Position>("Nowhere")->equals(pos_69));
}

BOOST_FIXTURE_TEST_CASE(set_position_destruct, VarSetPositionFix)
{
	boost::shared_ptr<VarSetter<Position> > var_setter;
	{
		VariableAccessor<Position> acc = cx.variables()->create_accessor<Position>("Pos_69");
		var_setter.reset(new VarSetter<Position>(acc, nowhere_constant));
	}
	BOOST_CHECK(cx.variables()->get<Position>("Pos_69")->equals(pos_69));
	BOOST_CHECK(var_setter->apply(cx)->get_value());
	BOOST_CHECK(cx.variables()->get<Position>("Pos_69")->equals(nowhere));
}

struct VarSetStrSetFix : public VarSetFix
{
	VarSetStrSetFix()
		: VarSetFix(),
		  one_elem_set(),
		  empty_set(),
		  one_elem_set_constant(),
		  empty_set_constant(new Constant<StrSet>(empty_set))
	{
		one_elem_set.insert_utf8("ThereCanBeOnly1");
		one_elem_set_constant.reset(new Constant<StrSet>(one_elem_set));
		cx.variables()->put("OneElemSet", one_elem_set);
		cx.variables()->put("EmptySet", empty_set);
	}

	StrSet one_elem_set;
	StrSet empty_set;
	boost::shared_ptr<Function<StrSet> > one_elem_set_constant;
	boost::shared_ptr<Function<StrSet> > empty_set_constant;
};

BOOST_FIXTURE_TEST_CASE(set_strset, VarSetStrSetFix)
{
	VariableAccessor<StrSet> acc = cx.variables()->create_accessor<StrSet>("EmptySet");
	VarSetter<StrSet> var_setter(acc, one_elem_set_constant);
	BOOST_CHECK(cx.variables()->get<StrSet>("EmptySet")->equals(empty_set));
	BOOST_CHECK(var_setter.apply(cx)->get_value());
	BOOST_CHECK(cx.variables()->get<StrSet>("EmptySet")->equals(one_elem_set));
}

BOOST_FIXTURE_TEST_CASE(set_strset_destruct, VarSetStrSetFix)
{
	boost::shared_ptr<VarSetter<StrSet> > var_setter;
	{
		VariableAccessor<StrSet> acc = cx.variables()->create_accessor<StrSet>("OneElemSet");
		var_setter.reset(new VarSetter<StrSet>(acc, empty_set_constant));
	}
	BOOST_CHECK(cx.variables()->get<StrSet>("OneElemSet")->equals(one_elem_set));
	BOOST_CHECK(var_setter->apply(cx)->get_value());
	BOOST_CHECK(cx.variables()->get<StrSet>("OneElemSet")->equals(empty_set));
}

struct VarSetTSetFix : public VarSetFix
{
	VarSetTSetFix()
		: VarSetFix(),
		  empty_tset(),
		  subst_pl_tag(),
		  empty_tset_constant(new Constant<TSet>(empty_tset)),
		  subst_pl_tag_constant()
	{
		subst_pl_tag.insert_symbol(tagset, "subst");
		subst_pl_tag.insert_symbol(tagset, "pl");
		subst_pl_tag_constant.reset(new Constant<TSet>(subst_pl_tag));
		cx.variables()->put("EmptyTag", empty_tset);
		cx.variables()->put("SubstPl", subst_pl_tag);
	}
	
	TSet empty_tset;
	TSet subst_pl_tag;
	boost::shared_ptr<Function<TSet> > empty_tset_constant;
	boost::shared_ptr<Function<TSet> > subst_pl_tag_constant;
};

BOOST_FIXTURE_TEST_CASE(set_tset, VarSetTSetFix)
{
	VariableAccessor<TSet> acc = cx.variables()->create_accessor<TSet>("EmptyTag");
	VarSetter<TSet> var_setter(acc, subst_pl_tag_constant);
	BOOST_CHECK(cx.variables()->get<TSet>("EmptyTag")->equals(empty_tset));
	BOOST_CHECK(var_setter.apply(cx)->get_value());
	BOOST_CHECK(cx.variables()->get<TSet>("EmptyTag")->equals(subst_pl_tag));
}

BOOST_FIXTURE_TEST_CASE(set_TSet_destruct, VarSetTSetFix)
{
	boost::shared_ptr<VarSetter<TSet> > var_setter;
	{
		VariableAccessor<TSet> acc = cx.variables()->create_accessor<TSet>("SubstPl");
		var_setter.reset(new VarSetter<TSet>(acc, empty_tset_constant));
	}
	BOOST_CHECK(cx.variables()->get<TSet>("SubstPl")->equals(subst_pl_tag));
	BOOST_CHECK(var_setter->apply(cx)->get_value());
	BOOST_CHECK(cx.variables()->get<TSet>("SubstPl")->equals(empty_tset));
}

//----  To String -----

BOOST_FIXTURE_TEST_CASE(bool_varset_to_string, VarSetBoolFix)
{
	VariableAccessor<Bool> acc_t = cx.variables()->create_accessor<Bool>("True_bool");
	VarSetter<Bool> var_setter_t(acc_t, false_constant);
	std::string expected =
		"setvar(" + Bool::var_repr("True_bool") + ", False)";
	BOOST_CHECK_EQUAL(expected, var_setter_t.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(bool_varset_to_raw_string, VarSetBoolFix)
{
	VariableAccessor<Bool> acc_t = cx.variables()->create_accessor<Bool>("True_bool");
	VarSetter<Bool> var_setter_t(acc_t, false_constant);
	std::string expected =
		"setvar(" + Bool::var_repr("True_bool") + ", False)";
	BOOST_CHECK_EQUAL(expected, var_setter_t.to_raw_string());
}

BOOST_FIXTURE_TEST_CASE(position_varset_to_string, VarSetPositionFix)
{
	VariableAccessor<Position> acc_t = cx.variables()->create_accessor<Position>("Nowhere");
	VarSetter<Position> var_setter_t(acc_t, pos_69_constant);
	std::string expected =
		"setvar(" + Position::var_repr("Nowhere") + ", 69)";
	BOOST_CHECK_EQUAL(expected, var_setter_t.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(position_varset_to_raw_string, VarSetPositionFix)
{
	VariableAccessor<Position> acc_t = cx.variables()->create_accessor<Position>("End");
	VarSetter<Position> var_setter_t(acc_t, nowhere_constant);
	std::string expected =
		"setvar(" + Position::var_repr("End") + ", nowhere)";
	BOOST_CHECK_EQUAL(expected, var_setter_t.to_raw_string());
}

BOOST_FIXTURE_TEST_CASE(StrSet_varset_to_string, VarSetStrSetFix)
{
	VariableAccessor<StrSet> acc = cx.variables()->create_accessor<StrSet>("EmptySet");
	VarSetter<StrSet> var_setter_t(acc, one_elem_set_constant);
	std::string expected =
		"setvar(" + StrSet::var_repr("EmptySet") + ", [\"ThereCanBeOnly1\"])";
	BOOST_CHECK_EQUAL(expected, var_setter_t.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(StrSet_varset_to_raw_string, VarSetStrSetFix)
{
	VariableAccessor<StrSet> acc = cx.variables()->create_accessor<StrSet>("EmptySet");
	VarSetter<StrSet> var_setter(acc, one_elem_set_constant);
	std::string expected =
		"setvar(" + StrSet::var_repr("EmptySet") + ", [\"ThereCanBeOnly1\"])";
	BOOST_CHECK_EQUAL(expected, var_setter.to_raw_string());
}

BOOST_FIXTURE_TEST_CASE(tset_varset_to_string, VarSetTSetFix)
{
	VariableAccessor<TSet> acc = cx.variables()->create_accessor<TSet>("SubstPl");
	VarSetter<TSet> var_setter(acc, subst_pl_tag_constant);
	std::string expected =
		"setvar(" + TSet::var_repr("SubstPl") + ", " + subst_pl_tag_constant->to_string(tagset) + ")";
	BOOST_CHECK_EQUAL(expected, var_setter.to_string(tagset));
}

BOOST_FIXTURE_TEST_CASE(tset_varset_to_raw_string, VarSetTSetFix)
{
	VariableAccessor<TSet> acc = cx.variables()->create_accessor<TSet>("SubstPl");
	VarSetter<TSet> var_setter(acc, subst_pl_tag_constant);
	std::string expected =
		"setvar(" + TSet::var_repr("SubstPl") + ", " + subst_pl_tag_constant->to_raw_string() + ")";
	BOOST_CHECK_EQUAL(expected, var_setter.to_raw_string());
}


BOOST_AUTO_TEST_SUITE_END()
