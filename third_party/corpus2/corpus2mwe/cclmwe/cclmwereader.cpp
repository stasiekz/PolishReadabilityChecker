#include <cclmwe/cclmwereader.h>
#include <cclmwe/mwemanager.h>

#include <boost/filesystem.hpp>
#include <boost/make_shared.hpp>

using namespace Corpus2MWE;

typedef boost::shared_ptr<TokenReader> TokenReaderPtr;
typedef boost::shared_ptr<MWEReader> MWEReaderPtr;


CclMWEReader::CclMWEReader(const std::string &doc_path, const Tagset &tagset,
		std::string mwe_file) : BaseRelReader("document"), annotate(true) 
{
	make_readers(tagset, doc_path);
	BaseRelReader::make_id_doc(doc_path, doc_path);
	if (mwe_file == "")
		mwe_file = get_mwe_dict("mwe");
	init_mwes(mwe_file);
}

CclMWEReader::CclMWEReader(const std::string &doc_path, const std::string &rel_path,
		const Tagset &tagset, std::string mwe_file) 
		: BaseRelReader("document"), annotate(true)
{
	make_readers(tagset, doc_path, rel_path);
	BaseRelReader::make_id_doc(doc_path, rel_path);
	if (mwe_file == "")
		mwe_file = get_mwe_dict("mwe");
	init_mwes(mwe_file);
}

void CclMWEReader::use_annotations(bool val) {
	if (val) {
		reader_->set_option("annotations:1");
	} else {
		reader_->set_option("annotations:0");
	}
}

void CclMWEReader::make_readers(const Tagset &tagset, const std::string &doc_path,
		const std::string &rel_path) 
{
	reader_ = boost::make_shared<MWEReader>(tagset, doc_path);
	if (rel_path != "")	
		rel_reader_ = boost::make_shared<RelationReader>(rel_path);
}

void CclMWEReader::set_files(const std::string &doc_path) 
{
	reader_->setFile(doc_path);
	BaseRelReader::make_id_doc(doc_path, doc_path);
}

void CclMWEReader::set_files(const std::string &doc_path,
		const std::string &rel_path)
{
	reader_->setFile(doc_path);
	rel_reader_ = boost::make_shared<RelationReader>(rel_path);
	BaseRelReader::make_id_doc(doc_path, rel_path);
}

void CclMWEReader::init_mwes(const std::string &mwe_file) {
    if (reader_) {
    	reader_->set_option("inner:ccl");
	if (boost::filesystem::exists(mwe_file)) {
		reader_->set_option("mwefile:" + mwe_file);
	} 
	else {
		throw Corpus2Error("MWE dictionary does not exist!");
	}
    } 
    else {
	throw Corpus2Error("Reader not initialized!");
    }
}

TokenReaderPtr CclMWEReader::reader() {
	return reader_;
}
