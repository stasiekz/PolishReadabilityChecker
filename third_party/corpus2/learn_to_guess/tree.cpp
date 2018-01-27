#include "tree.h"
#include "config.h"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <unicode/ustream.h>

#include <libcorpus2/tagsetmanager.h>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH



Properties::Properties()
{
	counter = 0;
	depth = 0;
}

void Properties::feedOn(Properties &another)
{
	foreach (tag_map::value_type kv, another.tags)
	{
		typedef std::map<recipe, int> rec_info;
		rec_info & my_recipes = tags[kv.first].recipes;
		rec_info & its_recipes = kv.second.recipes;
		
		foreach (rec_info::value_type it, its_recipes)
			my_recipes[it.first] += it.second;
		
		tags[kv.first].strength += kv.second.strength;
	}
	
	another.tags.clear();
}


template <typename T>
static void bin_dump(std::ostream &stream, const T & dumpendum)
{
	stream.write((char*)&dumpendum, sizeof(T));
}

void Properties::dump(std::ostream &stream) const
{
	bin_dump(stream, tags.size());
	
	foreach (Properties::tag_map::value_type kv, tags)
	{
		typedef std::map<Properties::recipe, int> rec_info;
		rec_info::iterator best = kv.second.recipes.begin();
		for (rec_info::iterator it = best; it != kv.second.recipes.end(); ++it)
			if (it->second > best->second)
				best = it;
		
		bin_dump(stream, kv.first.get_pos().to_ulong());
		bin_dump(stream, kv.first.get_values().to_ulong());
		bin_dump(stream, best->first.second);
		
		std::string str;
		best->first.first.toUTF8String(str);
		stream.write(str.c_str(), str.size());
		stream.put('\n');
	}
}

void Properties::print(std::ostream &stream) const
{
	foreach (Properties::tag_map::value_type kv, tags)
	{
		typedef std::map<Properties::recipe, int> rec_info;
		rec_info::iterator best = kv.second.recipes.begin();
		for (rec_info::iterator it = best; it != kv.second.recipes.end(); ++it)
			if (it->second > best->second)
				best = it;
		
		std::string str;
		best->first.first.toUTF8String(str);
		stream << '[' << Cfg.getTagset().tag_to_string(kv.first) << ", " << boost::io::quoted(str) << ", " << best->first.second << ']';
	}
}





typedef boost::shared_ptr<Corpus2::Token> TokenPtr;

/// Extracts all negated content into separate token, forced into affirmative
TokenPtr extract_negation(TokenPtr token)
{
	TokenPtr neg_token(new Corpus2::Token);
	neg_token->set_orth(UnicodeString(token->orth()).remove(0, 3));
	
	
	const static Corpus2::mask_t neg_mask = Cfg.getTagset().get_value_mask(std::string("neg"));
	const static Corpus2::mask_t aff_mask = Cfg.getTagset().get_value_mask(std::string("aff"));
	
	std::vector<Corpus2::Lexeme> lexemes = token->lexemes();
	token->remove_all_lexemes();
	
	foreach (Corpus2::Lexeme & lx, lexemes)
	{
		if (lx.tag().get_values_for(neg_mask).any())
		{
			Corpus2::Tag tag = lx.tag();
			tag.set_values(((lx.tag().get_values()) & ~neg_mask) | aff_mask);
			lx.set_tag(tag);
			neg_token->add_lexeme(lx);
		}
		else
		{
			token->add_lexeme(lx);
		}
	}
	
	return neg_token;
}

/// Extracts all superlative content into separate token, forced into compartive
TokenPtr extract_superlativum(TokenPtr token)
{
	TokenPtr sup_token(new Corpus2::Token);
	token->set_orth(token->orth());
	sup_token->set_orth(UnicodeString(token->orth()).remove(0, 3));
	
	
	const static Corpus2::mask_t sup_mask = Cfg.getTagset().get_value_mask(std::string("sup"));
	const static Corpus2::mask_t com_mask = Cfg.getTagset().get_value_mask(std::string("com"));
	
	std::vector<Corpus2::Lexeme> lexemes = token->lexemes();
	token->remove_all_lexemes();
	
	foreach (Corpus2::Lexeme & lx, lexemes)
	{
		if (lx.tag().get_values_for(sup_mask).any())
		{
			Corpus2::Tag tag = lx.tag();
			tag.set_values(((lx.tag().get_values()) & ~sup_mask) | com_mask);
			lx.set_tag(tag);
			sup_token->add_lexeme(lx);
		}
		else
		{
			token->add_lexeme(lx);
		}
	}
	
	return sup_token;
}

void increase_count(Properties & prop)
{
	prop.counter++;
}

void Tree::insert_aux(const Corpus2::Token &token)
{
	// insert
	
	UnicodeString orth = token.orth();
	orth.toLower();
	
	Node & n = find(orth, &increase_count);
	
	Node * last_node = &n;
	Node ** it = !n.first_child ? &n.first_child : &n.lastChild()->next_sibling;
	for (int i = n.properties.depth; i < orth.length(); i++)
	{
		last_node = new Node(orth[orth.length()-1-i]);
		last_node->properties.depth = i+1;
		last_node->properties.counter = 1;
		*it = last_node;
		it = &last_node->first_child;
	}
	
	
	// set tags and recipes 
	
	foreach (const Corpus2::Lexeme & lexeme, token.lexemes())
	{
		if (Cfg.isForbidden(lexeme))
			continue;
		
		Properties::tag_info & ti = last_node->properties.tags[lexeme.tag()];
		UnicodeString basel = lexeme.lemma();
		basel.toLower();
		
		// longest common prefix
		int i = 0;
		for (; i < std::min(orth.length(), basel.length()); i++)
			if (orth[i] != basel[i])
				break;
		
		UnicodeString suffix = basel.tempSubStringBetween(i, basel.length());
		int delendum = orth.length() - i;
		
		ti.recipes[std::make_pair(suffix, delendum)] += 1;
		ti.strength += 1;
	}
}

void Tree::insert(const Corpus2::Token &token)
{
	// Manage prefixes
	if (token.orth().startsWith("nie"))
	{
		boost::shared_ptr<Corpus2::Token> tokenptr(token.clone());
		insert_aux(*extract_negation(tokenptr));
		insert_aux(*tokenptr);
	}
	else if (token.orth().startsWith("naj"))
	{
		boost::shared_ptr<Corpus2::Token> tokenptr(token.clone());
		insert_aux(*extract_superlativum(tokenptr));
		insert_aux(*tokenptr);
	}
	else
		insert_aux(token);
}







void Tree::compressLemmawise()
{
	compressLemmawise(root);
}

bool Tree::compressLemmawise(Node &node)
{
	if (!node.first_child)
	{
		return true;
	}
	else if (!node.first_child->next_sibling)
	{
		if (compressLemmawise(*node.first_child) && node.first_child->properties.counter == node.properties.counter)
		{
			node.properties.feedOn(node.first_child->properties);
			delete node.first_child;
			node.first_child = NULL;
			return true;
		}
	}
	else for (Node * it = node.first_child; it != NULL; it = it->next_sibling)
	{
		compressLemmawise(*it);
	}
	
	return false;
}





bool Tree::compressTagwise(Node &node)
{
	Node * new_list = NULL;
	
	Node * it = node.first_child;
	while (it != NULL)
	{
		Node * tmp = it->next_sibling;
		
		if (!compressTagwise(*it))
		{
			it->next_sibling = new_list;
			new_list = it;
		}
		else
		{
			it->next_sibling = NULL;
			delete it;
		}
		
		it = tmp;
	}
	
	node.first_child = new_list;
	
	if (!node.first_child && node.properties.tags.empty())
		return true;
	else
		return false;
}





void Tree::gatherTags()
{
	gatherTags(root);
	compressTagwise(root);
}

void Tree::gatherTags(Node &node)
{
	// recurence
	
	if (!node.first_child)
		return;
	
	for (Node * it = node.first_child; it != NULL; it = it->next_sibling)
		gatherTags(*it);
	
	
	// if we have our own tags, we won't take alien ones. It makes better results.
	if (! node.properties.tags.empty())
		return;
	
	// compute common part of all children, acknowledging strengths of their tags
	typedef std::map <Corpus2::Tag, int> tag_strengths;
	
	std::set <Corpus2::Tag> interesting;
	
	tag_strengths intersection;
	int whole_strength = 0;
	
	foreach (Properties::tag_map::value_type kv, node.first_child->properties.tags)
	{
		interesting.insert(kv.first);
		intersection[kv.first] = 0;
	}
	
	for (Node * it = node.first_child; it != NULL; it = it->next_sibling)
	{
		foreach (Corpus2::Tag tag, interesting)
		{
			if (intersection.find(tag) != intersection.end())
			{
				if(it->properties.tags.find(tag) != it->properties.tags.end())
					intersection[tag] += it->properties.tags[tag].strength;
				else
					intersection.erase(tag);
			}
		}
		
		foreach (Properties::tag_map::value_type kv, it->properties.tags)
			whole_strength += kv.second.strength;
	}
	
	int common_sum = 0;
	foreach (tag_strengths::value_type kv, intersection)
		common_sum += kv.second;
	
	
	// decision
	
	if (common_sum >= 1 * whole_strength)
		for (Node * it = node.first_child; it != NULL; it = it->next_sibling)
			node.properties.feedOn(it->properties);
}




void Tree::prune()
{
	prune(root);
}

void Tree::prune(Node &node) const
{
	while (node.first_child && isBad(*node.first_child))
	{
		Node * tmp = node.first_child;
		node.first_child = node.first_child->next_sibling;
		tmp->next_sibling = NULL;
		
		collect(*tmp);
		node.properties.feedOn(tmp->properties);
		delete tmp;
	}
	
	if (! node.first_child)
		return;
	
	prune(*node.first_child);
	
	Node * prev = node.first_child;
	for (Node * it = node.first_child->next_sibling; it != NULL; it = it->next_sibling)
		if (isBad(*it))
		{
			prev->next_sibling = it->next_sibling;
			it->next_sibling = NULL;
			
			collect(*it);
			node.properties.feedOn(it->properties);
			delete it;
			it = prev;
		}
		else
		{
			prune(*it);
			prev = it;
		}
}

/// is this node worthy of death?
bool Tree::isBad(const Node &node) const
{
	//return node.properties.counter < root.properties.counter / 3000.0;
	return false;
}

/// collect all tags from the whole subtree in this single node
void Tree::collect(Node &node)
{
	for (Node * it = node.first_child; it != NULL; it = it->next_sibling)
	{
		collect(*it);
		node.properties.feedOn(it->properties);
	}
	delete node.first_child;
	node.first_child = NULL;
}









