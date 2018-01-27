#ifndef BASEREADER_H
#define BASEREADER_H

#include <libcorpus2/io/reader.h>
#include <libcorpus2_whole/io/reader_i.h>
#include <libcorpus2_whole/document.h>

#include <boost/shared_ptr.hpp>

namespace Corpus2 {
namespace whole {
/**
 * Base implementation of DocumentReader interface. Reads document
 * with a custom type of reader. 
 */
class BaseReader : public DocumentReaderI {
public:
	typedef boost::shared_ptr<TokenReader> TokenReaderPtr;
	/**
	 * initializes Document Reader 
	 */
	BaseReader(std::string type);

	/**
	 * initializes Document Reader with selected type of Reader
	 */
	BaseReader(std::string type, TokenReaderPtr reader);

	/**
	 * Sets options for selected reader. Note that readers can differ in 
	 * the available set of options. Use correct options for concrete
	 * type of readers.
	 * @param option  name of the option to set
	 */
	virtual void set_option(const std::string& option);

	/**
	 * Get an option from reader.
	 * @return option
	 */
	virtual std::string get_option(const std::string& option) const;

	/**
	 * Reads a file with selected type of reader.
	 * @return Pointer to Corpus2 Document
	 */
	virtual boost::shared_ptr<Document> read();

protected:
	/** set a reader */
	void set_reader(TokenReaderPtr reader);

	std::string id_;

	/** Reader Pointer */
	TokenReaderPtr reader_;
};

} /* end ns whole */
} /* end ns Corpus2 */

#endif // BASEREADER_H
