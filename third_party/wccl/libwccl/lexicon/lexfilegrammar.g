header {
	//don't try to add all the headers inside our namespace
	ANTLR_END_NAMESPACE

	#include <antlr/Token.hpp>
	
	#include <boost/shared_ptr.hpp>
	
	#include <libwccl/lexicon/lexicon.h>
	
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
// ANTLR PARSER FOR LEXICON FILES
// ----------------------------------------------------------------------------
class ANTLRLexiconParser extends Parser;
options {
	k = 2;
	buildAST = false;
	exportVocab = ANTLRLexicons;
	defaultErrorHandler = false;
}

{
private:
	// 
	const UnicodeString to_ustring(antlr::RefToken& rstr) const { 
		return UnicodeString::fromUTF8(((antlr::Token*)rstr)->getText().c_str()).unescape();
	}
	//
	const std::string to_std_string(antlr::RefToken& rstr) const { 
		return (((antlr::Token*)rstr)->getText());
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// PARSER RULES
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

parse_lexicon_file
	[const std::string& name, const std::string& file_name]
	returns [boost::shared_ptr<Lexicon> lex]
{
	lex.reset(new Lexicon(name, file_name));
}
	: (lexicon_entry [*lex])*
	  EOF
;

lexicon_entry
	[Lexicon& lex]
{
	UnicodeString key, value;
}
	: key = ustring (TAB value = ustring)? {
		if (value.isEmpty()) {
			lex.insert(key);
		} else {
			lex.insert(key, value);
		}
	}
;

ustring
	returns [UnicodeString us]
	: s : STRING { us = to_ustring(s); }
;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// ANTLR LEXER FOR LEXICON FILES
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ANTLRLexiconLexer extends Lexer;
options {
	exportVocab    = ANTLRLexicons;
	charVocabulary = '\3'..'\377';
	testLiterals   = false;
	k              = 2;
}

STRING
options {
	paraphrase = "a string";
}
	: (~( '\t' | '\n' | '\r'))+
;

TAB
options {
	paraphrase = "a tabulator";
}
	: '\t'
;

NEWLINE
	: ( "\r\n"
		| '\r'
		| '\n'
		) { newline(); $setType(antlr::Token::SKIP); }
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
  (     // TODO: test it and add reference to the site it's taken from!
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
