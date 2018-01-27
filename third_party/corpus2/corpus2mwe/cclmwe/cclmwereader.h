#ifndef CORPUS2_MWE_CCLMWEREADER_H
#define CORPUS2_MWE_CCLMWEREADER_H

#include <libmwereader/mwereader.h>
#include <libcorpus2_whole/io/baserelreader.h>

namespace Corpus2MWE {

    using namespace Corpus2;
    using namespace whole;

    class CclMWEReader : public BaseRelReader
    {
    /** 
     * Wrapper MWEReader(WCCL). Provides a simple interface 
     * to access a reader with MWE support.
     */
    public:
        typedef boost::shared_ptr<TokenReader> TokenReaderPtr;

	CclMWEReader(const std::string &doc_path, const Tagset &tagset,
			std::string mwe_file="");
	
	CclMWEReader(const std::string &doc_path, const std::string &rel_path,
			const Tagset &tagset, std::string mwe_file="");

	/** set new file to read */
	void set_files(const std::string &doc_path);
	void set_files(const std::string &doc_path, const std::string &rel_path);
    void use_annotations(bool annots_used);

	/** get the reader object */
        TokenReaderPtr reader();
        
    private:
	    /**
         * load MWE dictionary
         * @param mwe_file: dictionary with MWE operators
         */
        void init_mwes(const std::string &mwe_file);
        bool annotate;

	/**
	 * create instances of readers
	 */
	void make_readers(const Tagset &tagset, const std::string &doc_path, 
			const std::string &rel_path="");
    };

} /* end of ns Corpus2MWE */

#endif //CORPUS2_MWE_CCLMWEREADER_H
