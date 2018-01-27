#include "config.h"


#include <libcorpus2/tagsetmanager.h>
#include <iostream>
#include <fstream>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

using namespace Corpus2;
using namespace boost::filesystem;


config * config::instance = NULL;



config::config(int argc, const char ** argv)
{
	using namespace boost::program_options;
	
	path corpus;
	path ignore_file;
	std::vector<std::string> ignored_poses;
	bool batch_mode;
	std::string tagset_name;
	
	
	options_description desc("Allowed options");
	desc.add_options()
			("help", "produce help message")
			("morphfile,m", value<path>(&morphfile), "morpheus dictionary through which all words shall be filtred")
			("corpus,c", value<path>(&corpus)->required(), "corpus whence statistcs will be gotten")
	        ("outfile,o", value<path>(&outfile)->required(), "file whither result should be stored")
	        ("batch,b", bool_switch(&batch_mode)->default_value(false), "if set, corpus will be treated as a list corporum")
	        ("tagset,t", value<std::string>(&tagset_name)->default_value("nkjp"), "tagset corporis")
	        ("reader,r", value<std::string>(&reader_name)->default_value("xces"), "reader for corpus")
	        ("ignore-from,i", value<path>(&ignore_file), "file where are listed all orths that should be ignored")
	        ("ignore-poses,p", value< std::vector<std::string> >(&ignored_poses)->multitoken(), "list of POSes to be ignored")
	        ("verbose,v", bool_switch(&verbose)->default_value(false), "if the program should talk")
	;
	
	
	variables_map vm;
	store(parse_command_line(argc, argv, desc), vm);
	
	if (vm.count("help"))
		std::cout << desc << std::endl, throw Skip();
	
	notify(vm);
	
	
	if (batch_mode)
	{
		std::ifstream file(corpus.c_str());
		
		std::string line;
		while (getline(file, line))
			corpora.push_back(line);
	}
	else
		corpora.push_back(corpus);
	
	if (!ignore_file.empty())
	{
		std::ifstream file(ignore_file.c_str());
		
		std::string line;
		while (getline(file, line))
			ignored_words.insert(UnicodeString(line.c_str()).trim());
	}
	
	tagset = &Corpus2::get_named_tagset(tagset_name);
	
	foreach	(const std::string & pos, ignored_poses)
		this->ignored_poses.insert(tagset->get_pos_index(pos));
}

bool config::isForbidden(const Token &token) const
{
	if (isForbidden(token.orth()))
		return true;
	
	foreach	(const Corpus2::Lexeme & lx, token.lexemes())
		if (!isForbidden(lx))
			return false;
	
	return true;
}










