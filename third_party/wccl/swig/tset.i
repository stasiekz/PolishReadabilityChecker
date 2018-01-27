#ifndef SWIG_LIBWCCL_TSET_I
#define SWIG_LIBWCCL_TSET_I

%module libccltset
%{
  #include <libwccl/values/value.h>
  #include <libwccl/values/tset.h>
%}

/* Include required headers */
%include "std_string.i"
%include "value.i"
%include "libcorpus2/tag.i"
%include "libcorpus2/tagset.i"

%feature("notabstract") Wccl::TSet;

%template(TSetPtr) boost::shared_ptr<Wccl::TSet>;
%template(ConstTSetPtr) boost::shared_ptr<Wccl::TSet const>;

namespace Wccl {
  class TSet : public Value {
  public:
    const char* get_type_name() const { return type_name; }
    static std::string var_repr(const std::string &var_name);
    std::string make_var_repr(const std::string &var_name) const {
      return var_repr(var_name);
    }

    TSet();
    explicit TSet(Corpus2::Tag tag);

    const Corpus2::Tag& get_value() const;
    void set_value(const Corpus2::Tag& tag);
    const Corpus2::Tag& contents();

    void insert_symbol(const Corpus2::Tagset& tagset, const std::string& s);

    bool empty() const;
    bool is_subset_of(const TSet& other) const;
    bool intersects(const TSet& other) const;
    bool equals(const TSet& other) const;

    int categories_count(const Corpus2::Tagset& tagset) const;
    int matching_categories(const Corpus2::Tag& tag) const;

    void combine_with(const Corpus2::Tag& other);
    void combine_with(const TSet& other);

    std::string to_string(const Corpus2::Tagset &) const;
    std::string to_raw_string() const;
  };
}

using namespace std;
using namespace Wccl;

#endif /* SWIG_LIBWCCL_TSET_I */
