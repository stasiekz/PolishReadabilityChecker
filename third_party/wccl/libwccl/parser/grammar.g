header {
	//don't try to add all the headers inside our namespace
	ANTLR_END_NAMESPACE

	#include <libwccl/parser/ParserException.h>
	#include <libwccl/parser/parsingscope.h>

	#include <cstdio>
	#include <antlr/Token.hpp>
	#include <boost/lexical_cast.hpp>


	// values/variables
	#include <libwccl/variables.h>
	#include <libwccl/values/bool.h>
	#include <libwccl/values/tset.h>
	#include <libwccl/values/strset.h>
	#include <libwccl/values/position.h>
	
	// sentence context
	#include <libwccl/sentencecontext.h>

	// operators
	#include <libwccl/ops/operator.h>

	#include <libwccl/ops/functions/constant.h>
	#include <libwccl/ops/functions/vargetter.h>
	#include <libwccl/ops/functions/conditional.h>

	#include <libwccl/ops/functions/setops.h>
	#include <libwccl/ops/functions/bool/varsetter.h>
	#include <libwccl/ops/functions/bool/predicates/debug.h>
	#include <libwccl/ops/functions/bool/predicates/ambiguous.h>
	#include <libwccl/ops/functions/bool/predicates/issingular.h>
	#include <libwccl/ops/functions/bool/predicates/or.h>
	#include <libwccl/ops/functions/bool/predicates/nor.h>
	#include <libwccl/ops/functions/bool/predicates/and.h>
	#include <libwccl/ops/functions/bool/predicates/regex.h>
	#include <libwccl/ops/functions/bool/predicates/intersects.h>
	#include <libwccl/ops/functions/bool/predicates/issubsetof.h>
	#include <libwccl/ops/functions/bool/predicates/isinside.h>
	#include <libwccl/ops/functions/bool/predicates/isoutside.h>
	#include <libwccl/ops/functions/bool/predicates/isempty.h>
	#include <libwccl/ops/functions/bool/predicates/equals.h>
	#include <libwccl/ops/functions/bool/predicates/weakagreement.h>
	#include <libwccl/ops/functions/bool/predicates/pointagreement.h>
	#include <libwccl/ops/functions/bool/predicates/strongagreement.h>
	#include <libwccl/ops/functions/bool/predicates/annsub.h>
	#include <libwccl/ops/functions/bool/predicates/ann.h>
	#include <libwccl/ops/functions/bool/predicates/anntoken.h>

	#include <libwccl/ops/functions/strset/affix.h>
	#include <libwccl/ops/functions/strset/getorth.h>
	#include <libwccl/ops/functions/strset/toupper.h>
	#include <libwccl/ops/functions/strset/tolower.h>
	#include <libwccl/ops/functions/strset/getlemmas.h>
	#include <libwccl/ops/functions/strset/lextranslator.h>
	#include <libwccl/ops/functions/strset/propval.h>
	#include <libwccl/ops/functions/strset/anninter.h>
	
	#include <libwccl/ops/functions/tset/agrfilter.h>
	#include <libwccl/ops/functions/tset/catfilter.h>
	#include <libwccl/ops/functions/tset/getsymbols.h>
	#include <libwccl/ops/functions/tset/getwordclass.h>
	#include <libwccl/ops/functions/tset/getsymbolsinrange.h>

	#include <libwccl/ops/functions/position/relativeposition.h>
	#include <libwccl/ops/functions/position/lasttoken.h>
	#include <libwccl/ops/functions/position/firsttoken.h>

	#include <libwccl/ops/functions/bool/iterations/only.h>
	#include <libwccl/ops/functions/bool/iterations/atleast.h>
	#include <libwccl/ops/functions/bool/iterations/leftlook.h>
	#include <libwccl/ops/functions/bool/iterations/rightlook.h>
	#include <libwccl/ops/functions/bool/iterations/skip.h>
	
	// Rules, actions
	#include <libwccl/ops/tagrule.h>
	#include <libwccl/ops/matchrule.h>
	#include <libwccl/ops/tagrulesequence.h>
	//
	#include <libwccl/ops/tagactions/unify.h>
	#include <libwccl/ops/tagactions/delete.h>
	#include <libwccl/ops/tagactions/select.h>
	#include <libwccl/ops/tagactions/relabel.h>
	#include <libwccl/ops/tagactions/mark.h>
	#include <libwccl/ops/tagactions/unmark.h>

	// Match operators
	#include <libwccl/values/tokenmatch.h>
	#include <libwccl/values/annotationmatch.h>
	#include <libwccl/values/matchvector.h>
	#include <libwccl/ops/match/applyoperator.h>
	#include <libwccl/ops/match/conditions/optionalmatch.h>
	#include <libwccl/ops/match/conditions/repeatedmatch.h>
	#include <libwccl/ops/match/conditions/isannotatedas.h>
	#include <libwccl/ops/match/conditions/matchtext.h>
	#include <libwccl/ops/match/conditions/conjconditions.h>
	#include <libwccl/ops/match/conditions/tokencondition.h>
	#include <libwccl/ops/match/conditions/oneof.h>
	#include <libwccl/ops/match/conditions/longest.h>
	#include <libwccl/ops/match/actions/markmatch.h>
	#include <libwccl/ops/match/actions/unmarkmatch.h>
	#include <libwccl/ops/match/actions/setpropmatch.h>
	#include <libwccl/ops/match/actions/overwritematch.h>
	#include <libwccl/ops/functions/match/submatch.h>

	// Wccl whole file syntax
	#include <libwccl/wcclfile.h>
	
	#include <libwccl/lexicon/lexiconparser.h>

	// Unicode String
	#include <unicode/uniset.h>
	#include <unicode/unistr.h>

	// start our namespace again
	ANTLR_BEGIN_NAMESPACE(Wccl)
}

options {
	language = "Cpp";
	genHashLines = false;
	namespace = "Wccl";
//	genHashLines = true;
}

// ----------------------------------------------------------------------------
// ANTLR PARSER
// ----------------------------------------------------------------------------
class ANTLRParser extends Parser;
options {
	k = 1;
	buildAST = false;
	exportVocab = ANTLRExpr;
	defaultErrorHandler = false;
}
{
private:
	// 
	const UnicodeString token_ref_to_ustring(antlr::RefToken& rstr) const { 
		return UnicodeString::fromUTF8(((antlr::Token*)rstr)->getText().c_str()).unescape();
	}
	/*
	const UnicodeString str_token_ref_to_ustring(antlr::RefToken& rstr) const { 
		UnicodeString ret_ustr, ustr = token_ref_to_ustring(rstr);

		if (ustr.length() < 3) {
			return "";
		}

		ustr.extract(1, ustr.length() - 2, ret_ustr);

		return ret_ustr;
	}
	*/
	//
	const std::string str_token_rem_grav(antlr::RefToken& rstr) const {
		size_t len = 0;
		std::string ret = token_ref_to_std_string(rstr);

		if ((len = ret.length()) < 2) {
			return ret;
		}

		if (ret[0] == '`' && ret[len - 1] == '`') {
			return ret.substr(1, len - 2);
		}

		return ret;
	}
	//
	const std::string token_ref_to_std_string(antlr::RefToken& rstr) const { 
		return (((antlr::Token*)rstr)->getText());
	}
	//
	int token_ref_to_int(antlr::RefToken& rstr) { 
		return atoi(((antlr::Token*)rstr)->getText().c_str());
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// "GLOBAL" RULES
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// Rule for parsing string set operator with scope. 
// Returns boost::shared_ptr<Operator<StrSet> >
parse_strset_operator
	[const boost::shared_ptr<const Corpus2::Tagset> tagset]
	returns [boost::shared_ptr<Operator<StrSet> > res]
{
	Lexicons empty_lex;
	ParsingScope scope(tagset, empty_lex);
}
	: res = functional_operator_strset [scope]
	EOF
;

// ----------------------------------------------------------------------------
// Rule for parsing bool operator with scope. 
// Returns boost::shared_ptr<Operator<Bool> > 
parse_bool_operator 
	[const boost::shared_ptr<const Corpus2::Tagset> tagset]
	returns [boost::shared_ptr<Operator<Bool> > res]
{
	Lexicons empty_lex;
	ParsingScope scope(tagset, empty_lex);
}
	: res = functional_operator_bool [scope]
	EOF
;

// ----------------------------------------------------------------------------
// Rule for parsing symbol set operator with scope.
// Returns boost::shared_ptr<Operator<TSet> >
parse_symset_operator
	[const boost::shared_ptr<const Corpus2::Tagset> tagset]
	returns [boost::shared_ptr<Operator<TSet> > res]
{
	Lexicons empty_lex;
	ParsingScope scope(tagset, empty_lex);
}
	: res = functional_operator_symset [scope]
	EOF
;

// ----------------------------------------------------------------------------
// Rule for parsing position operator with scope.
// Returns boost::shared_ptr<Operator<Position> >
parse_position_operator
	[const boost::shared_ptr<const Corpus2::Tagset> tagset]
	returns [boost::shared_ptr<Operator<Position> > res]
{	
	Lexicons empty_lex;
	ParsingScope scope(tagset, empty_lex);
}
	: res = functional_operator_position [scope]
	EOF
;

// ----------------------------------------------------------------------------
// Rule for parsing match operator with scope.
// Returns boost::shared_ptr<Operator<Position> >
parse_match_operator
	[const boost::shared_ptr<const Corpus2::Tagset> tagset]
	returns [boost::shared_ptr<Operator<Match> > res]
{	
	Lexicons empty_lex;
	ParsingScope scope(tagset, empty_lex);
}
	: res = functional_operator_match [scope]
	EOF
;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Rule for parsing single WCCL Rule
// Returns boost::shared_ptr<TagRule>
parse_single_tag_rule
	[const boost::shared_ptr<const Corpus2::Tagset> tagset]
	returns [boost::shared_ptr<TagRule> rle]
{	
	Lexicons empty_lex;
	ParsingScope scope(tagset, empty_lex);
}
	: rle = tag_rule [scope]
;

// Rule for parsing tag rule section in the wccl file
// Returns boost::shared_ptr<TagRuleSequence>
parse_tag_rule_sequence
	[const boost::shared_ptr<const Corpus2::Tagset> tagset]
	returns [boost::shared_ptr<TagRuleSequence> rule_seq]
{
	Lexicons empty_lex;
	ParsingScope scope(tagset, empty_lex);
}
	: rule_seq = tag_rules[scope]
;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Rule for parsing the match rules
// Returns boost::shared_ptr<Matching::MatchRule>
parse_match_rule
	[const boost::shared_ptr<const Corpus2::Tagset> tagset]
	returns [boost::shared_ptr<Matching::MatchRule> ret_match]
{
	Lexicons empty_lex;
	ParsingScope scope(tagset, empty_lex);
}
	: ret_match = match_rule_operator[scope] 
;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Rule for parsing wccl files
parse_wccl_file
	[const boost::shared_ptr<const Corpus2::Tagset> tagset, const std::string search_path]
	returns [boost::shared_ptr<WcclFile> wccl_file]
{
	wccl_file = boost::make_shared<WcclFile>(tagset, search_path);
}
	: (imports_section [*wccl_file])?
	  (wccl_file_section [*wccl_file])+
	  EOF
;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// VALUES
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// Single or multiple (comma separated) elements in string set, may be:
//   'a' "a" [] ['a'] ['a', 'b'] ["a"] ["a", "b"] ['a', "b"]
// Parsing strset literal and returning plain strset value.
// Returns boost::shared_ptr<StrSet> 
strset_literal 
	returns [boost::shared_ptr<StrSet> s_set]
{
	s_set.reset(new StrSet());
}
	: s0: STRING { 
			s_set->insert(token_ref_to_ustring(s0)); 
		}
	| LBRACKET 
		(
			s1: STRING { 
				s_set->insert(token_ref_to_ustring(s1)); 
			}
	  	(
				COMMA s2: STRING { 
					s_set->insert(token_ref_to_ustring(s2)); 
				}
			)*
	  )? 
		RBRACKET
;
// String set value as a constant string set:
// Returns boost::shared_ptr<Constant<StrSet> >
strset_value 
	returns [boost::shared_ptr<Constant<StrSet> > val]
{
	boost::shared_ptr<StrSet> set;
}
	: set = strset_literal {
		val.reset(new Constant<StrSet>(*set));
	}
;

// ----------------------------------------------------------------------------
// Element of sym set. This rule inserts element into symbol set 
// using corresponding tagset. 
// WARNING! This rule can throw ParserException! Be careful!
symset_elem
	[const boost::shared_ptr<const Corpus2::Tagset> tagset, boost::shared_ptr<TSet>& t_set]
	: s1: SYMBOL {
		try {
			t_set->insert_symbol(*tagset, str_token_rem_grav(s1));
		}
		catch(Corpus2::TagParseError &e) {
			throw(ParserException(e.info()));
		}
	}
;

// Symset literal. Symset element may be: 
//  a, `a ` (this is guaranteed by lexer rule - SYMBOL) or {a} {`a`} {a, b} 
//  {`a`, `b`} {a, `b`} {`a`, b}
// Parsing symset literal and returning plain symset value.
// Returns boost::shared_ptr<TSet>
symset_literal
	[const boost::shared_ptr<const Corpus2::Tagset> tagset]
	returns [boost::shared_ptr<TSet> t_set]
{
	t_set.reset(new TSet());
}
	: symset_elem [tagset, t_set]
	| LCURLY 
		(
			symset_elem [tagset, t_set] (COMMA symset_elem [tagset, t_set])* 
		)?
	  RCURLY
;
// Symset value, as constant symbol set
// Returns boost::shared_ptr<Constant<TSet> >
symset_value 
	[const boost::shared_ptr<const Corpus2::Tagset> tagset]
	returns [boost::shared_ptr<Constant<TSet> > val]
{
	boost::shared_ptr<TSet> set;
}
	: set = symset_literal [tagset] {
		val.reset(new Constant<TSet>(*set));
	}
;

// ----------------------------------------------------------------------------
// Bool literal. May be True or False. Parsing bool literal and returning 
// plain bool value.
// Returns boost::shared_ptr<Bool>
bool_literal
	returns [boost::shared_ptr<Bool> val]
	: "True"  { val.reset(new Bool(Bool(true ))); }
	| "False" { val.reset(new Bool(Bool(false))); }
;
// Bool value, as constat bool Value
// Returns boost::shared_ptr<Constant<Bool> >
bool_value 
	returns [boost::shared_ptr<Constant<Bool> > val]
{
	boost::shared_ptr<Bool> bool_lit;
}
	: bool_lit = bool_literal {
		val.reset(new Constant<Bool>(*bool_lit));
	}
;

// ----------------------------------------------------------------------------
// Position literal may be:
// 	(+|-)?(0-9)+ or  begin or end or nowhere
// Parsing position literal and returning plain position value.
// returns boost::shared_ptr<Position>
position_literal
	returns [boost::shared_ptr<Position> val]
{
	int i = 0;
}
	: i = number {
		val.reset(new Position(Position(i)));
	}
	| "begin" {
		val.reset(new Position(Position(Position::Begin)));
	}
	| "end" {
		val.reset(new Position(Position(Position::End)));
	}
	| "nowhere" {
		val.reset(new Position(Position(Position::Nowhere)));
	}
;

// Position as constant position value
// Returns boost::shared_ptr<Constant<Position> >
position_value
	returns [boost::shared_ptr<Constant<Position> > val]
{
	boost::shared_ptr<Position> pos_lit;
}
	: pos_lit = position_literal {
		val.reset(new Constant<Position>(*pos_lit));
	}
;

// ----------------------------------------------------------------------------
// Value used into match operator such as TOK[position] and ANN[position, name]
// Returns boost::shared_ptr<Match>
match_literal
	returns [boost::shared_ptr<Match> val]
{
	boost::shared_ptr<MatchData> m;
}
	: m = match_data_literal {
		val.reset(new Match(m));
	}
;

// Constant match value
// Returns boost::shared_ptr<Constant<Match> >
match_value_const
	returns [boost::shared_ptr<Constant<Match> > val]
{
	boost::shared_ptr<Match> m;
}
	: m = match_literal {
		val.reset(new Constant<Match>(*m));
	}
;

// ----------------------------------------------------------------------------
// Value used into match operator such as TOK[position] and ANN[position, name]
// Returns boost::shared_ptr<MatchData>
match_data_literal
	returns [boost::shared_ptr<MatchData> val]
	: val = token_match_literal
	| val = ann_match_literal
	| val = match_vector_literal
;

// token match literal - TOK[position]
// Returns boost::shared_ptr<TokenMatch>
token_match_literal
	returns [boost::shared_ptr<TokenMatch> val]
{
}
	: "TOK" LBRACKET u: UNSIGNED_INT RBRACKET {
		val.reset(new TokenMatch(token_ref_to_int(u)));
	}
;

// annotation match literal - ANN[position, name]
// Returns boost::shared_ptr<AnnotationMatch> 
ann_match_literal
	returns [boost::shared_ptr<AnnotationMatch> val]
{
}
	: "ANN" LBRACKET u : UNSIGNED_INT COMMA channel : STRING RBRACKET {
		val.reset(new AnnotationMatch(token_ref_to_int(u), token_ref_to_std_string(channel)));
	}
;

// annotation match vector literal: MATCH() or MATCH(token, ann, MATCH())
// Returns boost::shared_ptr<MatchVector>
match_vector_literal
	returns [boost::shared_ptr<MatchVector> val]
{
	val.reset(new MatchVector());
}
	: "MATCH" LPAREN (match_vector_literal_item[val])? RPAREN
;

// Body of the MATCH value. It only adds vector items to the MatchVector
// Item may be single or multiple
match_vector_literal_item [boost::shared_ptr<MatchVector>& mvector]
{
	boost::shared_ptr<Match> m_val;
}
	: m_val = match_literal {
		mvector->append(m_val);
	} 
	(
		COMMA
		m_val = match_literal {
			mvector->append(m_val);
		}
	)*
;

// ----------------------------------------------------------------------------
// Number may be unsigned or signed: 1, +1, -1
number 
	returns [int ret]
{
	ret = 0;
}
	: s: SIGNED_INT   { ret = token_ref_to_int(s); }
	| u: UNSIGNED_INT { ret = token_ref_to_int(u); }
;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// VARIABLES
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// Position: $Name
// Get position variable (however, before put into) from variables
// Returns boost::shared_ptr<VariableAccessor<Position> > 
position_variable_acc
	[Variables& vars] 
	returns [boost::shared_ptr<VariableAccessor<Position> > pos_acc]
	: POS_PREFIX n: SYMBOL { 
		vars.get_put<Position>(str_token_rem_grav(n));

		VariableAccessor<Position> acc = 
			vars.create_accessor<Position>(str_token_rem_grav(n));

		pos_acc.reset(new VariableAccessor<Position>(acc));
	}
;

// VarGetter for Position variable. This rule wrapped position_variable_acc.
// Returs boost::shared_ptr<VarGetter<Position> >
position_variable
	[Variables& vars] 
	returns [boost::shared_ptr<VarGetter<Position> > op]
{
	boost::shared_ptr<VariableAccessor<Position> > pos_acc;
}
	: pos_acc = position_variable_acc [vars] {
		op.reset(new VarGetter<Position>(*pos_acc));
	}
;

// ----------------------------------------------------------------------------
// String set, $s:name
// This expression gets variable of StrSet type from the scope's Variables.
// Creates one first if it's not yet there.
// 
// Returns boost::shared_ptr<VariableAccessor<StrSet> >
strset_variable_acc
	[Variables& vars] 
	returns [boost::shared_ptr<VariableAccessor<StrSet> > strset_acc]
	:	STR_PREFIX n: SYMBOL {
		vars.get_put<StrSet>(str_token_rem_grav(n));
		
		VariableAccessor<StrSet> acc = 
			vars.create_accessor<StrSet>(str_token_rem_grav(n));

		strset_acc.reset(new VariableAccessor<StrSet>(acc));
	}
;

// Vargetter for StrSet variable. This rule wrapped strset_variable_acc.
// Returns boost::shared_ptr<VarGetter<StrSet> > 
strset_variable 
	[Variables& vars] 
	returns [boost::shared_ptr<VarGetter<StrSet> > op]
{
	boost::shared_ptr<VariableAccessor<StrSet> > strset_acc;
}
	: strset_acc = strset_variable_acc [vars] {
		op.reset(new VarGetter<StrSet>(*strset_acc));
	}
;

// ----------------------------------------------------------------------------
// Symbol set: $t:name
// This expression gets variable of TSet type from the scope's Variables.
// Creates one first if it's not there yet.
// 
// Returns boost::shared_ptr<VariableAccessor<TSet> >
symset_variable_acc 
	[Variables& vars] 
	returns [boost::shared_ptr<VariableAccessor<TSet> > symset_acc]
	: TST_PREFIX n: SYMBOL { 
			vars.get_put<TSet>(str_token_rem_grav(n)); 	

			VariableAccessor<TSet> acc = 
				vars.create_accessor<TSet>(str_token_rem_grav(n));

			symset_acc.reset(new VariableAccessor<TSet>(acc));
	}
;

// Vargetter for symbol set variable. This rule wrapped symset_variable_acc
// Returns boost::shared_ptr<VarGetter<TSet> > 
symset_variable 
	[Variables& vars] 
	returns [boost::shared_ptr<VarGetter<TSet> > op]
{
	boost::shared_ptr<VariableAccessor<TSet> > symset_acc;
}
	: symset_acc = symset_variable_acc [vars] {
			op.reset(new VarGetter<TSet>(*symset_acc));
	}
;

// ----------------------------------------------------------------------------
// Bool: $b:name
// This expression gets variable of Bool type from the scope's Variables.
// Creates one first if it's not there yet.
// Returns boost::shared_ptr<VariableAccessor<Bool> >
bool_variable_acc
	[Variables& vars] 
	returns [boost::shared_ptr<VariableAccessor<Bool> > bool_acc]
	: BOOL_PREFIX n: SYMBOL { 
			vars.get_put<Bool>(str_token_rem_grav(n));

			VariableAccessor<Bool> acc = 
				vars.create_accessor<Bool>(str_token_rem_grav(n));

			bool_acc.reset(new VariableAccessor<Bool>(acc));
	}
;

// Vargetter for bool variable. It is only wrapper for bool_variable_acc
// Returns boost::shared_ptr<VarGetter<Bool> >
bool_variable
	[Variables& vars] 
	returns [boost::shared_ptr<VarGetter<Bool> > op]
{
	boost::shared_ptr<VariableAccessor<Bool> > bool_acc;
}
	: bool_acc = bool_variable_acc [vars] {
			op.reset(new VarGetter<Bool>(*bool_acc));
	}
;

// ----------------------------------------------------------------------------
// Match: $m:name
// This expression gets variable of Match type from the scope's Variables.
// Creates one first if it's not there yet.
// Returns boost::shared_ptr<VariableAccessor<Match> >
match_vector_variable_acc
	[Variables& vars]
	returns [boost::shared_ptr<VariableAccessor<Match> > mvv_acc]
	: MATCH_VECTOR_PREFIX n: SYMBOL {
			vars.get_put<Match>(str_token_rem_grav(n));

			VariableAccessor<Match> acc = 
				vars.create_accessor<Match>(str_token_rem_grav(n));

			mvv_acc.reset(new VariableAccessor<Match>(acc));
	}
;

// Vargetter for the match vector variable. Wrapper for match_vector_variable_acc
// Returns boost::shared_ptr<VarGetter<Match> >
match_vector_variable
	[Variables& vars]
	returns [boost::shared_ptr<VarGetter<Match> > mvv]
{
	boost::shared_ptr<VariableAccessor<Match> > mvv_acc;
}
	: mvv_acc = match_vector_variable_acc [vars] {
		mvv.reset(new VarGetter<Match>(*mvv_acc));
	}
;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// OPERATORS
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Symbol set (tagset) operators
// Returns boost::shared_ptr<Function<TSet> >
///////////////////////////////////////////////////////////////////////////////
symset_operator
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<TSet> > ret]
	: ret = symset_condition [scope]
	| {LA(1)==SYMBOL && LA(2)==LBRACKET}? (ret = symset_getsymbol [scope])
	| ret = symset_var_val [scope]
	| ret = symset_class   [scope]
	| ret = symset_range   [scope]
	| ret = symset_catflt  [scope]
	| ret = symset_agrflt  [scope]
	| ret = symset_union   [scope]
	| ret = symset_intersection  [scope]
	//
	| LPAREN ret = symset_operator [scope] RPAREN
;

// ----------------------------------------------------------------------------
// comma-separated symset operators
symset_operator_comma_sep
	[ParsingScope& scope]
	returns
		[boost::shared_ptr<std::vector<boost::shared_ptr<Function<TSet> > > > ret_v]
{
	boost::shared_ptr<Function<TSet> > pred;
	ret_v.reset(
		new std::vector<boost::shared_ptr<Function<TSet> > >
	);
}
	: pred = symset_operator [scope] {
		ret_v->push_back(pred);
	}
	(
		COMMA pred = symset_operator [scope] {
			ret_v->push_back(pred);
		}
	)*
;

// ----------------------------------------------------------------------------
// Wrapper from Function<TSet> to Operator<TSet>
functional_operator_symset
	[ParsingScope& scope]
	returns [boost::shared_ptr<Operator<TSet> > op]
{
	boost::shared_ptr<Function<TSet> > body;
}
	: body = symset_operator [scope] {
		op.reset(new Operator<TSet>(body, scope.variables()));
	}
;

// ----------------------------------------------------------------------------
// A wrapper for symset variable and symset value.
symset_var_val
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<TSet> > op]
	: op = symset_variable [scope.variables()]
	| op = symset_value    [scope.tagset()]
;

// ----------------------------------------------------------------------------
// Condition of the symset value:
// 	if (Bool, TSet, TSet)
// 	? TSet ? Bool : {}
symset_condition
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<TSet> > op]
{
	boost::shared_ptr<Function<Bool> > test;
	boost::shared_ptr<Function<TSet> > p_true, p_false;
}
	: "if" LPAREN test  = bool_operator [scope] COMMA 
							p_true  = symset_operator  [scope] 
							(COMMA p_false = symset_operator [scope])? 
		RPAREN {
			if (p_false) {
				op.reset(new Conditional<TSet>(test, p_true, p_false));
			}
			else {
				op.reset(new Conditional<TSet>(test, p_true));
			}
		}
	| Q_MARK 
			(p_true = symset_operator [scope])
		Q_MARK 
			(test = bool_operator [scope]) {
			op.reset(new Conditional<TSet>(test, p_true));
		}
;

// ----------------------------------------------------------------------------
// GetSymbol operator may be cas, m1, f, sg...
// WARNING! This rule can throw ParserException! Be careful!
symset_getsymbol
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<TSet> > op]
{
	Corpus2::Tag tag;
	boost::shared_ptr<Wccl::Function<Position> > position;
}
	: t: SYMBOL LBRACKET position = position_operator [scope] RBRACKET {
			try {
				tag = scope.tagset()->parse_symbol(str_token_rem_grav(t));
			}
			catch(Corpus2::TagParseError &e) {
				throw(ParserException(e.info()));
			}
				
			op.reset(new Wccl::GetSymbols(tag, position));
		}
;

// ----------------------------------------------------------------------------
// Class operator.
symset_class 
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<TSet> > ret]
{
	boost::shared_ptr<Function<Position> > pos;
}
	: "class" LBRACKET pos = position_operator [scope] RBRACKET { 
		ret.reset(new GetWordClass(pos));
	}
;

// ----------------------------------------------------------------------------
// Range operator: range(class, begin, end) or range({...}, begin, end)
symset_range
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<TSet> > ret]
{
	boost::shared_ptr<TSet> tset;
	boost::shared_ptr<Function<Position> > p1, p2;
}
	: "range" LPAREN
			(tset = symset_literal [scope.tagset()] | tag_class: "class") COMMA
			p1  = position_operator [scope] COMMA 
			p2  = position_operator [scope] 
		RPAREN {
			if (tag_class) {
				ret.reset(new GetSymbolsInRange(Corpus2::Tag::all_pos_mask, p1, p2));
			}
			else {
		 		ret.reset(new GetSymbolsInRange(tset->get_value(), p1, p2));
			}
		}
;

// ----------------------------------------------------------------------------
// Catflt operator
symset_catflt
  [ParsingScope& scope]
  returns [boost::shared_ptr<Function<TSet> > ret]
{
  boost::shared_ptr<Function<TSet> > selector, mask;
  boost::shared_ptr<Function<Position> > position;
}
  : "catflt" LPAREN 
			position = position_operator [scope] COMMA
			selector = symset_operator   [scope] COMMA
			mask    = symset_operator    [scope] 
  	RPAREN {
  	  ret.reset(new CatFilter(position, selector, mask));
  	}
;

// ----------------------------------------------------------------------------
// Agrflt operator
symset_agrflt
  [ParsingScope& scope]
  returns [boost::shared_ptr<Function<TSet> > ret]
{
  boost::shared_ptr<Function<TSet> > attr, mask;
  boost::shared_ptr<Function<Position> > lpos, rpos;
}
  : "agrflt" LPAREN 
			lpos = position_operator [scope] COMMA
			rpos = position_operator [scope] COMMA
			attr = symset_operator   [scope] COMMA
			mask = symset_operator   [scope] 
  	RPAREN {
  	  ret.reset(new AgrFilter(lpos, rpos, attr, mask, scope.tagset()));
  	}
;

// ----------------------------------------------------------------------------
// Union operator
symset_union
  [ParsingScope& scope]
  returns [boost::shared_ptr<Function<TSet> > ret]
{
  boost::shared_ptr< std::vector< boost::shared_ptr<Function<TSet> > > > sets;
}
  : "union" LPAREN
			sets = symset_operator_comma_sep   [scope]
  	RPAREN {
  	  ret.reset(new SetUnion<TSet>(sets));
  	}
;

// ----------------------------------------------------------------------------
// Intersection operator
symset_intersection
  [ParsingScope& scope]
  returns [boost::shared_ptr<Function<TSet> > ret]
{
  boost::shared_ptr< std::vector< boost::shared_ptr<Function<TSet> > > > sets;
}
  : "intersection" LPAREN
			sets = symset_operator_comma_sep   [scope]
  	RPAREN {
  	  ret.reset(new SetIntersection<TSet>(sets));
  	}
;

///////////////////////////////////////////////////////////////////////////////
// Position operator
// Returns boost::shared_ptr<Function<Position> >
///////////////////////////////////////////////////////////////////////////////
position_operator
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Position> > ret]
	: 
	( ret = position_var_val     [scope.variables()]
	| ret = position_condition   [scope]
	| ret = position_first_token [scope]
	| ret = position_last_token  [scope]
	| LPAREN ret = position_operator [scope] RPAREN
	) 
	( // if there is SIGNED_INT after the position, it is actually a relative position
		i: SIGNED_INT {
			ret.reset(new RelativePosition(ret, token_ref_to_int(i)));
		}
	)?
;

// ----------------------------------------------------------------------------
// Wrapper from Function<Position> to Operator<Position>
functional_operator_position
	[ParsingScope& scope]
	returns [boost::shared_ptr<Operator<Position> > op]
{
	boost::shared_ptr<Function<Position> > body;
}
	: body = position_operator [scope] {
		op.reset(new Operator<Position>(body, scope.variables()));
	}
;

// ----------------------------------------------------------------------------
// Wrapper for position variable and position value
position_var_val
	[Variables& vars]
	returns [boost::shared_ptr<Function<Position> > ret]
	: ret = position_value
	| ret = position_variable [vars]
;

// ----------------------------------------------------------------------------
// Condition of the position value
// if (Bool, Position, Position)
// ? Position ? Bool : 0
position_condition
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Position> > op]
{
	boost::shared_ptr<Function<Bool> > test;
	boost::shared_ptr<Function<Position> > p_true, p_false;
}
	: "if" LPAREN test  = bool_operator [scope] COMMA 
							p_true  = position_operator [scope] 
							(COMMA p_false = position_operator [scope])? 
	RPAREN {
		if (p_false) {
			op.reset(new Conditional<Position>(test, p_true, p_false));
		}
		else {
			op.reset(new Conditional<Position>(test, p_true));
		}
	}
	| Q_MARK 
			p_true = position_operator [scope]
		Q_MARK 
			test = bool_operator [scope] {
			op.reset(new Conditional<Position>(test, p_true));
		}
;

// ----------------------------------------------------------------------------
// Taking position of a first token in a match
// first(Match)
position_first_token [ParsingScope& scope]
	returns [boost::shared_ptr<Function<Position> > ret]
{
	boost::shared_ptr<Function<Match> > m;
}
	: "first" LPAREN m = match_operator [scope] RPAREN {
		ret.reset(new FirstToken(m));
	}
;

// ----------------------------------------------------------------------------
// Taking position of a first token in a match
// last(Match)
position_last_token [ParsingScope& scope]
	returns [boost::shared_ptr<Function<Position> > ret]
{
	boost::shared_ptr<Function<Match> > m;
}
	: "last" LPAREN m = match_operator [scope] RPAREN {
		ret.reset(new LastToken(m));
	}
;

///////////////////////////////////////////////////////////////////////////////
// String operator
// Returns boost::shared_ptr<Function<StrSet> >
///////////////////////////////////////////////////////////////////////////////
strset_operator [ParsingScope& scope]
	returns [boost::shared_ptr<Function<StrSet> > ret]
	: ret = strset_orth      [scope] 
	| ret = strset_base      [scope]
	| ret = strset_prop      [scope]
	| ret = strset_lower     [scope] 
	| ret = strset_upper     [scope]
	| ret = strset_affix     [scope] 
	| ret = strset_var_val   [scope] 
	| ret = strset_condition [scope]
	| ret = strset_union     [scope]
	| ret = strset_intersection [scope]
	| ret = strset_anninter [scope]
	| ret = strset_lex       [scope]
	//
	| LPAREN ret = strset_operator [scope] RPAREN
;

// ----------------------------------------------------------------------------
// comma-separated strset operators
strset_operator_comma_sep
	[ParsingScope& scope]
	returns
		[boost::shared_ptr< std::vector<boost::shared_ptr<Function<StrSet> > > > ret_v]
{
	boost::shared_ptr<Function<StrSet> > pred;
	ret_v.reset(
		new std::vector<boost::shared_ptr<Function<StrSet> > >
	);
}
	: pred = strset_operator [scope] {
		ret_v->push_back(pred);
	}
	(
		COMMA pred = strset_operator [scope] {
			ret_v->push_back(pred);
		}
	)*
;


// ----------------------------------------------------------------------------
// Wrapper from Function<StrSet> to Operator<StrSet>
functional_operator_strset
	[ParsingScope& scope]
	returns [boost::shared_ptr<Operator<StrSet> > op]
{
	boost::shared_ptr<Function<StrSet> > body;
}
	: body = strset_operator [scope] {
		op.reset(new Operator<StrSet>(body, scope.variables()));
	}
;
// ----------------------------------------------------------------------------
// Orth operator.
strset_orth 
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<StrSet> > ret]
{
	boost::shared_ptr<Function<Position> > pos;
}
	: "orth" LBRACKET pos = position_operator [scope] RBRACKET { 
			ret.reset(new GetOrth(pos));
	}
;

// ----------------------------------------------------------------------------
// Base operator.
strset_base 
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<StrSet> > ret]
{
	boost::shared_ptr<Function<Position> > pos;
}
	: "base" LBRACKET pos = position_operator [scope] RBRACKET { 
		ret.reset(new GetLemmas(pos));
	}
;

// ----------------------------------------------------------------------------
// Token-level property value operator.
strset_prop
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<StrSet> > ret]
{
	boost::shared_ptr<Function<Position> > pos;
	boost::shared_ptr<Function<StrSet> > keys;
}
	: "prop" LPAREN 
			pos = position_operator [scope] COMMA
			keys = strset_operator [scope]
	RPAREN {
		ret.reset(new PropVal(pos, keys));
	}
;

// ----------------------------------------------------------------------------
// Lower operator.
strset_lower 
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<StrSet> > ret]
{
	boost::shared_ptr<Function<StrSet> > o_ret;
}
	: "lower" LPAREN o_ret = strset_operator [scope] RPAREN {
		ret.reset(new ToLower(o_ret));
	}
;

// ----------------------------------------------------------------------------
// Upper operator.
strset_upper 
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<StrSet> > ret]
{
	boost::shared_ptr<Function<StrSet> > o_ret;
}
	: "upper" LPAREN o_ret = strset_operator [scope] RPAREN {
		ret.reset(new ToUpper(o_ret));
	}
;

// ----------------------------------------------------------------------------
// Affix operator.
strset_affix 
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<StrSet> > ret]
{
	int offset = 0;
	boost::shared_ptr<Function<StrSet> > o_ret;
}
	: "affix" LPAREN 
			o_ret = strset_operator [scope] COMMA offset = number 
		RPAREN {
			ret.reset(new Affix(o_ret, offset));
		}
;

// ----------------------------------------------------------------------------
// A wrapper for strset value and strset variable
strset_var_val
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<StrSet> > op]
	: op = strset_value 
	| op = strset_variable [scope.variables()]
;

// ----------------------------------------------------------------------------
// Condition of the strset value
// if (Bool, StrSet, StrSet)
// ? StrSet ? Bool : []
strset_condition
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<StrSet> > op]
{
	boost::shared_ptr<Function<Bool> > test;
	boost::shared_ptr<Function<StrSet> > p_true, p_false;
}
	: "if" LPAREN test  = bool_operator [scope] COMMA 
							p_true  = strset_operator   [scope] 
							(COMMA p_false = strset_operator [scope])? 
	RPAREN {
		if (p_false) {
			op.reset(new Conditional<StrSet>(test, p_true, p_false));
		}
		else {
			op.reset(new Conditional<StrSet>(test, p_true));
		}
	}
	| Q_MARK 
			p_true = strset_operator [scope]
		Q_MARK 
			test = bool_operator [scope] {
			op.reset(new Conditional<StrSet>(test, p_true));
		}
;

// ----------------------------------------------------------------------------
// Union operator, strset
strset_union
  [ParsingScope& scope]
  returns [boost::shared_ptr<Function<StrSet> > ret]
{
  boost::shared_ptr<std::vector<boost::shared_ptr<Function<StrSet> > > > sets;
}
  : "union" LPAREN
			sets = strset_operator_comma_sep   [scope]
  	RPAREN {
  	  ret.reset(new SetUnion<StrSet>(sets));
  	}
;

// ----------------------------------------------------------------------------
// Intersection operator, strset
strset_intersection
  [ParsingScope& scope]
  returns [boost::shared_ptr<Function<StrSet> > ret]
{
  boost::shared_ptr<std::vector<boost::shared_ptr<Function<StrSet> > > > sets;
}
  : "intersection" LPAREN
			sets = strset_operator_comma_sep   [scope]
  	RPAREN {
  	  ret.reset(new SetIntersection<StrSet>(sets));
  	}
;

strset_anninter
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<StrSet> > op]
{
	boost::shared_ptr<Function<Position> > pos;
	boost::shared_ptr<Function<StrSet> > in_strs;
}
	: "anninter"
	LPAREN
		pos = position_operator [scope]
		COMMA
		in_strs = strset_operator [scope]
	RPAREN
	{
		op.reset(new AnnInter(pos, in_strs));
	}
;

strset_lex
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<StrSet> > op]
{
	boost::shared_ptr<Function<StrSet> > s;
}
	: "lex" LPAREN s = strset_operator [scope] COMMA name : STRING RPAREN 
		{
			op.reset(new LexTranslator(
				s,
				scope.lexicons().get_ptr(token_ref_to_std_string(name))));
		}
		exception catch [WcclError ex] {
			throw ParserException(ex.what());
		}
;
///////////////////////////////////////////////////////////////////////////////
// Bool operator 
// Returns boost::shared_ptr<Function<Bool> >
///////////////////////////////////////////////////////////////////////////////
bool_operator
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > ret]
	: ret = bool_and        [scope]  
	| ret = bool_or         [scope]  
	| ret = bool_nor        [scope]  
	| ret = bool_var_val    [scope]	
	| ret = bool_regex      [scope]
	| ret = bool_inout      [scope]
	| ret = bool_condition  [scope]
	// setvar:
	| ret = setvar_operator [scope]
	// empty
	| ret = empty_operator  [scope]
	// equal/in/inter:
	| ret = equal_operator  [scope]
	| ret = in_operator     [scope]
	| ret = inter_operator  [scope]
	// iterations
	| ret = bool_iteration  [scope]
	// agreement
	| ret = bool_agreement  [scope]
	//
	| ret = bool_phrase     [scope]
	// annotation
	| ret = bool_ann        [scope]
	| ret = bool_annsub     [scope]
	// singular/amb
	| ret = bool_ambiguous  [scope]
	| ret = bool_singular   [scope]
	// debug operators
	| ret = debug_print_operator [scope]
	//
	| LPAREN ret = bool_operator [scope] RPAREN
;

// ----------------------------------------------------------------------------
// wrapper from Function<Bool> to Operator<Bool>
functional_operator_bool
	[ParsingScope& scope]
	returns [boost::shared_ptr<Operator<Bool> > op]
{
	boost::shared_ptr<Function<Bool> > body;
}
	: body = bool_operator [scope] {
		op.reset(new Operator<Bool>(body, scope.variables()));
	}
;

// ----------------------------------------------------------------------------
// comma-separated predicates (bool operators)
bool_operator_comma_sep
	[ParsingScope& scope]
	returns 
		[boost::shared_ptr<std::vector<boost::shared_ptr<Function<Bool> > > > ret_v]
{
	boost::shared_ptr<Function<Bool> > pred;
	ret_v.reset(
		new std::vector<boost::shared_ptr<Function<Bool> > >
	);
}
	: pred = bool_operator [scope] { 
		ret_v->push_back(pred);
	} 
	(
		COMMA pred = bool_operator [scope] {
			ret_v->push_back(pred);
		}
	)*
;

// ----------------------------------------------------------------------------
// And operator.
bool_and 
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
{
	boost::shared_ptr<std::vector<boost::shared_ptr<Function<Bool> > > > ret_v;
}
	: "and" LPAREN ret_v = bool_operator_comma_sep [scope] RPAREN {
			op.reset(new And(ret_v));
	}
;

// ----------------------------------------------------------------------------
// Or operator
bool_or
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
{
	boost::shared_ptr<std::vector<boost::shared_ptr<Function<Bool> > > > ret_v;
}
	: "or" LPAREN ret_v = bool_operator_comma_sep [scope] RPAREN {
			op.reset(new Or(ret_v));
	}
;

// ----------------------------------------------------------------------------
// Nor/Not operator
bool_nor
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
{
	boost::shared_ptr<std::vector<boost::shared_ptr<Function<Bool> > > > ret_v;
}
	: "not" LPAREN ret_v = bool_operator_comma_sep [scope] RPAREN {
			op.reset(new Nor(ret_v));
	}
;

// ----------------------------------------------------------------------------
// Wrapper for bool value and bool variable
bool_var_val
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
	: op = bool_value 
	| op = bool_variable [scope.variables()]
;

// ----------------------------------------------------------------------------
// Regex operator
bool_regex
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
{
	boost::shared_ptr<Function<StrSet> > expr;
}
	: "regex" 
		LPAREN 
			expr = strset_operator [scope] COMMA reg: STRING 
		RPAREN {
			op.reset(new Regex(expr, token_ref_to_ustring(reg)));
		}
;

// ----------------------------------------------------------------------------
// Input/output operator
bool_inout
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
{
	boost::shared_ptr<Function<Position> > ret_pos;
}
	: "inside"  LPAREN ret_pos = position_operator [scope] RPAREN {
		op.reset(new IsInside(ret_pos));
	}
	| "outside" LPAREN ret_pos = position_operator [scope] RPAREN {
		op.reset(new IsOutside(ret_pos));
	}
;

// ----------------------------------------------------------------------------
// if (Bool, Bool, Bool)
// ? Bool ? Bool : False
bool_condition
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
{
	boost::shared_ptr<Function<Bool> > test, p_true, p_false;
}
	: "if" LPAREN test = bool_operator [scope] COMMA 
							p_true = bool_operator [scope] 
							(COMMA p_false = bool_operator [scope])? 
	RPAREN {
		if (p_false) {
			op.reset(new Conditional<Bool>(test, p_true, p_false));
		}
		else {
			op.reset(new Conditional<Bool>(test, p_true));
		}
	}
	| Q_MARK 
			p_true = bool_operator [scope]
		Q_MARK 
			test = bool_operator [scope] {
			op.reset(new Conditional<Bool>(test, p_true));
		}
;

// ----------------------------------------------------------------------------
// Equal operator
equal_operator
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
{
	boost::shared_ptr<Function<TSet> > t1, t2;
	boost::shared_ptr<Function<Bool> > b1, b2;
	boost::shared_ptr<Function<StrSet>  > s1, s2;
	boost::shared_ptr<Function<Position> > p1, p2;
}
	: "equal" LPAREN
	(
		(position_operator [scope]) =>
		(
			p1 = position_operator [scope] COMMA 
			p2 = position_operator [scope] {
				op.reset(new Equals<Position>(p1, p2));
			}
		)
	|
		(symset_operator [scope]) =>
		(
			t1 = symset_operator [scope] COMMA  
			t2 = symset_operator [scope] {
				op.reset(new Equals<TSet>(t1, t2));
			}
		)
	|
		(strset_operator [scope]) =>
		(
			s1 = strset_operator [scope] COMMA  
			s2 = strset_operator [scope] {
				op.reset(new Equals<StrSet>(s1, s2));
			}
		)
	|
		(
			b1 = bool_operator [scope] COMMA
			b2 = bool_operator [scope] {
				op.reset(new Equals<Bool>(b1, b2));
			}
		)
	)
	RPAREN
;

// ----------------------------------------------------------------------------
// In operator
in_operator
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
{
	boost::shared_ptr<Function<TSet> > t1, t2;
	boost::shared_ptr<Function<StrSet> > s1, s2;
}
:
	"in" LPAREN
	(
		(symset_operator [scope]) =>
		(
			t1 = symset_operator [scope] COMMA 
			t2 = symset_operator [scope] {
				op.reset(new IsSubsetOf<TSet>(t1, t2));
			}
		)
		|
		(
			s1 = strset_operator [scope] COMMA
			s2 = strset_operator [scope] {
				op.reset(new IsSubsetOf<StrSet>(s1, s2));
			}
		)
	)
	RPAREN
;
// ----------------------------------------------------------------------------
// Inter operator
inter_operator
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
{
	boost::shared_ptr<Function<TSet> > t1, t2;
	boost::shared_ptr<Function<StrSet> > s1, s2;
}
	:
	"inter" LPAREN
	(
		(symset_operator [scope]) =>
		(
			t1 = symset_operator [scope] COMMA  
			t2 = symset_operator [scope]  {
				op.reset(new Intersects<TSet>(t1, t2));
			}
		)
	|
		(
			s1 = strset_operator [scope] COMMA  
			s2 = strset_operator [scope]  {
				op.reset(new Intersects<StrSet>(s1, s2));
			}
		)
	)
	RPAREN
;

// ----------------------------------------------------------------------------
// Annotation operator.
bool_ann
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
{
	boost::shared_ptr< Function<Match> > match_from;
	boost::shared_ptr< Function<Match> > match_to;
}
	: "ann" LPAREN
			match_from = match_operator [scope] COMMA
			(match_to  = match_operator [scope] COMMA)?
			channel : STRING
		RPAREN {
			if (match_to) {
				op.reset(new Ann(match_from, match_to, token_ref_to_std_string(channel)));
			} else {
				op.reset(new Ann(match_from, token_ref_to_std_string(channel)));
			}
		}
;


// ----------------------------------------------------------------------------
// Annotation-sub operator.
bool_annsub
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
{
	boost::shared_ptr< Function<Match> > match_from;
	boost::shared_ptr< Function<Match> > match_to;
}
	: "annsub" LPAREN
		match_from = match_operator [scope] COMMA
		(match_to = match_operator [scope] COMMA)?
		channel : STRING
		RPAREN
		{
			if (match_to) {
				op.reset(new AnnSub(match_from, match_to, token_ref_to_std_string(channel)));
			} else {
				op.reset(new AnnSub(match_from, token_ref_to_std_string(channel)));
			}
		}
;

// ----------------------------------------------------------------------------
// Ambiguity checking operator
bool_ambiguous
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > ret]
{
	boost::shared_ptr<Function<TSet> > tf;
	boost::shared_ptr<Function<StrSet> > sf;
	boost::shared_ptr<Function<Position> > pf;
}
	: "ambiguous" LPAREN
	(
		(position_operator [scope]) =>
		(
			pf = position_operator [scope] {
				ret.reset(new IsAmbiguous<Position>(pf));
			}
		)
	|
		(symset_operator [scope]) =>
		(
			tf = symset_operator [scope] {
				ret.reset(new IsAmbiguous<TSet>(tf));
			}
		)
	|
		(strset_operator [scope]) =>
		(
			sf = strset_operator [scope] {
				ret.reset(new IsAmbiguous<StrSet>(sf));
			}
		)
	)
	RPAREN
;

// ----------------------------------------------------------------------------
// Tag singularity checking operator
bool_singular
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > ret]
{
	boost::shared_ptr< Function<TSet> > v;
}
	: "singular" LPAREN
		v = symset_operator [scope]
		RPAREN
		{
			ret.reset(new IsSingular(v, scope.tagset()));
		}
;

// ----------------------------------------------------------------------------
// Debug printing:
debug_print_operator
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > ret]
{
	boost::shared_ptr<FunctionBase> v;
}
	: "debug" LPAREN
	(
		(position_operator [scope]) =>
		(
			v = position_operator [scope] {
				ret.reset(new DebugPrint(v));
			}
		)
	|
		(symset_operator [scope]) =>
		(
			v = symset_operator [scope] {
				ret.reset(new DebugPrint(v));
			}
		)
	|
		(strset_operator [scope]) =>
		(
			v = strset_operator [scope] {
				ret.reset(new DebugPrint(v));
			}
		)
	|
		(bool_operator [scope]) =>
		(
			v = bool_operator [scope] {
				ret.reset(new DebugPrint(v));
			}
		)
	|
		(
			v = match_operator [scope] {
				ret.reset(new DebugPrint(v));
			}
		)
	)
	RPAREN
;

// ----------------------------------------------------------------------------
// Iterations:
bool_iteration
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > ret]
{
	int min_match = 0;
	int offset = 0;
	boost::shared_ptr<Function<Bool> > expr;
	boost::shared_ptr<Function<Position> > lpos, rpos;
	boost::shared_ptr<VariableAccessor<Position> > pacc;
}
	: "only" LPAREN 
			lpos = position_operator [scope] COMMA 
			rpos = position_operator [scope] COMMA
			pacc = position_variable_acc [scope.variables()]     COMMA
			expr = bool_operator     [scope]
		RPAREN {
			ret.reset(new Only(lpos, rpos, *pacc, expr));
		}

	| "atleast" LPAREN
			lpos = position_operator [scope] COMMA 
			rpos = position_operator [scope] COMMA
			pacc = position_variable_acc [scope.variables()]     COMMA
			expr = bool_operator     [scope] COMMA
			min_match = number
		RPAREN {
			ret.reset(new AtLeast(lpos, rpos, *pacc, expr, min_match));
		}
	| "llook" LPAREN //note inverted rpos/lpos order
			rpos = position_operator [scope] COMMA 
			lpos = position_operator [scope] COMMA
			pacc = position_variable_acc [scope.variables()]     COMMA
			expr = bool_operator     [scope] 
		RPAREN {
			ret.reset(new LeftLook(lpos, rpos, *pacc, expr));
		}
	| "rlook" LPAREN
			lpos = position_operator [scope] COMMA 
			rpos = position_operator [scope] COMMA
			pacc = position_variable_acc [scope.variables()]     COMMA
			expr = bool_operator     [scope] 
		RPAREN {
			ret.reset(new RightLook(lpos, rpos, *pacc, expr));
		}
	| "skip" LPAREN
			lpos = position_operator [scope] COMMA 
			pacc = position_variable_acc [scope.variables()]     COMMA
			expr = bool_operator     [scope] COMMA
			offset = number
		RPAREN {
			ret.reset(new Skip(lpos, *pacc, expr, offset));
		}
;

// ----------------------------------------------------------------------------
// Agreement operator: agr, agrpp, wagr
bool_agreement
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > ret]
{
	boost::shared_ptr<Function<TSet> > expr;
	boost::shared_ptr<Function<Position> > lpos, rpos;
}
	: "agr"   LPAREN 
			lpos = position_operator [scope] COMMA 
			rpos = position_operator [scope] COMMA 
			expr = symset_operator [scope]
		RPAREN {
			ret.reset(new StrongAgreement(lpos, rpos, expr, scope.tagset()));
		}
	| "agrpp" LPAREN
			lpos = position_operator [scope] COMMA 
			rpos = position_operator [scope] COMMA 
			expr = symset_operator [scope]
		RPAREN {
			ret.reset(new PointAgreement(lpos, rpos, expr, scope.tagset()));
		}
	| "wagr"  LPAREN
			lpos = position_operator [scope] COMMA 
			rpos = position_operator [scope] COMMA 
			expr = symset_operator [scope]
		RPAREN {
			ret.reset(new WeakAgreement(lpos, rpos, expr, scope.tagset()));
		}
;

// ----------------------------------------------------------------------------
// Parse operator on L1 level
bool_phrase
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > ret]
	: ret = bool_phrase_annotation [scope]
	| ret = bool_phrase_iteration  [scope]
;

// ----------------------------------------------------------------------------
// Annotation operator: phrase, phrase_beg, phrase_end, phrase_whole, phrase_pp
bool_phrase_annotation
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > ret]
{
	boost::shared_ptr<Function<Position> > lpos, rpos;
}
	: "isannpart" LPAREN 
			lpos = position_operator [scope] COMMA n1: STRING 
		RPAREN {
			ret.reset(new AnnToken(lpos, token_ref_to_std_string(n1), AnnToken::O_ANY));
		}
	| "isannbeg" LPAREN 
			lpos = position_operator [scope] COMMA n2: STRING 
		RPAREN {
			ret.reset(new AnnToken(lpos, token_ref_to_std_string(n2), AnnToken::O_FIRST));
		}
	| "isannend" LPAREN 
			lpos = position_operator [scope] COMMA n3: STRING 
		RPAREN {
			ret.reset(new AnnToken(lpos, token_ref_to_std_string(n3), AnnToken::O_LAST));
		}
	| "isannhead" LPAREN 
			lpos = position_operator [scope] COMMA n4: STRING 
		RPAREN {
			ret.reset(new AnnToken(lpos, token_ref_to_std_string(n4), AnnToken::O_HEAD));
		}
	| "isannwhole" LPAREN 
			lpos = position_operator [scope] COMMA 
			rpos = position_operator [scope] COMMA n5: STRING 
		RPAREN {
			// TODO
		}
	| "isannpp" LPAREN 
			lpos = position_operator [scope] COMMA 
			rpos = position_operator [scope] COMMA n6: STRING 
		RPAREN {
			// TODO
		}
;

// ----------------------------------------------------------------------------
// Phrase iteration operator: lphrase, rphrase
bool_phrase_iteration
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > ret]
{
	boost::shared_ptr<Function<Position> > position;
	boost::shared_ptr<VarGetter<Position> > var_position;
}
	: "lanniter" LPAREN
			position     = position_operator [scope] COMMA
			var_position = position_variable [scope.variables()]         COMMA
			n1: STRING
		RPAREN {
			// TODO
		}
	| "ranniter" LPAREN 
			position     = position_operator [scope] COMMA
			var_position = position_variable [scope.variables()]         COMMA
			n2: STRING
		RPAREN {
			// TODO
		}
;


// ----------------------------------------------------------------------------
// Setvar operator
// Returns boost::shared_ptr<Function<Bool> >
// ----------------------------------------------------------------------------
setvar_operator 
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > ret]
	: "setvar" LPAREN
	  (
		  ret = position_setvar [scope]
		| ret = bool_setvar     [scope]
		| ret = strset_setvar   [scope]
		| ret = symset_setvar   [scope]
	  )
	  RPAREN
;

// ----------------------------------------------------------------------------
// Setvar for position
position_setvar 
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
{
	boost::shared_ptr<Function<Position> > ret_op;
	boost::shared_ptr<VariableAccessor<Position> > ret_acc;
}
	:	ret_acc = position_variable_acc [scope.variables()]
		COMMA
		ret_op  = position_operator [scope] {
			op.reset(new VarSetter<Position>(*ret_acc, ret_op));
		}
;

// ----------------------------------------------------------------------------
// Setvar for bool
bool_setvar 
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
{
	boost::shared_ptr<Function<Bool> > ret_op;
	boost::shared_ptr<VariableAccessor<Bool> > ret_acc;
}
	:	ret_acc = bool_variable_acc [scope.variables()]
		COMMA
		ret_op  = bool_operator [scope] {
			op.reset(new VarSetter<Bool>(*ret_acc, ret_op));
		}
;

// ----------------------------------------------------------------------------
// Setvar for strset
strset_setvar
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
{
	boost::shared_ptr<Function<StrSet> > ret_op;
	boost::shared_ptr<VariableAccessor<StrSet> > ret_acc;
}
	: ret_acc = strset_variable_acc [scope.variables()]
	  COMMA 
		ret_op  = strset_operator [scope] {
			op.reset(new VarSetter<StrSet>(*ret_acc, ret_op));
		}
;

// ----------------------------------------------------------------------------
// Setvar for symset
symset_setvar 
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
{
	boost::shared_ptr<Function<TSet> > ret_op;
	boost::shared_ptr<VariableAccessor<TSet> > ret_acc;
}
	: ret_acc = symset_variable_acc [scope.variables()]
	  COMMA 
	  ret_op  = symset_operator [scope] {
			op.reset(new VarSetter<TSet>(*ret_acc, ret_op));
		}
;

// ----------------------------------------------------------------------------
// empty() operator
// Returns boost::shared_ptr<Function<Bool> > 
//----------------------------------------------------------------------------
empty_operator
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
	: "empty" LPAREN
		(
			  (match_empty[scope]) => 
				(
					op = match_empty  [scope]
				)
			| (symset_empty [scope]) => 
				(
					op = symset_empty [scope]
				)
			| op = strset_empty [scope]
		)
	RPAREN
;

/*
empty_operator
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
	: "empty" LPAREN
	(
		  op = match_empty  [scope]
		| op = symset_empty [scope]
		| op = strset_empty [scope]
	)
	RPAREN
;
*/

//----------------------------------------------------------------------------
// match empty() operator
// Returns boost::shared_ptr<Function<Bool> > 
match_empty
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
{
	boost::shared_ptr<Function<Match> > arg;
}
	: arg = match_operator [scope] {
		op.reset(new IsEmpty<Match>(arg));
	}
;

//----------------------------------------------------------------------------
// SymSet empty() operator
// Returns boost::shared_ptr<Function<Bool> > 
symset_empty
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
{
	boost::shared_ptr<Function<TSet> > arg;
}
	: arg = symset_operator [scope] {
		op.reset(new IsEmpty<TSet>(arg));
	}
;

//----------------------------------------------------------------------------
// Strset empty() operator
// Returns boost::shared_ptr<Function<Bool> >
strset_empty
	[ParsingScope& scope]
	returns [boost::shared_ptr<Function<Bool> > op]
{
	boost::shared_ptr<Function<StrSet> > arg;
}
	: arg = strset_operator [scope] {
		op.reset(new IsEmpty<StrSet>(arg));
	}
;

///////////////////////////////////////////////////////////////////////////////
// Match functional operators,
// which return boost::shared_ptr<Function<Match> >
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// A wrapper for match variable and match value.
match_var_val [ParsingScope& scope]
	returns [boost::shared_ptr<Function<Match> > ret]
	: ret = match_vector_variable [scope.variables()]
	| ret = match_value_const
;

///////////////////////////////////////////////////////////////////////////////
// Match operators.
// Returns boost::shared_ptr<Function<Match> >
///////////////////////////////////////////////////////////////////////////////
match_operator
  [ParsingScope& scope]
	returns [boost::shared_ptr<Function<Match> > ret]
{
	//
}
	:
	( ret = match_var_val [scope]
	| {LA(1)==LITERAL_M || LA(1)==COLON}? ("M")? {
			ret.reset(new VarGetter<Match>(scope.variables().create_accessor<Match>("_M")));
			ret.reset(new Submatch(ret, 1));
		}
	| "MA" { 
			ret.reset(new VarGetter<Match>(scope.variables().create_accessor<Match>("_M")));
			ret.reset(new Submatch(ret, 2));
		}
	| LPAREN ret = match_operator [scope] RPAREN
	)
	( // if there's a colon after the match, we have a submatch reference
		COLON i: UNSIGNED_INT { ret.reset(new Submatch(ret, token_ref_to_int(i))); }
	)*
;

// ----------------------------------------------------------------------------
// Wrapper from Function<Match> to Operator<Match>
functional_operator_match
	[ParsingScope& scope]
	returns [boost::shared_ptr<Operator<Match> > op]
{
	boost::shared_ptr<Function<Match> > body;
}
	: body = match_operator [scope] {
		op.reset(new Operator<Match>(body, scope.variables()));
	}
;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// WCCL FILE PARSING RULES
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

imports_section [WcclFile& wccl_file]
	: (import [wccl_file])+
;

import [WcclFile& wccl_file]
	: "import" LPAREN file_path : STRING COMMA lexicon_name : STRING RPAREN {
		wccl_file.import_lexicon(
			LexiconParser::parse_lexicon(
				wccl_file.path(),
				token_ref_to_std_string(lexicon_name),
				token_ref_to_std_string(file_path)));
	}
;

wccl_file_section [WcclFile& wccl_file]
	: any_operator_section [wccl_file]
	| tag_rules_section [wccl_file]
	| match_rules_section [wccl_file]
;

tag_rules_section [WcclFile& wccl_file]
{
	boost::shared_ptr<TagRuleSequence> rule_seq;
}
	: rule_seq = parse_tag_rule_sequence [wccl_file.tagset()] {
		if (wccl_file.has_tag_rules()) {
			throw ParserException("Only one tag_rules section allowed in a WCCL file.");
		}
		wccl_file.set_tag_rules(rule_seq);
	}
;

match_rules_section [WcclFile& wccl_file]
{
	ParsingScope scope(wccl_file);
	boost::shared_ptr<Matching::MatchRule> match_rule;
	boost::shared_ptr<Matching::MatchRuleSequence> rule_seq = boost::make_shared<Matching::MatchRuleSequence>();
}
	: "match_rules" {
		if (wccl_file.has_match_rules()) {
			throw ParserException("Only one match_rules section allowed in a WCCL file.");
		}
	}
	LPAREN
		match_rule = match_rule_operator [scope] {
			rule_seq->push_back(*match_rule);
			scope.reset_variables();
		}
		(
			SEMI match_rule = match_rule_operator [scope] {
				rule_seq->push_back(*match_rule);
				scope.reset_variables();
			}
		)* 
	RPAREN { wccl_file.set_match_rules(rule_seq); }
;

any_operator_section
	[WcclFile& wccl_file]
{
	boost::shared_ptr<UntypedOpSequence> untyped_seq;
	boost::shared_ptr<OpSequence<Bool> > bool_seq;
	boost::shared_ptr<OpSequence<TSet> > symset_seq;
	boost::shared_ptr<OpSequence<StrSet> > strset_seq;
	boost::shared_ptr<OpSequence<Position> > pos_seq;
	boost::shared_ptr<OpSequence<Match> > m_seq;
}
	: untyped_seq = untyped_operator_sequence [wccl_file] {
			wccl_file.add_untyped_section(untyped_seq);
		}
	| bool_seq = bool_operator_sequence [wccl_file] {
			wccl_file.add_section(bool_seq);
		}
	| symset_seq = symset_operator_sequence [wccl_file] {
			wccl_file.add_section(symset_seq);
		}
	| strset_seq = strset_operator_sequence [wccl_file] {
			wccl_file.add_section(strset_seq);
		}
	| pos_seq = position_operator_sequence [wccl_file] {
			wccl_file.add_section(pos_seq);
		}
	| m_seq = match_operator_sequence [wccl_file] {
			wccl_file.add_section(m_seq);
		}
;

bool_operator_sequence
	[const WcclFile& wccl_file]
	returns [boost::shared_ptr<OpSequence<Bool> > seq]
{
	ParsingScope scope(wccl_file);
	boost::shared_ptr<Operator<Bool> > op;
}
	: BOOL_SECTION_PREFIX name: STRING {
			seq.reset(new OpSequence<Bool>(token_ref_to_std_string(name)));
		}
		LPAREN
			op = functional_operator_bool [scope] { seq->append(op); scope.reset_variables(); }
			(SEMI op = functional_operator_bool [scope] { seq->append(op); scope.reset_variables(); })*
		RPAREN
;

symset_operator_sequence
	[const WcclFile& wccl_file]
	returns [boost::shared_ptr<OpSequence<TSet> > seq]
{
	ParsingScope scope(wccl_file);
	boost::shared_ptr<Operator<TSet> > op;
}
	: TST_SECTION_PREFIX name: STRING {
			seq.reset(new OpSequence<TSet>(token_ref_to_std_string(name)));
		}
		LPAREN
			op = functional_operator_symset [scope] { seq->append(op); scope.reset_variables(); }
			(SEMI op = functional_operator_symset [scope] { seq->append(op); scope.reset_variables(); })*
		RPAREN
;

strset_operator_sequence
	[const WcclFile& wccl_file]
	returns [boost::shared_ptr<OpSequence<StrSet> > seq]
{
	ParsingScope scope(wccl_file);
	boost::shared_ptr<Operator<StrSet> > op;
}
	: STR_SECTION_PREFIX name: STRING {
			seq.reset(new OpSequence<StrSet>(token_ref_to_std_string(name)));
		}
		LPAREN
			op = functional_operator_strset [scope] { seq->append(op); scope.reset_variables(); }
			(SEMI op = functional_operator_strset [scope] { seq->append(op); scope.reset_variables(); })*
		RPAREN
;

position_operator_sequence
	[const WcclFile& wccl_file]
	returns [boost::shared_ptr<OpSequence<Position> > seq]
{
	ParsingScope scope(wccl_file);
	boost::shared_ptr<Operator<Position> > op;
}
	: POS_SECTION_PREFIX name: STRING {
			seq.reset(new OpSequence<Position>(token_ref_to_std_string(name)));
		}
		LPAREN
			op = functional_operator_position [scope] { seq->append(op); scope.reset_variables(); }
			(SEMI op = functional_operator_position [scope] { seq->append(op); scope.reset_variables(); })*
		RPAREN
;

untyped_operator_sequence
	[const WcclFile& wccl_file]
	returns [boost::shared_ptr<UntypedOpSequence> seq]
{
	ParsingScope scope(wccl_file);
	boost::shared_ptr<FunctionalOperator> op;
}
	: AT_MARK name: STRING { 
			seq.reset(new UntypedOpSequence(token_ref_to_std_string(name)));
		}
		LPAREN
			op = functional_operator_any [scope] { seq->append(op); scope.reset_variables(); }
			(SEMI op = functional_operator_any [scope] { seq->append(op); scope.reset_variables(); })*
		RPAREN
;

match_operator_sequence
	[const WcclFile& wccl_file]
	returns [boost::shared_ptr<OpSequence<Match> > seq]
{
	ParsingScope scope(wccl_file);
	boost::shared_ptr<Operator<Match> > op;
}
	: MATCH_SECTION_PREFIX name: STRING {
			seq.reset(new OpSequence<Match>(token_ref_to_std_string(name)));
		}
		LPAREN
			op = functional_operator_match [scope] { seq->append(op); scope.reset_variables(); }
			(SEMI op = functional_operator_match [scope] { seq->append(op); scope.reset_variables();  })*
		RPAREN
;

functional_operator_any 
	[ParsingScope& scope]
	returns [boost::shared_ptr<FunctionalOperator> op]
{
}
	: (position_operator [scope]) => op = functional_operator_position [scope]
	| (symset_operator [scope]) => op = functional_operator_symset [scope]
	| (strset_operator [scope]) => op = functional_operator_strset [scope]
	| (match_operator [scope]) => op = functional_operator_match [scope]
	| op = functional_operator_bool [scope]
;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Tagging actions and rules:
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Single action such as select, delete, relabel or unify
action
	[ParsingScope& scope]
	returns [boost::shared_ptr<TagAction> act]
	: act = action_select  [scope]
	| act = action_delete  [scope]
	| act = action_relabel [scope]
	//
	| act = action_unify [scope]
	//
	| act = action_mark [scope]
	| act = action_unmark [scope]
;

// Action sequence - the actions are separated with commas:
// 	select(...), select(...), delete(...)
action_sequence
	[ParsingScope& scope]
	returns [boost::shared_ptr<std::vector<boost::shared_ptr<TagAction> > > v_act]
{
	boost::shared_ptr<TagAction> act;
	v_act.reset(new std::vector<boost::shared_ptr<TagAction> >);
}
	: act = action[scope] {
		v_act->push_back(act);
	} 
	(
		COMMA act = action[scope] {
			v_act->push_back(act);
		}
	)*
;

// ----------------------------------------------------------------------------
// Single rule:
// 	rule(NAME, ACTIONS) or rule(NAME, COND, ACTIONS)
tag_rule
	[ParsingScope& scope]
	returns [boost::shared_ptr<TagRule> rle]
{
	boost::shared_ptr<Function<Bool> > condition;
	boost::shared_ptr<std::vector<boost::shared_ptr<TagAction> > > actions;
}
	: "rule" LPAREN name: STRING COMMA 
				(condition = bool_operator [scope] COMMA)?
				actions = action_sequence [scope] 
		RPAREN {
			if (condition) {
				rle.reset(
					new TagRule(token_ref_to_std_string(name), scope.variables(), actions, condition));
			}
			else {
				rle.reset(
					new TagRule(token_ref_to_std_string(name), scope.variables(), actions));
			}
		}
;

// Rule sequence
tag_rule_sequence
	[ParsingScope& scope]
	returns [boost::shared_ptr<TagRuleSequence> rule_seq]
{
	boost::shared_ptr<TagRule> rle;
	rule_seq.reset(new TagRuleSequence());
}
	: rle = tag_rule [scope] {
		rule_seq->push_back(*rle);
		scope.reset_variables();
	}
	(
		SEMI rle = tag_rule [scope] {
			rule_seq->push_back(*rle);
			scope.reset_variables();
		}
	)*
;

// ----------------------------------------------------------------------------
// This is wrapper for tag_rule_sequence in rules section in the wccl file
tag_rules
	[ParsingScope& scope]
	returns [boost::shared_ptr<TagRuleSequence> rule_seq]
	: "tag_rules" LPAREN rule_seq = tag_rule_sequence [scope] RPAREN {
		//
	}
;

// ----------------------------------------------------------------------------
// Select action: 
//	select(position, predicate) or select(predicate);
action_select 
	[ParsingScope& scope]
	returns [boost::shared_ptr<Select> action]
{
	boost::shared_ptr<Function<Position> > pos;
	boost::shared_ptr<Function<Bool> > condition;
}
	: "select" LPAREN
	(
		(position_operator [scope]) =>
		(
			pos = position_operator [scope] COMMA
			condition = bool_operator [scope] {
				// select(positon, condition); 
				action.reset(new Select(condition, pos));
			}
		)
	|
		(
			condition = bool_operator [scope] {
				// select(condition); 
				action.reset(new Select(condition));
			}
		)
	) 
	RPAREN
;

// ----------------------------------------------------------------------------
// Delete action
//	delete(position, predicate) or delete(predicate);
action_delete
	[ParsingScope& scope]
	returns [boost::shared_ptr<Delete> action]
{
	boost::shared_ptr<Function<Position> > pos;
	boost::shared_ptr<Function<Bool> > condition;
}
	: "delete" LPAREN
	(
		(position_operator [scope]) =>
		(
			pos = position_operator [scope] COMMA
			condition = bool_operator [scope] {
				// delete(positon, condition); 
				action.reset(new Delete(condition, pos));
			}
		)
	|
		(
			condition = bool_operator [scope] {
				// delete(condition); 
				action.reset(new Delete(condition));
			}
		)
	) 
	RPAREN
;

// ----------------------------------------------------------------------------
// Relabel action
// 	relabel(pos, symset, predicate) or relabel(symset, predicate)
action_relabel
	[ParsingScope& scope]
	returns [boost::shared_ptr<Relabel> action]
{
	boost::shared_ptr<Function<Position> > pos;
	boost::shared_ptr<Function<Bool> > condition;
	boost::shared_ptr<Function<TSet> > replace_with;
}
	: "relabel" LPAREN
	(
		(position_operator [scope]) =>
		(
			pos = position_operator [scope] COMMA
			replace_with = symset_operator [scope] COMMA
			condition = bool_operator [scope] {
				// relabel(pos, symset, predicate)
				action.reset(new Relabel(replace_with, condition, pos));
			}
		)
	|
		(
			replace_with = symset_operator [scope] COMMA
			condition = bool_operator [scope] {
				// relabel(symset, predicate)
				action.reset(new Relabel(replace_with, condition));
			}
		)
	)
	RPAREN
;

// ----------------------------------------------------------------------------
// Unify action 
action_unify
	[ParsingScope& scope]
	returns [boost::shared_ptr<Unify> action]
{
	boost::shared_ptr<Function<TSet> > attribs_expr;
	boost::shared_ptr<Function<Position> > pos_begin, pos_end;
}
	: "unify" LPAREN
			pos_begin    = position_operator [scope] COMMA
			pos_end      = position_operator [scope] COMMA
			attribs_expr = symset_operator   [scope]
		RPAREN  {
			action.reset(new Unify(pos_begin, pos_end, attribs_expr));
		}
;

// ----------------------------------------------------------------------------
// Mark action
action_mark
	[ParsingScope& scope]
	returns [boost::shared_ptr<Mark> action]
{
	boost::shared_ptr<Function<Position> > pos_begin, pos_end, pos_head;
}
	: "mark" LPAREN
			pos_begin    = position_operator [scope] COMMA
			pos_end      = position_operator [scope] COMMA
			(pos_head    = position_operator [scope] COMMA)?
			chan_name: STRING
		RPAREN  {
			action.reset(new Mark(pos_begin, pos_end, pos_head, ((antlr::Token*)chan_name)->getText()));
		}
;

// ----------------------------------------------------------------------------
// Unmark action
action_unmark
	[ParsingScope& scope]
	returns [boost::shared_ptr<Unmark> action]
{
	boost::shared_ptr<Function<Position> > pos;
}
	: "unmark" LPAREN
			pos = position_operator [scope] COMMA
			chan_name: STRING
		RPAREN  {
			action.reset(new Unmark(pos, ((antlr::Token*)chan_name)->getText()));
		}
;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Match rules
// Returns boost::shared_ptr<Matching::MatchRule>
match_rule_operator
	[ParsingScope& scope]
	returns [boost::shared_ptr<Matching::MatchRule> ret_op]
{
	boost::shared_ptr<Matching::ApplyOperator> apply;
	scope.variables().get_put<Match>("_M");
}
	: apply = match_apply_operator [scope] {
		ret_op = boost::make_shared<Matching::MatchRule>(scope.variables(), apply);
	}
;

// Match apply operator:
// 	apply(match(), cond(conditions), actions(actions)) 
// 	apply(match(), actions(actions))
// Returns boost::shared_ptr<Matching::ApplyOperator>
match_apply_operator
	[ParsingScope& scope]
	returns [boost::shared_ptr<Matching::ApplyOperator> ret_op]
{
	VariableAccessor<Match> matches = scope.variables().create_accessor<Match>("_M");;
	boost::shared_ptr<Matching::ConjConditions> match_cond;
	boost::shared_ptr<std::vector<boost::shared_ptr<Matching::MatchAction> > > actions;
	boost::shared_ptr<std::vector<boost::shared_ptr<Function<Bool> > > > conditions;
}
	: "apply" LPAREN 
		"match" LPAREN match_cond = match_condition [scope] RPAREN COMMA
		("cond"   LPAREN conditions = bool_operator_comma_sep [scope] RPAREN COMMA)?
		"actions" LPAREN actions    =  match_action_comma_sep [scope] RPAREN
		RPAREN {
			if (conditions) {
				ret_op.reset(
					new Matching::ApplyOperator(matches, match_cond, actions, conditions)
				);
			}
			else {
				ret_op.reset(
					new Matching::ApplyOperator(matches, match_cond, actions)
				);
			}
		}
;

// Match conditions. Wrapper for vector of the match conditions
match_condition
	[ParsingScope& scope]
	returns [boost::shared_ptr<Matching::ConjConditions> condition]
{
	std::vector<boost::shared_ptr<const Matching::MatchCondition> > m_cond;
}
	: m_cond = match_condition_in [scope] {
		condition.reset(new Matching::ConjConditions(m_cond));
	}
;

// Match conditions.
// Retutns std::vector< boost::shared_ptr<const Matching::MatchCondition> >
match_condition_in
	[ParsingScope& scope]
	returns [std::vector< boost::shared_ptr<const Matching::MatchCondition> > ret]
{
	boost::shared_ptr<const Matching::MatchCondition> r_cond;
}
	: r_cond = match_cond_all[scope] {
		ret.push_back(r_cond);
	}
	(
		COMMA
		r_cond = match_cond_all[scope] {
			ret.push_back(r_cond);
		}
	)*
;

// Match variants variant(v1), variant(v2), ...
// Retutns boost::shared_ptr<std::vector<Matching::ConjConditions> >
match_variants
	[ParsingScope& scope]
	returns [boost::shared_ptr<std::vector<boost::shared_ptr<Matching::ConjConditions> > > variants]
{
	variants.reset(new std::vector<boost::shared_ptr<Matching::ConjConditions> >());

	boost::shared_ptr<Matching::ConjConditions> variant;
}
	: "variant" LPAREN variant = match_condition [scope] RPAREN {
		variants->push_back(variant);
	} 
	(
		COMMA "variant" LPAREN variant = match_condition [scope] RPAREN {
			variants->push_back(variant);
		}
	)*
;

// One of the match condition
// Returns boost::shared_ptr<const Matching::MatchCondition>
match_cond_all
	[ParsingScope& scope]
	returns [boost::shared_ptr<const Matching::MatchCondition> ret]
	: ret = match_cond_optional [scope]
	| ret = match_cond_repeate  [scope]
	| ret = match_cond_token    [scope]
	| ret = match_cond_oneof    [scope]
	| ret = match_cond_longest  [scope]
	| ret = match_cond_is
	| ret = match_cond_text
;

// Match condition - token (wraps a L0 predicate)
// Returns boost::shared_ptr<const Matching::MatchCondition>
match_cond_token
	[ParsingScope& scope]
	returns [boost::shared_ptr<const Matching::TokenCondition> ret]
{
	boost::shared_ptr<Function<Bool> > bool_op;
}
	: bool_op = bool_operator [scope] {
		ret = boost::make_shared<Matching::TokenCondition>(bool_op);
	}
;


 // Match condition - optional
// Returns boost::shared_ptr<Matching::OptionalMatch>
match_cond_optional
	[ParsingScope& scope]
	returns [boost::shared_ptr<Matching::OptionalMatch> mtch]
{
	boost::shared_ptr<Matching::ConjConditions> m_cond;
}
	: "optional" LPAREN m_cond = match_condition [scope] RPAREN {
		mtch.reset(new Matching::OptionalMatch(m_cond));
	}
;

// Match condition - repeat
// Returns boost::shared_ptr<Matching::RepeatedMatch>
match_cond_repeate
	[ParsingScope& scope]
	returns [boost::shared_ptr<Matching::RepeatedMatch> mtch]
{
	boost::shared_ptr<Matching::ConjConditions> m_cond;
}
	: "repeat" LPAREN m_cond = match_condition [scope] RPAREN {
		mtch.reset(new Matching::RepeatedMatch(m_cond));
	}
;

// Match condition - is(ann_name)
// Returns boost::shared_ptr<Matching::IsAnnotatedAs>
match_cond_is
	returns [boost::shared_ptr<Matching::IsAnnotatedAs> mtch]
	: "is" LPAREN annotation_name: STRING RPAREN {
		mtch.reset(new Matching::IsAnnotatedAs(token_ref_to_std_string(annotation_name)));
	}
;

// Match condition - text(text)
// Returns boost::shared_ptr<Matching::MatchText>
match_cond_text
	returns [boost::shared_ptr<Matching::MatchText> mtch]
	: "text" LPAREN txt: STRING RPAREN {
		mtch.reset(new Matching::MatchText(token_ref_to_ustring(txt)));
	}
;

// Match condition - oneof(variant1(v1), variant(v2), ...)
// Returns boost::shared_ptr<Matching::OneOf>
match_cond_oneof
	[ParsingScope& scope]
	returns [boost::shared_ptr<Matching::OneOf> onf]
{
	boost::shared_ptr<std::vector<boost::shared_ptr<Matching::ConjConditions> > > variants;
}
	: "oneof" LPAREN variants = match_variants [scope] RPAREN {
		onf.reset(new Matching::OneOf(variants));
	}
;

// Match condition - longest(variant1(v1), variant(v2), ...)
// Returns boost::shared_ptr<Matching::Longest>
match_cond_longest
	[ParsingScope& scope]
	returns [boost::shared_ptr<Matching::Longest> lng]
{
	boost::shared_ptr<std::vector<boost::shared_ptr<Matching::ConjConditions> > > variants;
}
	: "longest" LPAREN variants = match_variants [scope] RPAREN {
		lng.reset(new Matching::Longest(variants));
	}
;

// ----------------------------------------------------------------------------

// Match actions. Match action can be mark or unmark
// Returns boost::shared_ptr<Matching::MatchAction>
match_action
	[ParsingScope& scope]
	returns [boost::shared_ptr<Matching::MatchAction> m_act]
	: m_act = match_mark_action   [scope]
	| m_act = match_unmark_action [scope]
	| m_act = match_remark_action [scope]
	| m_act = match_setprop_action [scope]
;

// Match mark action
// Returns 
match_mark_action
	[ParsingScope& scope]
	returns [boost::shared_ptr<Matching::MarkMatch> m_act]
{
	boost::shared_ptr<Function<Match> > match_to;
	boost::shared_ptr<Function<Match> > match_from;
	boost::shared_ptr<Function<Match> > head_match;
}
	: "mark" LPAREN 
			match_from = match_operator[scope] COMMA
			( match_to  = match_operator[scope] COMMA
				( head_match = match_operator[scope] COMMA )?
			)?
			annotation_name : STRING
		RPAREN {
			if (!match_to) {
				m_act.reset(
					new Matching::MarkMatch(
						match_from,
						((antlr::Token*)annotation_name)->getText()));
			} else {
				if (!head_match) {
					m_act.reset(
						new Matching::MarkMatch(
							match_from,
							match_to,
							((antlr::Token*)annotation_name)->getText()));
				} else {
					m_act.reset(
						new Matching::MarkMatch(
							match_from,
							match_to,
							head_match,
							((antlr::Token*)annotation_name)->getText()));
				}
			}
		}
;

// Match unmark action
// Returns boost::shared_ptr<Matching::UnmarkMatch>
match_unmark_action
	[ParsingScope& scope]
	returns [boost::shared_ptr<Matching::UnmarkMatch> m_act]
{
	boost::shared_ptr<Function<Match> > match_at;
}
	: "unmark" LPAREN
				match_at = match_operator[scope] COMMA
				annotation_name : STRING
			RPAREN {
				m_act.reset(
					new Matching::UnmarkMatch(
							match_at,
							((antlr::Token*)annotation_name)->getText()));
			}
;

// Overwrite-match mark action ("remark")
// Returns 
match_remark_action
	[ParsingScope& scope]
	returns [boost::shared_ptr<Matching::OverwriteMatch> m_act]
{
	boost::shared_ptr<Function<Match> > match_to;
	boost::shared_ptr<Function<Match> > match_from;
	boost::shared_ptr<Function<Match> > head_match;
}
	: "remark" LPAREN 
			match_from = match_operator[scope] COMMA
			( match_to  = match_operator[scope] COMMA
				( head_match = match_operator[scope] COMMA )?
			)?
			annotation_name : STRING
		RPAREN {
			if (!match_to) {
				m_act.reset(
					new Matching::OverwriteMatch(
						match_from,
						((antlr::Token*)annotation_name)->getText()));
			} else {
				if (!head_match) {
					m_act.reset(
						new Matching::OverwriteMatch(
							match_from,
							match_to,
							((antlr::Token*)annotation_name)->getText()));
				} else {
					m_act.reset(
						new Matching::OverwriteMatch(
							match_from,
							match_to,
							head_match,
							((antlr::Token*)annotation_name)->getText()));
				}
			}
		}
;

// Match prop action
// Returns boost::shared_ptr<Matching::PropMatch>
match_setprop_action
	[ParsingScope& scope]
	returns [boost::shared_ptr<Matching::PropMatch> m_act]
{
	boost::shared_ptr<Function<Match> > match_at;
}
	: "setprop" LPAREN
				match_at = match_operator[scope] COMMA
				key_name : STRING COMMA
				value_name : STRING
			RPAREN {
				m_act.reset(
					new Matching::PropMatch(
							match_at,
							((antlr::Token*)key_name)->getText(),
							((antlr::Token*)value_name)->getText()));
			}
;

// Match action separated by comma
// Returns boost::shared_ptr<std::vector<boost::shared_ptr<Matching::MatchAction> > >
match_action_comma_sep
	[ParsingScope& scope]
	returns [boost::shared_ptr<std::vector<boost::shared_ptr<Matching::MatchAction> > > r_vec]
{
	boost::shared_ptr<Matching::MatchAction> act;

	r_vec.reset(
		new std::vector<boost::shared_ptr<Matching::MatchAction> >
	);
}
	: act = match_action [scope] {
			r_vec->push_back(act);
	}
	(
		COMMA act = match_action [scope] {
			r_vec->push_back(act);
		}
	)*
;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// ANTLR LEXER
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ANTLRLexer extends Lexer;
options {
	exportVocab    = ANTLRExpr;
	charVocabulary = '\3'..'\377';
	testLiterals   = false;
	k              = 2;
}


// TODO
STRING
options {
	paraphrase = "a string";
}
	: '"'!  (~('"'  | '\n' | '\r'))* '"'!
	| '\''! (~('\'' | '\n' | '\r'))* '\''!
;

SIGNED_INT
options {
	paraphrase = "Signed integer";
}
	: ('-'|'+') (' '!|'\t'!)* ('0'..'9')+ 
;	

UNSIGNED_INT
options {
	paraphrase = "Unsigned integer";
}
	: ('0'..'9')+ 
;	


QUOT_MARK
options {
	paraphrase = "Quote";
} 
	: '\'' 
;

APOS_MARK
options {
	paraphrase = "Apostrophe";
}
	: '"' 
;

Q_MARK
options {
	paraphrase = "Question mark";
}
	: '?'
;

E_MARK
options {
	paraphrase = "Exclamation mark";
}
	: '!'
;

STR_PREFIX
options {
	paraphrase = "String prefix";
}
	: "$s:"
;

TST_PREFIX
options {
	paraphrase = "Symset prefix";
}
	: "$t:"
;

BOOL_PREFIX
options {
	paraphrase = "Bool prefix";
}
	: "$b:"
;

POS_PREFIX
options {
	paraphrase = "Position prefix";
}
	: '$'
;

MATCH_VECTOR_PREFIX
options {
	paraphrase = "Match vector prefix";
}
	: "$m:"
;

STR_SECTION_PREFIX
options {
	paraphrase = "Wccl file string operators section prefix";
}
	: "@s:"
;

POS_SECTION_PREFIX
options {
	paraphrase = "Wccl file position operators section prefix";
}
	: "@p:"
;

BOOL_SECTION_PREFIX
options {
	paraphrase = "Wccl file bool operators section prefix";
}
	: "@b:"
;

TST_SECTION_PREFIX
options {
	paraphrase = "Wccl file symset operators section prefix";
}
	: "@t:"
;

MATCH_SECTION_PREFIX
options {
	paraphrase = "Wccl file symset operators section prefix";
}
	: "@m:"
;

LBRACKET 
options {
	paraphrase = "'['";
}
	: '[' 
;

RBRACKET 
options {
	paraphrase = "']'";
}
	: ']' 
;

LPAREN
options {
	paraphrase = "'('";
}   
	: '(' 
;

RPAREN 
options {
	paraphrase = "')'";
} 
	: ')' 
;

LCURLY 
options {
	paraphrase = "'{'";
} 
	: '{' 
;

RCURLY 
options {
	paraphrase = "'}'";
} 
	: '}' 
;

AT_MARK 
options {
	paraphrase = "'@'";
} 
	: '@' 
;


COMMA
options { 
	paraphrase = "','";
}
	: ','
;

ARROW
options {
	paraphrase = "->";
}
	: "->"
;

COLON
options {
	paraphrase = ":";
}
	: ':'
;

SEMI
options {
	paraphrase = ";";
}
	: ';'
;

SYMBOL
options { 
	paraphrase = "Symbol"; 
	testLiterals = true; 
}
	: ('a'..'z' | 'A'..'Z' | '_') ('a'..'z' | 'A'..'Z' | '_' | '0'..'9')*
	| '`' ('a'..'z' | 'A'..'Z' | '_') ('a'..'z' | 'A'..'Z' | '_' | '0'..'9')* '`'
;

WS
	: ( ' '
	  	| '\t'
	  	| '\f'
	  	| 
			( "\r\n"
				| '\r'
				| '\n'
			) { newline(); } 
		) { $setType(antlr::Token::SKIP); } 
;

COMMENT
options {
	paraphrase = "Single line comment";
}
	: "//" (~('\n'|'\r'))* { $setType(antlr::Token::SKIP);  }
;

ML_COMMENT
options {
	paraphrase = "Multi line comment";
}
  : "/*"
	(			// TODO: test it and add reference to the site it's taken from!
				/* This actually works OK despite the ambiguity that
				'\r' '\n' can be matched in one alternative or by matching
				'\r' in one iteration and '\n' in another.. But 
				this is really matched just by one rule per (...)* 
				loop iteration, so it's OK.
				This is exactly how they do it all over the web - just
				turn off the warning for this particular token.*/
		options { 
			generateAmbigWarnings = false; 
		}
      : { LA(2)!='/' }? '*'
      | '\r' '\n' { newline(); }
      | '\r' { newline(); }
      | '\n' { newline(); }
      | ~('*'|'\n'|'\r')
  	)*
    "*/"
    { $setType(antlr::Token::SKIP); }
;

HASH
options { 
	paraphrase = "'#'"; 
}
	: '#' 
;

//DSEPARATOR
//options { 
//	paraphrase = "':-'"; 
//}
//	: ":-" 
//;
