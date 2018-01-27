#ifndef SWIG_LIBMACA_PLAINTEXTREADER_I
#define SWIG_LIBMACA_PLAINTEXTREADER_I


%module libmacaplaintextreader
%{

#include <libmaca/io/text.h>

%}

%nodefaultctor;

%include "std_string.i"
%include "libcorpus2/tokenreader.i"

namespace Maca{

	class PlainTextReader{

	public:
	    static boost::shared_ptr<Corpus2::TokenReader> create_file_reader(const std::string& filename, const std::string& config);
	    static boost::shared_ptr<Corpus2::TokenReader> create_stream_reader(const std::string& config);
	    static boost::shared_ptr<Corpus2::TokenReader> create_string_reader(const std::string& inputText, const std::string& config);
	};
}

using namespace std;
using namespace Maca;
using namespace Corpus2;

#endif /* SWIG_LIBMACA_PLAINTEXTREADER_I */
