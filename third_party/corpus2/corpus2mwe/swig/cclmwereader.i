#ifndef SWIG_CORPUS2MWE_CCLMWEREADER_I
#define SWIG_CORPUS2MWE_CCLMWEREADER_I

%module unicode_strings

%include <std_string.i>

%begin %{
    #define SWIG_PYTHON_2_UNICODE
%}

%module cclmwereader
%{
    #include <libmwereader/mwereader.h>
    #include <libcorpus2_whole/io/baserelreader.h>
    #include <cclmwe/cclmwereader.h>
%}

%include <libcorpus2/boost_shared_ptr.i>

namespace Corpus2MWE {
    class CclMWEReader {
        public:
            typedef boost::shared_ptr<Corpus2::TokenReader> TokenReaderPtr;

            CclMWEReader(const std::string &doc_path, const std::string &rel_path, 
                        const Corpus2::Tagset & tagset, std::string mwe_file="");

            CclMWEReader(const std::string &doc_path, const Corpus2::Tagset & tagset, 
                        std::string mwe_file="");

            boost::shared_ptr<Corpus2::whole::Document> read();

            void set_option(const std::string &option);

            std::string get_option(const std::string &option) const;

            void set_files(const std::string &filename);
            void set_files(const std::string &filename, const std::string &rel_path);
            void use_annotations(bool val);

            TokenReaderPtr reader();
    };
}

using namespace boost;
using namespace Corpus2MWE;

#endif /* SWIG_CORPUS2MWE_CCLMWEREADER_I */
