#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <libcorpus2/tagset.h>
#include <libcorpus2/tagsetmanager.h>
#include <libcorpus2/io/reader.h>

#include <vector>


class config
{
public:
	
	/// Exception teling to skip the rest of the prograp because there is nothing to do, eg. parameter --help were given.
	class Skip {};
	
	
	/// get this singleton
	static config & get()
	{
		return *instance;
	}
	
	/// create this singleton
	static config & create(int argc, const char ** argv)
	{
		delete instance;
		return *(instance = new config(argc, argv));
	}
	
	
	
	const Corpus2::Tagset & getTagset() const
	{
		return *tagset;
	}
	
	const Corpus2::TokenReader::TokenReaderPtr getReader(boost::filesystem::path corpus_path) const
	{
		return Corpus2::TokenReader::create_path_reader(reader_name, getTagset(), corpus_path.string());
	}
	
	const boost::filesystem::path & getMorphfile() const
	{
		return morphfile;
	}
	
	const std::vector<boost::filesystem::path> & getCorpora() const
	{
		return corpora;
	}
	
	const boost::filesystem::path & getOutfile() const
	{
		return outfile;
	}
	
	bool isVerbose() const
	{
		return verbose;
	}
	
	
	/// check if the given word is in any way forbidden
	bool isForbidden(const UnicodeString & word) const
	{
		return word.length() < 3 || ignored_words.find(word) != ignored_words.end();
	}
	
	/// check if the given tag is in any way forbidden
	bool isForbidden(const Corpus2::Tag & tag) const
	{
		return ignored_poses.find(tag.get_pos_index()) != ignored_poses.end();
	}
	
	/// check if the given lexeme is in any way forbidden
	bool isForbidden(const Corpus2::Lexeme & lexeme) const
	{
		return isForbidden(lexeme.tag());
	}
	
	/// check if the given token is in any way forbidden
	bool isForbidden(const Corpus2::Token & token) const;
	
	
	
private:
	
	static config * instance;
	
	boost::filesystem::path morphfile;
	std::vector<boost::filesystem::path> corpora;
	std::set<UnicodeString> ignored_words;
	std::set<int> ignored_poses;
	boost::filesystem::path outfile;
	const Corpus2::Tagset * tagset;
	std::string reader_name;
	bool verbose;
	
	
	
	config(int argc, const char ** argv);
	
};

#define Cfg config::get()


#endif // CONFIG_HPP
