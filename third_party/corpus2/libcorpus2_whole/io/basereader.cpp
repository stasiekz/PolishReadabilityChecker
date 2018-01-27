#include <libcorpus2_whole/io/basereader.h>

namespace Corpus2 {
namespace whole {

typedef boost::shared_ptr<TokenReader> TokenReaderPtr;

BaseReader::BaseReader(std::string type) : DocumentReaderI(type){

}

BaseReader::BaseReader(std::string type, TokenReaderPtr reader) 
	: DocumentReaderI(type), reader_(reader){

}

boost::shared_ptr<Document> BaseReader::read() {
	boost::shared_ptr<Chunk> chunk;
	boost::shared_ptr<Document> document = boost::make_shared<Document>(id_);
	while(1) {
		chunk = reader_->get_next_chunk();
		if (!chunk) {
			break;
		} 
		else {
			document->add_paragraph(chunk);
		}
	}
	return document;
}

void BaseReader::set_reader(const TokenReaderPtr reader) {
	reader_ = reader;
}

void BaseReader::set_option(const std::string& option) {
	if (reader_)
		reader_->set_option(option);
	else
		throw Corpus2Error("Reader not initialized!");
}

std::string BaseReader::get_option(const std::string& option) const {
	if (reader_)
		return reader_->get_option(option);
	else
		throw Corpus2Error("Reader not initialized!");
}

} /* end ns whole */
} /* end ns Corpus2 */

