#ifndef SWIG_LIBWCCL_STRSET_I
#define SWIG_LIBWCCL_STRSET_I

%module libcclstrset
%{
  #include <libwccl/values/value.h>
  #include <libwccl/values/strset.h>
  #include <boost/unordered_set.hpp>
  #include <libcorpus2/lexeme.h>
%}

%include "value.i"
%include "std_string.i"
%include "std_set.i"

%feature("notabstract") Wccl::StrSet;

%template(StrSetPtr) boost::shared_ptr<Wccl::StrSet>;
%template(ConstStrSetPtr) boost::shared_ptr<Wccl::StrSet const>;

%template(UnicodeStringMap) std::set<UnicodeString>;

namespace Wccl {
  class StrSet : public Value {
  public:
    const char* get_type_name() const { return type_name; }
    static std::string var_repr(const std::string &var_name);
    std::string make_var_repr(const std::string &var_name) const {
      return var_repr(var_name);
    }

    typedef boost::unordered_set<UnicodeString> value_type;

    StrSet();
    explicit StrSet(const value_type& s);

    const value_type& get_value() const;
    void set_value(const value_type& set);
    const value_type& contents() const;
    value_type& contents();
    void swap(StrSet& ss);
  
    // TEST IT!
    void insert(const UnicodeString& u);
    void insert_utf8(const std::string& u);

    int size() const;
    bool empty() const;
    bool is_subset_of(const StrSet& other) const;
    bool intersects(const StrSet& other) const;
    bool equals(const StrSet& other) const;
    std::string to_raw_string() const;

    // 
    UnicodeString to_raw_string_u() const;

    //
    std::set<UnicodeString> to_std_set() const;
  };
}

using namespace std;
using namespace Wccl;

#endif /* SWIG_LIBWCCL_STRSET_I */
