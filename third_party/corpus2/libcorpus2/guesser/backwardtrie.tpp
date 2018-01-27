#include <unicode/ustream.h>
#include <fstream>
#include <iostream>


namespace Corpus2 {


template <typename PropType>
BackwardTrie<PropType>::Node::Node(UChar character) :
    character(character)
{
	first_child = NULL;
	next_sibling = NULL;
}

template <typename PropType>
BackwardTrie<PropType>::Node::~Node()
{
	delete next_sibling;
	delete first_child;
}

template <typename PropType>
BackwardTrie<PropType>::BackwardTrie()
{
	
}




template <typename PropType>
typename BackwardTrie<PropType>::Node & BackwardTrie<PropType>::find_aux(BackwardTrie<PropType>::Node & node,
                                                 const UnicodeString &word,
                                                 boost::function<void(PropType &)> f, int i)
{
	if (f)
		f(node.properties);
	
	if (i < 0)
		return node;
	
	for (BackwardTrie<PropType>::Node * it = node.first_child; it != NULL; it = it->next_sibling)
		if (it->character == word[i])
			return find_aux(*it, word, f, i-1);
	return node;
}

template <typename PropType>
typename BackwardTrie<PropType>::Node & BackwardTrie<PropType>::find(const UnicodeString &word, boost::function<void(PropType &)> f)
{
	return find_aux(root, word, f, word.length()-1);
}

template <typename PropType>
const typename BackwardTrie<PropType>::Node & BackwardTrie<PropType>::find_aux(const BackwardTrie<PropType>::Node & node,
                                                 const UnicodeString &word,
                                                 boost::function<void(const PropType &)> f, int i) const
{
	if (f)
		f(node.properties);
	
	if (i < 0)
		return node;
	
	for (BackwardTrie<PropType>::Node * it = node.first_child; it != NULL; it = it->next_sibling)
		if (it->character == word[i])
			return find_aux(*it, word, f, i-1);
	return node;
}

template <typename PropType>
const typename BackwardTrie<PropType>::Node & BackwardTrie<PropType>::find(const UnicodeString &word, boost::function<void(const PropType &)> f) const
{
	return find_aux(root, word, f, word.length()-1);
}





template <typename PropType>
void BackwardTrie<PropType>::print() const
{
	root.print();
}

template <typename PropType>
void BackwardTrie<PropType>::Node::print(int d) const
{
	for (int i = 0; i < d; i++)
		std::cout << "  ";
	
	std::cout << UnicodeString(character) << " (";
	properties.print(std::cout);
	std::cout << ")" << std::endl;
	
	for (Node * it = first_child; it != NULL; it = it->next_sibling)
		it->print(d+1);
}



template <typename PropType>
void BackwardTrie<PropType>::write(const boost::filesystem::path & path) const
{
	std::ofstream file(path.c_str(), std::ios::binary);
	root.write(file);
}

template <typename T>
static void bin_dump(std::ostream &stream, const T & dumpendum)
{
	stream.write((char*)&dumpendum, sizeof(T));
}

template <typename PropType>
void BackwardTrie<PropType>::Node::write(std::ostream & stream) const
{
	bin_dump(stream, character);
	
	int cc = childrenCount();
	bin_dump(stream, cc);
	
	properties.dump(stream);
	
	for (typename Corpus2::BackwardTrie<PropType>::Node * it = first_child; it != NULL; it = it->next_sibling)
		it->write(stream);
	
	/*
	stream << character << "\t";
	properties.dump(stream);
	stream << '\t' << childrenCount() << "\n";
	for (typename Corpus2::BackwardTrie<PropType>::Node * it = first_child; it != NULL; it = it->next_sibling)
		it->write(stream);
	*/
}


namespace detail
{
	template <typename T>
	void bin_dedump(std::istream &stream, T & dedumpendum)
	{
		stream.read((char*)&dedumpendum, sizeof(T));
	}
};

template <typename PropType>
void BackwardTrie<PropType>::read(const boost::filesystem::path & path)
{
	std::ifstream file(path.c_str(), std::ios::binary);
	root.read(file);
}

template <typename PropType>
void BackwardTrie<PropType>::Node::read(std::istream & stream)
{
	detail::bin_dedump(stream, character);
	
	int cc;
	detail::bin_dedump(stream, cc);
	
	properties.dedump(stream);
	
	Node ** it = &first_child;
	for (int i = 0; i < cc; i++)
	{
		*it = new Node();
		(*it)->read(stream);
		it = &((*it)->next_sibling);
	}
}




template <typename PropType>
int BackwardTrie<PropType>::Node::childrenCount() const
{
	Node * it = first_child;
	
	if (!it)
		return 0;
	
	int i = 1;
	
	while (it->next_sibling)
		it = it->next_sibling, ++i;
	return i;
}

template <typename PropType>
typename BackwardTrie<PropType>::Node * BackwardTrie<PropType>::Node::lastChild() const
{
	Node * it = first_child;
	
	if (!it)
		return it;
	
	while (it->next_sibling)
		it = it->next_sibling;
	return it;
}


} /* end ns Corpus2 */














