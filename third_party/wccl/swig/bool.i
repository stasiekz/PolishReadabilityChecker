#ifndef SWIG_LIBWCCL_BOOL_I
#define SWIG_LIBWCCL_BOOL_I

%module libcclbool
%{
  #include <libwccl/values/value.h>
  #include <libwccl/values/bool.h>
%}

%include "value.i"
%include "std_string.i"

%feature("notabstract") Wccl::Bool;

%template(BoolPtr) boost::shared_ptr<Wccl::Bool>;
%template(ConstBoolPtr) boost::shared_ptr<Wccl::Bool const>;

namespace Wccl {
  class Bool : public Value {
  public:
    const char* get_type_name() const { return type_name; }
    static std::string var_repr(const std::string &var_name);
    std::string make_var_repr(const std::string &var_name) const {
      return var_repr(var_name);
    }

    explicit Bool(bool v);

    const bool& get_value() const;
    void set_value(bool v);
    bool equals(const Bool& other) const;
    std::string to_raw_string() const;
  };
}

using namespace std;
using namespace Wccl;

#endif /* SWIG_LIBWCCL_BOOL_I */
