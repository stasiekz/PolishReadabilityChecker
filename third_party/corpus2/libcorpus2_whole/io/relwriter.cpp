/*
        Copyright (C) 2010 Tomasz Åniatowski, Adam Radziszewski, PaweÅ KÄdzia
        Part of the libcorpus2 project

        This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

        This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.

        See the LICENSE.CORPUS2, LICENSE.POLIQARP, COPYING.LESSER and COPYING files for more details.
*/


#include <boost/foreach.hpp>
#include <libcorpus2/exception.h>
#include <libcorpus2_whole/io/relwriter.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/make_shared.hpp>
using namespace std;

namespace Corpus2 {
namespace whole {

RelationWriter::RelationWriter(const std::string &rela_path)
{
	rela_path_=  rela_path;
}


void RelationWriter::write(const std::vector< boost::shared_ptr<Relation> >& relations)
{	
	ofstream ofs(rela_path_.c_str(), ofstream::out);
	
	//header
	ofs<<"<relations>\n";
	
	//relations
	BOOST_FOREACH(const boost::shared_ptr<Relation>& r, relations){
		ofs<<"   <rel name=\"";
		ofs<<r -> name();
		ofs<<"\" set=\"";
		ofs<<r -> set()<<"\">"<<endl;
		ofs<<"      <from sent=\"";
		ofs<<r -> from() -> sentence_id();
		ofs<<"\" chan=\"";
		ofs<<r -> from() -> channel_name();
		ofs<<"\">";
		ofs<<r -> from() -> annotation_number();
		ofs<<"</from>"<<endl;
		ofs<<"      <to sent=\"";
		ofs<<r -> to() -> sentence_id();
		ofs<<"\" chan=\"";
		ofs<<r -> to() -> channel_name();
		ofs<<"\">";
		ofs<<r -> to() -> annotation_number();
		ofs<<"</to>"<<endl;
		ofs<<"   </rel>"<<endl;
	}
        //footer
	ofs<<"</relations>";
	ofs.close();
}



} // whole ns
} // Corpus2 ns

