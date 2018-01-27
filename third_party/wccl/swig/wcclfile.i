#ifndef SWIG_LIBWCCL_WCCLFILE_I
#define SWIG_LIBWCCL_WCCLFILE_I

%module libcclwcclfile
%{
  #include <libwccl/wcclfile.h>
%}

%include "bool.i"
%include "tset.i"
%include "match.i"
%include "strset.i"
%include "position.i"
%include "lexicon.i"
%include "lexicons.i"
%include "tagrulesequence.i"
%include "matchrulesequence.i"
%include "functionaloperator.i"
%include "wcclfileopsections.i"
%include "functionalopsequence.i"

%include "libcorpus2/tagset.i"

%include "std_string.i"
%include "std_vector.i"
%include <libcorpus2/boost_shared_ptr.i>

%nodefaultctor Wccl::WcclFile;

%template(WcclFilePtr) boost::shared_ptr<Wccl::WcclFile>;

namespace Wccl {
  class WcclFile 
    : public WcclFileOpSections<UntypedOpSequence> /*,
    WcclFileOpSections<OpSequence<StrSet> >,
    WcclFileOpSections<OpSequence<TSet> >,
    WcclFileOpSections<OpSequence<Bool> >,
    WcclFileOpSections<OpSequence<Position> >,
    WcclFileOpSections<OpSequence<Match> > 
    */ {
  public:
    WcclFile(const Corpus2::Tagset& tagset, const std::string& search_path);

    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */

    /*
    const std::vector<shared_ptr<UntypedOpSequence> >& untyped_sections();
    */

    /*
    template<class T> const typename std::vector<shared_ptr<OpSequence<T> > >& sections();
    %template(sections_bool) sections<Bool>;
    %template(sections_tset) sections<TSet>;
    %template(sections_strset) sections<StrSet>;
    %template(sections_position) sections<Position>;
    %template(sections_match) sections<Match>;
    */

    bool has_untyped_section(const std::string& name) const;

    template<class T> bool has_section(const std::string& name) const;
    
    /* --------------------------------------------------------------------- */
  
    std::vector<std::string> untyped_section_names() const;

    template<class T> std::vector<std::string> section_names() const;

    /* --------------------------------------------------------------------- */

    // const UntypedOpSequence& get_untyped_section(const std::string& name) const;
    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    UntypedOpSequence& get_untyped_section(const std::string& name);

    /* --------------------------------------------------------------------- */

    // template<class T> const OpSequence<T>& get_section(const std::string& name) const;
    template<class T> OpSequence<T>& get_section(const std::string& name);
    
    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    shared_ptr<UntypedOpSequence> get_untyped_section_ptr(const std::string& name);


    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    shared_ptr<const UntypedOpSequence> get_untyped_section_ptr(const std::string& name) const;
    
    /* --------------------------------------------------------------------- */

    // template<class T> shared_ptr<const OpSequence<T> > get_section_ptr(const std::string& name) const;
    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    template<class T> shared_ptr<OpSequence<T> > get_section_ptr(const std::string& name);
 
    /* --------------------------------------------------------------------- */

    // const FunctionalOperator& get_untyped_op(const std::string& name, size_t idx = 0) const;
    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    FunctionalOperator& get_untyped_op(const std::string& name, size_t idx = 0);
    
    /* --------------------------------------------------------------------- */

    // template<class T> const Operator<T>& get_op(const std::string& name, size_t idx = 0) const;
    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    template<class T> Operator<T>& get_op(const std::string& name, size_t idx = 0);
    /*
    %template(get_op_bool) get_op<Bool>;
    %template(get_op_tset) get_op<TSet>;
    %template(get_op_strset) get_op<StrSet>;
    %template(get_op_position) get_op<Position>;
    %template(get_op_match) get_op<Match>;
    */
    
    /* --------------------------------------------------------------------- */

    // shared_ptr<const FunctionalOperator> get_untyped_op_ptr(const std::string& name, size_t idx = 0) const;
    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    shared_ptr<FunctionalOperator> get_untyped_op_ptr(const std::string& name, size_t idx = 0);

    /* --------------------------------------------------------------------- */

    // template<class T> shared_ptr<const Operator<T> > get_op_ptr(const std::string& name, size_t idx = 0) const;
    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    template<class T> shared_ptr<Operator<T> > get_op_ptr(const std::string& name, size_t idx = 0);

    /* --------------------------------------------------------------------- */

    // UntypedOpSequence::name_op_v_c_t gen_name_untyped_op_pairs() const;
    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    UntypedOpSequence::name_op_v_t gen_name_untyped_op_pairs();

    /* --------------------------------------------------------------------- */

    // template<class T> typename OpSequence<T>::name_op_v_c_t gen_name_op_pairs() const;
    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    template<class T> typename OpSequence<T>::name_op_v_t gen_name_op_pairs();

    /* --------------------------------------------------------------------- */

    // FunctionalOpSequence::name_op_v_c_t gen_all_op_pairs() const;
    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    FunctionalOpSequence::name_op_v_t gen_all_op_pairs();


    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */

    bool has_lexicon(const std::string& name) const;
    bool has_lexicons() const;

    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    shared_ptr<const Lexicon> get_lexicon_ptr(const std::string& name) const;

    %exception {
      try {
        $action
      } catch (PwrNlp::PwrNlpError &e) {
        PyErr_SetString(PyExc_IndexError, e.info().c_str());
        return NULL;
      }
    }
    const Lexicon& get_lexicon(const std::string& name) const;

    shared_ptr<const Lexicons> get_lexicons_ptr() const;
    const Lexicons& get_lexicons() const;

    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
  
    bool has_tag_rules() const;

    const TagRuleSequence& get_tag_rules() const;

    // shared_ptr<const TagRuleSequence> get_tag_rules_ptr() const;
    shared_ptr<TagRuleSequence> get_tag_rules_ptr();

    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */

    bool has_match_rules() const;
    
    const Matching::MatchRuleSequence& get_match_rules() const;

    // shared_ptr<const MatchRuleSequence> get_match_rules_ptr() const;
    shared_ptr<Matching::MatchRuleSequence> get_match_rules_ptr();

    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */

    const boost::shared_ptr<const Corpus2::Tagset> tagset() const;

    /*
    // const PwrNlp::PathSearcher<Wccl::FileNotFound> path() const { return path_; }
    PwrNlp::PathSearcher<Wccl::FileNotFound> path(); 
    */

    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */
    /* --------------------------------------------------------------------- */

    /*
    // void add_untyped_section(const boost::shared_ptr<const UntypedOpSequence>& section);
    void add_untyped_section(const shared_ptr<UntypedOpSequence>& section);
    */

    void add_untyped_section(const UntypedOpSequence& section);

    /*
    // template<class T> void add_section(const shared_ptr<const OpSequence<T> >& section);
    template<class T> void add_section(const shared_ptr<OpSequence<T> >& section);
    %template(add_section_ptr_bool) add_section<Bool>;
    %template(add_section_ptr_tset) add_section<TSet>;
    %template(add_section_ptr_strset) add_section<StrSet>;
    %template(add_section_ptr_position) add_section<Position>;
    %template(add_section_ptr_match) add_section<Match>;
    */
    
    template<class T> void add_section(const OpSequence<T>& section);

    void import_lexicon(const shared_ptr<Lexicon>& lexicon);

    /*
    void set_tag_rules(const shared_ptr<TagRuleSequence>& tag_rules);
    */

    /*
    void set_match_rules(const shared_ptr<MatchRuleSequence>& match_rules);
    */
  };

  %extend WcclFile {  
    %template(has_section_bool) has_section<Bool>;
    %template(has_section_tset) has_section<TSet>;
    %template(has_section_strset) has_section<StrSet>;
    %template(has_section_position) has_section<Position>;
    %template(has_section_match) has_section<Match>;
    
    %template(section_names_bool) section_names<Bool>;
    %template(section_names_tset) section_names<TSet>;
    %template(section_names_strset) section_names<StrSet>;
    %template(section_names_position) section_names<Position>;
    %template(section_names_match) section_names<Match>;

    %template(get_section_bool) get_section<Bool>;
    %template(get_section_tset) get_section<TSet>;
    %template(get_section_strset) get_section<StrSet>;
    %template(get_section_position) get_section<Position>;
    %template(get_section_match) get_section<Match>;

    %template(get_section_ptr_bool) get_section_ptr<Bool>;
    %template(get_section_ptr_tset) get_section_ptr<TSet>;
    %template(get_section_ptr_strset) get_section_ptr<StrSet>;
    %template(get_section_ptr_position) get_section_ptr<Position>;
    %template(get_section_ptr_match) get_section_ptr<Match>;

    %template(get_op_ptr_bool) get_op_ptr<Bool>;
    %template(get_op_ptr_tset) get_op_ptr<TSet>;
    %template(get_op_ptr_strset) get_op_ptr<StrSet>;
    %template(get_op_ptr_position) get_op_ptr<Position>;
    %template(get_op_ptr_match) get_op_ptr<Match>;

    %template(gen_name_op_pairs_bool) gen_name_op_pairs<Bool>;
    %template(gen_name_op_pairs_tset) gen_name_op_pairs<TSet>;
    %template(gen_name_op_pairs_strset) gen_name_op_pairs<StrSet>;
    %template(gen_name_op_pairs_position) gen_name_op_pairs<Position>;
    %template(gen_name_op_pairs_match) gen_name_op_pairs<Match>;

    %template(add_section_bool) add_section<Bool>;
    %template(add_section_tset) add_section<TSet>;
    %template(add_section_strset) add_section<StrSet>;
    %template(add_section_position) add_section<Position>;
    %template(add_section_match) add_section<Match>;

  }
} // namespace Wccl

using namespace boost;
using namespace Wccl;
using namespace std;

#endif /* SWIG_LIBWCCL_WCCLFILE_I */
