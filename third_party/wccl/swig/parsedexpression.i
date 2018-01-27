#ifndef SWIG_LIBWCCL_PARSEDEXPRESSION_I
#define SWIG_LIBWCCL_PARSEDEXPRESSION_I

%module libcclparsedexpression
%{
  #include <libwccl/ops/parsedexpression.h>
%}

%include "value.i"

%include "match.i"
%include "bool.i"
%include "tset.i"
%include "strset.i"
%include "position.i"
%include "expression.i"

%include "std_string.i"
%include <libcorpus2/boost_shared_ptr.i>

%template (ParsedExpressionPtr) boost::shared_ptr<Wccl::ParsedExpression>;

namespace Wccl {
  class ParsedExpression : public Expression {
  public:
    %rename(Getvalue) operator[](const std::string& var_name) const;
    const Value& operator[](const std::string& var_name) const;

    /* --------------------------------------------------------------------- */    
    template<class T> T& get(const std::string& var_name);
    
    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }

    template<class T> bool has(const std::string& var_name);
    
    /* Those functions are overridden later to handle shared_ptr values,
     * anyway they are needed for those overrides to work. */
    template<class T> void set(const std::string& var_name, const T& value);

    /* --------------------------------------------------------------------- */    
    void clean();

    /* --------------------------------------------------------------------- */    
    boost::shared_ptr<ParsedExpression> clone_ptr() const;
    boost::shared_ptr<ParsedExpression> clone_clean_ptr() const;

    /* --------------------------------------------------------------------- */    
    std::string variables_string(const Corpus2::Tagset& tagset) const;
    std::ostream& dump_variables(std::ostream& ostream, const Corpus2::Tagset& tagset) const;

  protected:
    explicit ParsedExpression(const Variables& variables);
    virtual ParsedExpression* clone_internal() const = 0;
  };

  %extend ParsedExpression {
    %template(get_bool) get<Bool>;
    %template(get_tset) get<TSet>;
    %template(get_strset) get<StrSet>;
    %template(get_position) get<Position>;
    %template(get_match) get<Match>;

    %template(has_bool) has<Bool>;
    %template(has_tset) has<TSet>;
    %template(has_strset) has<StrSet>;
    %template(has_position) has<Position>;
    %template(has_match) has<Match>;

    %template(set_bool) set<Bool>;
    %template(set_tset) set<TSet>;
    %template(set_strset) set<StrSet>;
    %template(set_position) set<Position>;
    %template(set_match) set<Match>;

    void set_bool(const std::string& name, const boost::shared_ptr<const Wccl::Bool>& b) {
        self->set<Wccl::Bool>(name, *b);
    }
    void set_tset(const std::string& name, const boost::shared_ptr<const Wccl::TSet>& b) {
        self->set<Wccl::TSet>(name, *b);
    }
    void set_strset(const std::string& name, const boost::shared_ptr<const Wccl::StrSet>& b) {
        self->set<Wccl::StrSet>(name, *b);
    }
    void set_position(const std::string& name, const boost::shared_ptr<const Wccl::Position>& b) {
        self->set<Wccl::Position>(name, *b);
    }
    void set_match(const std::string& name, const boost::shared_ptr<const Wccl::Match>& b) {
        self->set<Wccl::Match>(name, *b);
    }
  }
} // namespace Wccl

using namespace std;
using namespace Wccl;

#endif /* SWIG_LIBWCCL_PARSEDEXPRESSION_I */
