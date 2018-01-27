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

#include <libwccl/parser/Parser.h>
#include "ANTLRLexer.hpp"
#include "ANTLRParser.hpp"

#include <antlr/NoViableAltException.hpp>
#include <antlr/MismatchedTokenException.hpp>
#include <antlr/TokenStreamRecognitionException.hpp>

#include <libcorpus2/tagsetmanager.h>

namespace Wccl {

Parser::Parser(const Corpus2::Tagset& t) : tagset_(new Corpus2::Tagset(t))
{
}

Parser::Parser(const boost::shared_ptr<const Corpus2::Tagset> tagset) : tagset_(tagset)
{
}

Parser::Parser(const std::string& tagset_name)
: tagset_(new Corpus2::Tagset(Corpus2::get_named_tagset(tagset_name)))
{
}

/**
 *
 */
Parser::~Parser()
{
	//
}

// ----------------------------------------------------------------------------
/**
 * @desc Parse a string operator contained in a std::string. Converts the string
 *       to a stream and calls parseStringOperator with it
 * @arg str operator string
 * @return the parsed operator via a shared pointer
 */
boost::shared_ptr<Operator<StrSet> > Parser::parseStringOperator(
		const std::string& str) const
{
	std::stringstream ss (std::stringstream::in | std::stringstream::out);
	ss << str;

	try {
		return this->parseStringOperator(ss);
	}
	catch (ParserException&) {
		throw;
	}
}

/**
 * @desc Parse a string operator. Runs parse_string_operator rule
 *       in the parser grammar.
 * @arg istr input stream with the operator
 * @return the parsed operator via a shared pointer
 */
boost::shared_ptr<Operator<StrSet> > Parser::parseStringOperator(
		std::istream& istr) const
{
	ANTLRLexer lexer(istr);
	ANTLRParser parser(lexer);
	boost::shared_ptr<Operator<StrSet> > res;

	try {
		res = parser.parse_strset_operator(tagset_);
	} catch (antlr::MismatchedTokenException &e) {
		throw ParserException(
				e.getFileLineColumnString() + " " + e.getMessage()
		);
	} catch(antlr::NoViableAltException &e) {
		throw ParserException(
			e.getFileLineColumnString() + " " + e.getMessage()
		);
	} catch(antlr::TokenStreamRecognitionException &e) {
		throw ParserException(
			e.getLine() + ":" + e.getColumn() + std::string(" ") + e.getMessage()
		);
	} catch (antlr::ANTLRException& e) {
		throw ParserException(e.getMessage());
	}

	return res;
}

/**
 * @desc Parse predicates contained in a std::string. Converts the string
 *       to a stream and callis parsePredicate with it
 * @arg str operator string
 * @return the parsed operator via a shared pointer
 */
boost::shared_ptr<Operator<Bool> > Parser::parseBoolOperator(
		const std::string& str) const
{
	std::stringstream ss (std::stringstream::in | std::stringstream::out);
	ss << str;

	try {
		return this->parseBoolOperator(ss);
	}
	catch (ParserException&) {
		throw;
	}
}

/**
 * @desc Parse a predicate. Runs parse_predicates rule in the parser grammar.
 * @arg istr input stream with writed predicate
 * @return the parsed operator via a shared pointer
 */
boost::shared_ptr<Operator<Bool> > Parser::parseBoolOperator(
		std::istream& istr) const
{
	ANTLRLexer lexer(istr);
	ANTLRParser parser(lexer);
	boost::shared_ptr<Operator<Bool> > res;

	try {
		res = parser.parse_bool_operator(tagset_);
	} catch (antlr::MismatchedTokenException &e) {
		throw ParserException(
				e.getFileLineColumnString() + " " + e.getMessage()
		);
	} catch(antlr::NoViableAltException &e) {
		throw ParserException(
			e.getFileLineColumnString() + " " + e.getMessage()
		);
	} catch(antlr::TokenStreamRecognitionException &e) {
		throw ParserException(
			e.getLine() + ":" + e.getColumn() + std::string(" ") + e.getMessage()
		);
	} catch (antlr::ANTLRException& e) {
		throw ParserException(e.getMessage());
	}

	return res;
}

// ----------------------------------------------------------------------------
/**
 * @desc Parse a sym set operator contained in a std::string. Converts the
 *       string to a stream and calls parseSymSetOperator with it
 * @arg str operator string
 * @return the parsed operator via a shared pointer
 */
boost::shared_ptr<Operator<TSet> > Parser::parseSymSetOperator(
		const std::string& str) const
{
	std::stringstream ss (std::stringstream::in | std::stringstream::out);
	ss << str;

	try {
		return this->parseSymSetOperator(ss);
	}
	catch (ParserException&) {
		throw;
	}
}

/**
 * @desc Parse a sym set operator. Runs parse_sym_set_operator rule
 *       in the parser grammar.
 * @arg istr input stream with the operator
 * @return the parsed operator via a shared pointer
 */
boost::shared_ptr<Operator<TSet> > Parser::parseSymSetOperator(
		std::istream& istr) const
{
	ANTLRLexer lexer(istr);
	ANTLRParser parser(lexer);
	boost::shared_ptr<Operator<TSet> > res;

	try {
		res = parser.parse_symset_operator(tagset_);
	} catch (antlr::MismatchedTokenException &e) {
		throw ParserException(
				e.getFileLineColumnString() + " " + e.getMessage()
		);
	} catch(antlr::NoViableAltException &e) {
		throw ParserException(
			e.getFileLineColumnString() + " " + e.getMessage()
		);
	} catch(antlr::TokenStreamRecognitionException &e) {
		throw ParserException(
			e.getLine() + ":" + e.getColumn() + std::string(" ") + e.getMessage()
		);
	} catch (antlr::ANTLRException& e) {
		throw ParserException(e.getMessage());
	}

	return res;
}

// ----------------------------------------------------------------------------
/**
 * @desc Parse a position operator contained in a std::string. Converts the
 *       string to a stream and calls parseSymSetOperator with it
 * @arg str operator string
 * @return the parsed operator via a shared pointer
 */
boost::shared_ptr<Operator<Position> > Parser::parsePositionOperator(
		const std::string& str) const
{
	std::stringstream ss (std::stringstream::in | std::stringstream::out);
	ss << str;

	try {
		return this->parsePositionOperator(ss);
	}
	catch (ParserException&) {
		throw;
	}
}

/**
 * @desc Parse a position operator. Runs parse_sym_set_operator rule
 *       in the parser grammar.
 * @arg istr input stream with the operator
 * @return the parsed operator via a shared pointer
 */
boost::shared_ptr<Operator<Position> > Parser::parsePositionOperator(
		std::istream& istr) const
{
	ANTLRLexer lexer(istr);
	ANTLRParser parser(lexer);
	boost::shared_ptr<Operator<Position> > res;

	try {
		res = parser.parse_position_operator(tagset_);
	} catch (antlr::MismatchedTokenException &e) {
		throw ParserException(
				e.getFileLineColumnString() + " " + e.getMessage()
		);
	} catch(antlr::NoViableAltException &e) {
		throw ParserException(
			e.getFileLineColumnString() + " " + e.getMessage()
		);
	} catch(antlr::TokenStreamRecognitionException &e) {
		throw ParserException(
			e.getLine() + ":" + e.getColumn() + std::string(" ") + e.getMessage()
		);
	} catch (antlr::ANTLRException& e) {
		throw ParserException(e.getMessage());
	}

	return res;
}

/**
 * @desc Parse any operator contained in a std::string. Converts the string to
 *       to a stream and calls parseAnyOperator with it.
 * @arg str operator string
 * @return the parsed operator via a shared pointer
 */
boost::shared_ptr<FunctionalOperator> Parser::parseAnyOperator(
		const std::string& str) const
{
	std::stringstream ss (std::stringstream::in | std::stringstream::out);
	ss << str;

	return this->parseAnyOperator(ss);
}

/**
 * @desc Parse any operator. Runs parse_*_operator rules in sequence
 *       in the parser grammar until one succedes, or all fail. Rethrows
 *       the exception returned by the last parse_* attempt.
 * @arg istr input stream with the operator
 * @return the parsed operator via a shared pointer
 */
boost::shared_ptr<FunctionalOperator> Parser::parseAnyOperator(
		std::istream& istr) const
{
	std::stringstream ss;
	ss << istr.rdbuf();
	std::stringstream errors;
	boost::shared_ptr<FunctionalOperator> result;
	if (!result) {
		ss.clear();
		ss.seekg(0, std::ios::beg);
		ANTLRLexer lexer(ss);
		ANTLRParser parser(lexer);
		try {
			result = parser.parse_symset_operator(tagset_);
		} catch (antlr::MismatchedTokenException &e) {
			errors << std::endl
					<< "(as tset    ) "
					<< e.getFileLineColumnString()
					<< " " << e.getMessage();
		} catch(antlr::NoViableAltException &e) {
			errors << std::endl
					<< "(as tset    ) "
					<< e.getFileLineColumnString()
					<< " " << e.getMessage();
		} catch(antlr::TokenStreamRecognitionException &e) {
			errors << std::endl
					<< "(as tset    ) "
					<< e.getLine() << ":" << e.getColumn()
					<< " " << e.getMessage();
		} catch (antlr::ANTLRException& e) {
			errors << std::endl
					<< "(as tset    ) " << e.getMessage();
			// ignore, try another type
		}
	}
	if (!result) {
		ss.clear();
		ss.seekg(0, std::ios::beg);
		ANTLRLexer lexer(ss);
		ANTLRParser parser(lexer);
		try {
			result = parser.parse_strset_operator(tagset_);
		} catch (antlr::MismatchedTokenException &e) {
			errors << std::endl
					<< "(as strset  ) "
					<< e.getFileLineColumnString()
					<< " " << e.getMessage();
		} catch(antlr::NoViableAltException &e) {
			errors << std::endl
					<< "(as strset  ) "
					<< e.getFileLineColumnString()
					<< " " << e.getMessage();
		} catch(antlr::TokenStreamRecognitionException &e) {
			errors << std::endl
					<< "(as strset  ) "
					<< e.getLine() << ":" << e.getColumn()
					<< " " << e.getMessage();
		} catch (antlr::ANTLRException& e) {
			errors << std::endl
					<< "(as strset  ) " << e.getMessage();
			// ignore, try another type
		}
	}
	if (!result) {
		ss.clear();
		ss.seekg(0, std::ios::beg);
		ANTLRLexer lexer(ss);
		ANTLRParser parser(lexer);
		try {
			result = parser.parse_bool_operator(tagset_);
		} catch (antlr::MismatchedTokenException &e) {
			errors << std::endl
					<< "(as bool    ) "
					<< e.getFileLineColumnString()
					<< " " << e.getMessage();
		} catch(antlr::NoViableAltException &e) {
			errors << std::endl
					<< "(as bool    ) "
					<< e.getFileLineColumnString()
					<< " " << e.getMessage();
		} catch(antlr::TokenStreamRecognitionException &e) {
			errors << std::endl
					<< "(as bool    ) "
					<< e.getLine() << ":" << e.getColumn()
					<< " " << e.getMessage();
		} catch (antlr::ANTLRException& e) {
			errors << std::endl
					<< "(as bool    ) " << e.getMessage();
			// ignore, try another type
		}
	}
	if (!result) {
		ss.clear();
		ss.seekg(0, std::ios::beg);
		ANTLRLexer lexer(ss);
		ANTLRParser parser(lexer);
		try {
			result = parser.parse_position_operator(tagset_);
		} catch (antlr::MismatchedTokenException &e) {
			errors << std::endl
					<< "(as position) "
					<< e.getFileLineColumnString()
					<< " " << e.getMessage() << std::endl;
		} catch(antlr::TokenStreamRecognitionException &e) {
			errors << std::endl
					<< "(as position) "
					<< e.getLine() << ":" << e.getColumn()
					<< " " << e.getMessage();
		} catch(antlr::NoViableAltException &e) {
			errors << std::endl
					<< "(as position) "
					<< e.getFileLineColumnString()
					<< " " << e.getMessage() << std::endl;
		} catch (antlr::ANTLRException& e) {
			errors << std::endl
					<< "(as position) " << e.getMessage() << std::endl;
			// ignore, try another type
		}
	}
	if (!result) {
		throw ParserException(errors.str());
	}
	return result;
}

// ----------------------------------------------------------------------------

/**
 * @desc Parse rule sequence contained in a std::string. Converts the string
 *       to a stream and calls parseTagRuleSequence with it
 * @arg str rules string
 * @return the parsed rule sequence via a shared pointer
 */
boost::shared_ptr<TagRuleSequence> Parser::parseTagRuleSequence(
		const std::string& str) const
{
	std::stringstream ss (std::stringstream::in | std::stringstream::out);
	ss << str;

	try {
		return this->parseTagRuleSequence(ss);
	}
	catch (ParserException&) {
		throw;
	}
}

/**
 * @desc Parse a sequence rules. Runs parse_tag_rule_sequence rule in the parser
 *       grammar.
 * @arg istr input stream with writed rules
 * @return the parsed rule sequence via a shared pointer
 */
boost::shared_ptr<TagRuleSequence> Parser::parseTagRuleSequence(
		std::istream& istr) const
{
	ANTLRLexer lexer(istr);
	ANTLRParser parser(lexer);
	boost::shared_ptr<TagRuleSequence> res;

	try {
		res = parser.parse_tag_rule_sequence(tagset_);
	} catch (antlr::MismatchedTokenException &e) {
		throw ParserException(
				e.getFileLineColumnString() + " " + e.getMessage()
		);
	} catch(antlr::NoViableAltException &e) {
		throw ParserException(
			e.getFileLineColumnString() + " " + e.getMessage()
		);
	} catch(antlr::TokenStreamRecognitionException &e) {
		throw ParserException(
			e.getLine() + ":" + e.getColumn() + std::string(" ") + e.getMessage()
		);
	} catch (antlr::ANTLRException& e) {
		throw ParserException(e.getMessage());
	}

	return res;
}

// ----------------------------------------------------------------------------

/**
 * @desc Parse single rule contained in a std::string. Converts the string
 *       to a stream and calls parseSingleRule with it
 * @arg str rule
 * @return the parsed rule via a shared pointer
 */
boost::shared_ptr<TagRule> Parser::parseSingleRule(
		const std::string& str) const
{
	std::stringstream ss (std::stringstream::in | std::stringstream::out);
	ss << str;

	try {
		return this->parseSingleRule(ss);
	}
	catch (ParserException&) {
		throw;
	}
}

/**
 * @desc Parse a single rule. Runs parse_single_tag_rule rule in the parser
 *       grammar.
 * @arg istr input stream with writed rule
 * @return the parsed rule via a shared pointer
 */
boost::shared_ptr<TagRule> Parser::parseSingleRule(
		std::istream& istr) const
{
	ANTLRLexer lexer(istr);
	ANTLRParser parser(lexer);
	boost::shared_ptr<TagRule> res;

	try {
		res = parser.parse_single_tag_rule(tagset_);
	} catch (antlr::MismatchedTokenException &e) {
		throw ParserException(
				e.getFileLineColumnString() + " " + e.getMessage()
		);
	} catch(antlr::NoViableAltException &e) {
		throw ParserException(
			e.getFileLineColumnString() + " " + e.getMessage()
		);
	} catch(antlr::TokenStreamRecognitionException &e) {
		throw ParserException(
			e.getLine() + ":" + e.getColumn() + std::string(" ") + e.getMessage()
		);
	} catch (antlr::ANTLRException& e) {
		throw ParserException(e.getMessage());
	}

	return res;
}

// ----------------------------------------------------------------------------

/**
 * @desc Parse single match rule contained in a std::string. Converts the string
 *       to a stream and calls parseMatchRule with it
 * @arg istr input stream with writed rule
 * @return the parsed rule via a shared pointer
 */
boost::shared_ptr<Matching::MatchRule> Parser::parseMatchRule(
		const std::string& str) const
{
	std::stringstream ss (std::stringstream::in | std::stringstream::out);
	ss << str;

	try {
		return this->parseMatchRule(ss);
	}
	catch (ParserException&) {
		throw;
	}
}

/**
 * @desc Parse match rule. Runs parse_match_rule rule from the parser grammar.
 * @arg istr input stream with writed rule
 * @return the parsed rule via a shared pointer
 */
boost::shared_ptr<Matching::MatchRule> Parser::parseMatchRule(std::istream& istr) const
{
	ANTLRLexer lexer(istr);
	ANTLRParser parser(lexer);
	boost::shared_ptr<Matching::MatchRule> res;

	try {
		res = parser.parse_match_rule(tagset_);
	} catch (antlr::MismatchedTokenException &e) {
		throw ParserException(
				e.getFileLineColumnString() + " " + e.getMessage()
		);
	} catch(antlr::NoViableAltException &e) {
		throw ParserException(
			e.getFileLineColumnString() + " " + e.getMessage()
		);
	} catch(antlr::TokenStreamRecognitionException &e) {
		throw ParserException(
			e.getLine() + ":" + e.getColumn() + std::string(" ") + e.getMessage()
		);
	} catch (antlr::ANTLRException& e) {
		throw ParserException(e.getMessage());
	}

	return res;
}

// ----------------------------------------------------------------------------

/**
 * @desc Parse an entire WCCL file contained in a std::string.
 *       Converts the string to a stream and calls parseWcclFile with it
 * @arg istr input stream with writed rule
 * @return the parsed file via a shared pointer
 */
boost::shared_ptr<WcclFile> Parser::parseWcclFile(
		const std::string& str, const std::string& search_path /*= "."*/) const
{
	std::stringstream ss (std::stringstream::in | std::stringstream::out);
	ss << str;

	try {
		return this->parseWcclFile(ss, search_path);
	}
	catch (ParserException&) {
		throw;
	}
}

/**
 * @desc Parse an entire WCCL file. Runs parse_wccl_file rule from the parser grammar.
 * @arg istr input stream with writed rule
 * @return the parsed file via a shared pointer
 */
boost::shared_ptr<WcclFile> Parser::parseWcclFile(std::istream& istr,
		const std::string& search_path /*= "."*/) const
{
	ANTLRLexer lexer(istr);
	ANTLRParser parser(lexer);
	boost::shared_ptr<WcclFile> res;

	try {
		res = parser.parse_wccl_file(tagset_, search_path);
	} catch (antlr::MismatchedTokenException &e) {
		throw ParserException(
				e.getFileLineColumnString() + " " + e.getMessage()
		);
	} catch(antlr::NoViableAltException &e) {
		throw ParserException(
			e.getFileLineColumnString() + " " + e.getMessage()
		);
	} catch(antlr::TokenStreamRecognitionException &e) {
		throw ParserException(
			e.getLine() + ":" + e.getColumn() + std::string(" ") + e.getMessage()
		);
	} catch (antlr::ANTLRException& e) {
		throw ParserException(e.getMessage());
	}

	return res;
}

boost::shared_ptr<WcclFile> Parser::parseWcclFileFromPath(const std::string&
		filename, const std::string& search_path /*= "."*/) const
{
	std::ifstream is(filename.c_str());
	if (!is.good()) {
		throw Wccl::FileNotFound(filename, "", __FUNCTION__);
	}
	try {
		return this->parseWcclFile(is, search_path);
	}
	catch (ParserException&) {
		throw;
	}	
}

} // end Wccl ns
