#ifndef SWIG_LIBPWRNLP_WHITESPACE_I
#define SWIG_LIBPWRNLP_WHITESPACE_I

%module libpwrnlpwhitespace
%{
#include <libpwrutils/whitespace.h>
%}

// Force the enum to be "namespaced"
%rename(WS_None) PwrNlp::Whitespace::None;
%rename(WS_Space) PwrNlp::Whitespace::Space;
%rename(WS_ManySpaces) PwrNlp::Whitespace::ManySpaces;
%rename(WS_Newline) PwrNlp::Whitespace::Newline;
%rename(WS_ManyNewlines) PwrNlp::Whitespace::ManyNewlines;
%rename(WS_PostLast) PwrNlp::Whitespace::PostLast;
%rename(WS_to_string) PwrNlp::Whitespace::to_string;
%rename(WS_to_whitespace) PwrNlp::Whitespace::to_whitespace;
%rename(WS_from_string) PwrNlp::Whitespace::from_string;
%rename(WS_is_valid) PwrNlp::Whitespace::is_valid;

namespace PwrNlp {
  namespace Whitespace {
    enum Enum {
      None,
      Space,
      ManySpaces,
      Newline,
      ManyNewlines,
      PostLast // last item guard
    };

    std::string to_string(Enum wa);
    const char* to_whitespace(Enum wa);
    Enum from_string(const std::string& s);
    bool is_valid(Enum w);
  }
}

using namespace std;
using namespace PwrNlp::Whitespace;
using namespace PwrNlp;

#endif // SWIG_LIBPWRNLP_WHITESPACE_I
