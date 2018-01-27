#ifndef SWIG_LIBCORPUS2_IOB_I
#define SWIG_LIBCORPUS2_IOB_I

%module libcorpusiob
%{
  #include <libcorpus2/ann/iob.h>
%}
%include "std_vector.i"

// Force the enum to be "namespaced"
%rename(IOB_O) Corpus2::IOB::O;
%rename(IOB_B) Corpus2::IOB::B;
%rename(IOB_I) Corpus2::IOB::I;
%rename(IOB_PostLast) Corpus2::IOB::PostLast;
%rename(IOB_to_string) Corpus2::IOB::to_string;
%rename(IOB_from_string) Corpus2::IOB::from_string;

namespace Corpus2 {
  namespace IOB {
    enum Enum {
      O = 0,
      B = 1,
      I = 2,
      PostLast
    };

    const char* to_string(Enum iob);
    Enum from_string(const std::string& s);
  }
}

%template(IOBVector) std::vector<Corpus2::IOB::Enum>;

using namespace std;
using namespace Corpus2::IOB;
using namespace Corpus2;

#endif /* SWIG_LIBCORPUS2_IOB_I */
