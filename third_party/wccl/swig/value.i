#ifndef SWIG_LIBWCCL_VALUE_I
#define SWIG_LIBWCCL_VALUE_I

%module libcclvalue
%{
  #include <libwccl/values/value.h>
%}

%include "std_string.i"
%include "libcorpus2/tagset.i"

%nodefaultctor Wccl::Value;

%template(ValuePtr) boost::shared_ptr<Wccl::Value>;
%template(ConstValuePtr) boost::shared_ptr<Wccl::Value const>;

namespace Wccl {
  class Value {
  public:
    static const char type_name[56];
    virtual const char* get_type_name() const;

    /* --------------------------------------------------------------------- */

    virtual ~Value();

    /* --------------------------------------------------------------------- */

    virtual std::string make_var_repr(const std::string&) const = 0;

    /* --------------------------------------------------------------------- */

    virtual std::string to_string(const Corpus2::Tagset& /*tagset*/) const;
    virtual std::string to_raw_string() const = 0;
    virtual std::string to_compact_string(const Corpus2::Tagset& /*tagset*/) const;
  };

}

using namespace std;
using namespace Wccl;

#endif /* SWIG_LIBWCCL_VALUE_I */
