#ifndef SWIG_LIBCCL_PARSER_I
#define SWIG_LIBCCL_PARSER_I

%module libcclparser
%{
  #include <libwccl/parser/Parser.h>
%}

%include "tagrule.i"
%include "tagrulesequence.i"
%include "matchrule.i"
%include "wcclfile.i"
%include "functionaloperator.i"

%include "libcorpus2/tagset.i"

%include "std_string.i"
%include "std_vector.i"
%include <libcorpus2/boost_shared_ptr.i>

namespace Wccl {
  class Parser {
  public:
    explicit Parser(const Corpus2::Tagset&);
    explicit Parser(const std::string& tagset_name);
    ~Parser();

    /* --------------------------------------------------------------------- */

    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    shared_ptr<Operator<StrSet> > 
      parseStringOperator (const std::string& operator_string) const;

    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    shared_ptr<Operator<Bool> > 
      parseBoolOperator (const std::string& operator_string) const;

    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    shared_ptr<Operator<TSet> > 
      parseSymSetOperator (const std::string& operator_string) const;

    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    shared_ptr<Operator<Position> > 
      parsePositionOperator(const std::string& operator_string) const;

    /* --------------------------------------------------------------------- */

    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    shared_ptr<FunctionalOperator>  
      parseAnyOperator(const std::string& operator_string) const;

    /* --------------------------------------------------------------------- */

    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    shared_ptr<TagRuleSequence>
      parseTagRuleSequence(const std::string& rule_string) const;

    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    shared_ptr<TagRule>
      parseSingleRule(const std::string& rule_string) const;
    
    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    shared_ptr<MatchRule>
      parseMatchRule(const std::string& rule_string) const;

    /* --------------------------------------------------------------------- */

    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    shared_ptr<WcclFile> parseWcclFile(
      const std::string& file_contents_string, 
      const std::string& search_path = ".") const;

    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    shared_ptr<WcclFile> parseWcclFileFromPath(
      const std::string& filename, 
      const std::string& search_path = ".") const;
    
/* --------------------------------------------------------------------- */

    const boost::shared_ptr<const Corpus2::Tagset> tagset() const;
  };
}

using namespace boost;
using namespace std;
using namespace Corpus2;

#endif /* SWIG_LIBCCL_PARSER_I */
