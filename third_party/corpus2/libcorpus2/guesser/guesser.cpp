#include <libcorpus2/guesser/guesser.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include <libcorpus2/tagset.h>


namespace Corpus2 {



void detail::tags_info::print(std::ostream &stream) const
{
	foreach (value_type kv, *this)
		stream << "[" << kv.first.get_pos() << "\t" << kv.first.get_values() << "\t" << kv.second.first << "\t" << kv.second.second << "]";
}

template <typename T>
static void bin_dedump(std::istream &stream, T & dedumpendum)
{
	stream.read((char*)&dedumpendum, sizeof(T));
}

void detail::tags_info::dedump(std::istream &stream)
{
	std::size_t tc;
	bin_dedump(stream, tc);
	
	for (std::size_t i = 0; i < tc; i++)
	{
		unsigned long pos, val;
		std::string r;
		int r2;
		
		bin_dedump(stream, pos);
		bin_dedump(stream, val);
		bin_dedump(stream, r2);
		std::getline(stream, r);
		(*this)[Tag(pos, val)] = std::make_pair(r.c_str(), r2);
	}
}



Guesser::Guesser(const boost::filesystem::path &path, const Tagset &tagset) :
    tagset(tagset)
{
	tree.read(path);
}


void get_aux(const detail::tags_info ** last_good, const detail::tags_info & prop)
{
	if (!prop.empty())
		*last_good = &prop;
}

const detail::tags_info &Guesser::Tree::get(const UnicodeString &word) const
{
	// get the downmost node that have any tag
	const detail::tags_info * last_good = &root.properties;
	find(word, bind(&get_aux, &last_good, _1));
	return *last_good;
}

std::vector<Lexeme> Guesser::guess(const UnicodeString &word) const
{
	const Corpus2::mask_t sup_mask = tagset.get_value_mask(std::string("sup"));
	const Corpus2::mask_t com_mask = tagset.get_value_mask(std::string("com"));
	const Corpus2::mask_t aff_mask = tagset.get_value_mask(std::string("aff"));
	const Corpus2::mask_t neg_mask = tagset.get_value_mask(std::string("neg"));
	
	
	std::vector<Lexeme> res;
	
	if (word.startsWith("nie"))   // If begins with 'nie'
	{
		UnicodeString orth = word.tempSubString(3);   // at first assume its prefix
		const detail::tags_info & inf = tree.get(orth);
		foreach (detail::tags_info::value_type kv, inf)
		{
			if (kv.first.get_values_for(aff_mask).any())
			{
				UnicodeString lemma = orth.tempSubString(0, orth.length() - kv.second.second) + kv.second.first;
				Tag tag = kv.first;
				tag.set_values(((tag.get_values()) & ~aff_mask) | neg_mask);
				res.push_back(Lexeme(lemma, tag));
			}
		}
		
		const detail::tags_info & inf2 = tree.get(word);  // then assume it's part of the word
		foreach (detail::tags_info::value_type kv, inf2)
		{
			if (&inf2 != &inf || !kv.first.get_values_for(aff_mask).any())
			{
				UnicodeString lemma = word.tempSubString(0, word.length() - kv.second.second) + kv.second.first;
				res.push_back(Lexeme(lemma, kv.first));
			}
		}
	}
	else if (word.startsWith("naj"))   // If begins with 'naj'
	{
		UnicodeString orth = word.tempSubString(3);   // at first assume its prefix
		const detail::tags_info & inf = tree.get(orth);
		
		foreach (detail::tags_info::value_type kv, inf)
		{
			if (kv.first.get_values_for(com_mask).any())
			{
				UnicodeString lemma = orth.tempSubString(0, orth.length() - kv.second.second) + kv.second.first;
				Tag tag = kv.first;
				tag.set_values(((tag.get_values()) & ~com_mask) | sup_mask);
				res.push_back(Lexeme(lemma, tag));
			}
		}
		
		const detail::tags_info & inf2 = tree.get(word);  // then assume it's part of the word
		foreach (detail::tags_info::value_type kv, inf2)
		{
			if (&inf2 != &inf || !kv.first.get_values_for(com_mask).any())
			{
				UnicodeString lemma = word.tempSubString(0, word.length() - kv.second.second) + kv.second.first;
				res.push_back(Lexeme(lemma, kv.first));
			}
		}
	}
	else   // If does not begin with 'nie' nor 'naj'
	{
		const detail::tags_info & inf = tree.get(word);  // find it
		foreach (detail::tags_info::value_type kv, inf)  // for each possible tag
		{
			// prepare lemma following its recipe
			UnicodeString lemma = word.tempSubString(0, word.length() - kv.second.second) + kv.second.first;
			
			// add to results
			res.push_back(Lexeme(lemma, kv.first));
		}
	}
	
	return res;
}

void Guesser::print() const
{
	tree.print();
}

}
