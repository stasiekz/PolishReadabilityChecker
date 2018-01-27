#include <libcorpus2_whole/io/baserelreader.h>

namespace Corpus2 {
namespace whole {

BaseRelReader::BaseRelReader(std::string type) : BaseReader(type) 
{ }

BaseRelReader::BaseRelReader(std::string type, TokenReaderPtr reader,
		RelationReaderPtr rel_reader)
	: BaseReader(type, reader), rel_reader_(rel_reader)
{ }

boost::shared_ptr<Document> BaseRelReader::read() 
{
	boost::shared_ptr<Document> document;

	// Reads ccl and makes a document object
	document = BaseReader::read();

	// Read relations and add them to the document obj.
	if (rel_reader_) {
		const std::vector< boost::shared_ptr<Relation> > relations =
			rel_reader_->relations();
		for (unsigned int i = 0; i < relations.size(); i++) {
			document->add_relation(relations[i]);
		}
	}

	return document;
}

void BaseRelReader::make_id_doc(const std::string &annot_path,
		const std::string &rela_path)
{
	id_ = (annot_path + ";" + rela_path);
}

} /* end of ns whole */
} /* end of ns Corpus2 */
