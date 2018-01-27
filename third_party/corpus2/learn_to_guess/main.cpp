#include "tree.h"
#include "config.h"

#include <fstream>
#include <set>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include <libcorpus2/guesser/guesser.h>

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace Corpus2;




set<UnicodeString> prepare_morphfile(const path & morphfile)
{
	set<UnicodeString> result;
	
	ifstream file(morphfile.c_str());
	
	string line;
	while (getline(file, line))
	{
		std::vector <std::string> splt;
		split(splt, line, is_any_of("\t"));
		
		result.insert(UnicodeString(splt[0].c_str()));
	}
	
	return result;
}

void add_corpus(const path & path, const set<UnicodeString> & real_words, Tree & tree)
{
	TokenReader::TokenReaderPtr reader = Cfg.getReader(path);
	
	Sentence::Ptr sentence;
	while((sentence = reader->get_next_sentence()))
		foreach (const Token * token, sentence->tokens())
			if (!Cfg.isForbidden(*token) && (real_words.empty() || real_words.find(token->orth()) != real_words.end()))
				tree.insert(*token);
}


int main(int argc, const char ** argv)
{
	try {
		config::create(argc, argv);
		
		Tree tree;
		
		set<UnicodeString> real_words;
		if (!Cfg.getMorphfile().empty())
		{
			if (Cfg.isVerbose()) cout << "Preparing morphfile " << Cfg.getMorphfile() << endl;
			real_words = prepare_morphfile(Cfg.getMorphfile());
		}
		
		/*
		Token token;
		
		#define it(ORTH, TAG)\
			token.set_orth(ORTH);\
			token.add_lexeme(Lexeme(ORTH, Cfg.getTagset().parse_simple_tag(TAG)));\
			tree.insert(token);
		
		it("hamak", "subst:nom:sg:m1");
		it("bamak", "subst:nom:sg:m1");
		/*/
		if (Cfg.isVerbose()) cout << "Reading corpora" << endl;
		foreach (path corpus, Cfg.getCorpora())
		{
			if (Cfg.isVerbose()) cout << "\treading through " << corpus << endl;
			add_corpus(corpus, real_words, tree);
		}
		//*/
		
		tree.compressLemmawise();
		tree.gatherTags();
		tree.prune();
		
		tree.write(Cfg.getOutfile());
		tree.print();
		
	}
	catch (const config::Skip & e)
	{
		// Do nothing.
	}
	catch (const program_options::error & e)
	{
		cerr << "Command line error: " << e.what() << endl;
	}
	catch (const std::exception & e)
	{
		cerr << "Error: " << e.what() << endl;
	}
	catch (...)
	{
		cerr << "Unknown exception!" << endl;
	}
}
