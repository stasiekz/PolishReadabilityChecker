#ifndef SWIG_UNICODESTRING_I
#define SWIG_UNICODESTRING_I

%module unicodestring
%{
  #include <unicode/unistr.h>
%} 

%include "std_string.i"
%include "std_except.i"


class UnicodeString {
  public:
    int length();
    UChar* getBuffer();
    UChar* getTerminatedBuffer();

%pythoncode %{
  def __unicode__(self):
    return self.as_utf16().decode('utf16')

  def __str__(self):
    return self.as_utf8()
%}

};

%extend UnicodeString {
    std::string as_utf16() {
        return std::string((char*)self->getTerminatedBuffer(), self->length()*2);
    }

  std::string as_utf8() {
    std::string r;
    self->toUTF8String(r);
    return r;
  }
}


#endif /* SWIG_UNICODESTRING_I */
