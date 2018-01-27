#ifndef BASERELREADER_H
#define BASERELREADER_H

#include <libcorpus2_whole/io/basereader.h>
#include <libcorpus2_whole/io/relreader.h>

#include <boost/shared_ptr.hpp>

namespace Corpus2 {
namespace whole {
/**
 * Base implementation of Document Reader with relations support.
 * Uses custom type of doc reader (CclReader, MWEReader, ...).
 */
class BaseRelReader : public BaseReader {

public:
	typedef boost::shared_ptr<TokenReader> TokenReaderPtr;
	typedef boost::shared_ptr<RelationReader> RelationReaderPtr;

	BaseRelReader(std::string type);

	/**
	 * initializes DocumentReader and RelationReader
	 */
	BaseRelReader(std::string type, TokenReaderPtr reader,
			RelationReaderPtr rel_reader);

	/**
	 * Reads document file with relations from corresponding 
	 * relations file.
	 */
	virtual boost::shared_ptr<Document> read();

protected:
	

	/** RelationReader Pointer */
	boost::shared_ptr<RelationReader> rel_reader_;

	void set_rel_reader(RelationReaderPtr reader);

	/**
	 * Based on given paths (annotations and relations) makes an indetifier
	 * of document.
	 */
	void make_id_doc(const std::string &annot_path, 
			const std::string &rela_path);

};

} /* end of ns whole */
} /* end of ns Corpus2 */

#endif // BASERELREADER_H
