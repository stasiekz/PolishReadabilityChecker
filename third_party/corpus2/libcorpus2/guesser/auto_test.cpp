#include <libcorpus2/guesser/guesser.h>
#include <libcorpus2/tagsetmanager.h>
#include <libcorpus2/io/reader.h>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH


using namespace	Corpus2;
using namespace boost::filesystem;
using namespace boost::program_options;



std::set<int> ignored_poses;


bool isForbidden(const Corpus2::Tag & tag)
{
	return ignored_poses.find(tag.get_pos_index()) != ignored_poses.end();
}
bool isForbidden(const Corpus2::Lexeme & lexeme)
{
	return isForbidden(lexeme.tag());
}
bool isForbidden(const Corpus2::Token & token)
{
	foreach	(const Corpus2::Lexeme & lx, token.lexemes())
		if (!isForbidden(lx))
			return false;
	
	return true;
}


int main(int argc, const char ** argv)
{
	// config
	path corpus, data;
	std::string tagset_name, reader_name;
	std::vector<std::string> ignored_pos_names;
	
	options_description desc("Allowed options");
	desc.add_options()
			("help", "produce help message")
			("corpus,c", value<path>(&corpus)->required(), "corpus whence tests will be taken. Each token is a test: orth is what we ask Guesser, each lexeme is what the guesser should return.")
	        ("data,d", value<path>(&data)->required(), "data of guesser")
	        ("tagset,t", value<std::string>(&tagset_name)->default_value("nkjp"), "tagset corporis")
	        ("reader,r", value<std::string>(&reader_name)->default_value("xces"), "reader for corpus")
	        ("ignore-poses,p", value< std::vector<std::string> >(&ignored_pos_names)->multitoken(), "list of POSes to be ignored")
	;
	
	
	variables_map vm;
	store(parse_command_line(argc, argv, desc), vm);
	
	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return 0;
	}
	
	notify(vm);
	
	const Tagset & tagset = get_named_tagset(tagset_name);
	
	foreach	(const std::string & pos, ignored_pos_names)
		ignored_poses.insert(tagset.get_pos_index(pos));
	
	
	// preparations
	
	Guesser g(data, tagset);
	TokenReader::TokenReaderPtr reader = Corpus2::TokenReader::create_path_reader(reader_name, tagset, corpus.string());
	
	
	long good_c=0, all_c=0, zero_c=0, forbidden_c=0;
	double coverage=0, quality=0;
	
	
	// for each test
	Sentence::Ptr sentence;
	while((sentence = reader->get_next_sentence()))
		foreach (const Token * test, sentence->tokens())
		{
			if (isForbidden(*test)) // if it is not forbidden one
			{
				forbidden_c++;
				continue;
			}
			
			std::cout << "\n" << test->orth() << std::endl;
			
			std::vector<Lexeme> gl = g.guess(test->orth());
			const std::vector<Lexeme> & rl = test->lexemes();
			
			std::set<Lexeme> guessed_lexemes(gl.begin(), gl.end());
			std::set<Lexeme> real_lexemes(rl.begin(), rl.end());
			
			bool good = true;
			long false_c=0;
			        
			foreach (const Lexeme & l, guessed_lexemes)
				if (!isForbidden(l) && real_lexemes.find(l) == real_lexemes.end())
					std::cout << "\tfalse   " << l.lemma() << " " << tagset.tag_to_string(l.tag()) << std::endl, false_c++, good=false;
			foreach (const Lexeme & l, real_lexemes)
				if (!isForbidden(l) && guessed_lexemes.find(l) == guessed_lexemes.end())
					std::cout << "\tmissed  " << l.lemma() << " " << tagset.tag_to_string(l.tag()) << std::endl, good=false;
			
			if (guessed_lexemes.empty())
				zero_c++;
			if (good)
				good_c++;
			all_c++;
			
			coverage += (guessed_lexemes.size() - false_c) / (double)real_lexemes.size();
			
			if (!guessed_lexemes.empty())
				quality += (guessed_lexemes.size() - false_c) / (double)guessed_lexemes.size();
		}
	
	std::cout << "\n\nGuessed: " << good_c << " out of " << all_c << ", not answered " << zero_c << std::endl;
	std::cout << "Average coverage: " << coverage/(double)all_c << "\tAverage quality: " << quality/(double)(all_c - zero_c) << std::endl;
	std::cout << "\n\nForbidden: " << forbidden_c << std::endl;
}
