#ifndef SWIG_LIBWCCL_POSITION_I
#define SWIG_LIBWCCL_POSITION_I

%module libcclposition
%{
  #include <libwccl/values/value.h>
  #include <libwccl/values/position.h>
  #include <boost/integer_traits.hpp>
%}

%include "value.i"
%include "std_string.i"

%feature("notabstract") Wccl::Position;

%template(PositionPtr) boost::shared_ptr<Wccl::Position>;
%template(ConstPositionPtr) boost::shared_ptr<Wccl::Position const>;

namespace Wccl {
  class Position : public Value {
  public:
    const char* get_type_name() const { return type_name; }
    static std::string var_repr(const std::string &var_name);
    std::string make_var_repr(const std::string &var_name) const {
      return var_repr(var_name);
    }

    typedef int value_type;
    
    enum Enum {
      Nowhere = boost::integer_traits<int>::const_min,
      Begin = boost::integer_traits<int>::const_min + 1,
      End = boost::integer_traits<int>::const_max
    };

    explicit Position(int v = Nowhere);
    const int& get_value() const;
    void set_value(int v);
    std::string to_raw_string() const;
    bool equals(const Position& other) const;

    // bool equals(const Position& other, const SentenceContext& context) const;
  };
}

using namespace std;
using namespace Wccl;

#endif /* SWIG_LIBWCCL_POSITION_I */
