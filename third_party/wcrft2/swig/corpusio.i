#ifndef SWIG_WCRFT2_CORPUSIO_I
#define SWIG_WCRFT2_CORPUSIO_I

%module corpusio
%{
	#include <libwcrft/corpusio.h>
%}

%include "std_string.i"

%include "libcorpus2/tagset.i"
%include "libcorpus2/tokenreader.i"
%include "libcorpus2/tokenwriter.i"

namespace Wcrft {
	boost::shared_ptr<Corpus2::TokenReader> get_reader(
			const std::string& input_path, const std::string& input_format,
			const Corpus2::Tagset& tagset, const std::string& maca_config = "");

	boost::shared_ptr<Corpus2::TokenReader> get_reader(
			std::istream& input_stream, const std::string& input_format,
			const Corpus2::Tagset& tagset, const std::string& maca_config = "");

	boost::shared_ptr<Corpus2::TokenWriter> get_writer(
			const std::string& output_path, const std::string& output_format,
			const Corpus2::Tagset& tagset);

	boost::shared_ptr<Corpus2::TokenWriter> get_writer(
			std::ostream& output_stream, const std::string& output_format,
			const Corpus2::Tagset& tagset);
}

using namespace std;
using namespace boost;
using namespace Wcrft;
using namespace Corpus2;

#endif
