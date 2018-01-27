/*
	Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski, Paweł Kędzia
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

#include <boost/make_shared.hpp>
#include <libcorpus2_whole/relation.h>
#include <stdio.h>

namespace Corpus2 {
namespace whole {

Relation::Relation(const std::string& name,
				   const std::string& set,
				   const boost::shared_ptr<const DirectionPoint> from,
				   const boost::shared_ptr<const DirectionPoint> to)
	: name_(name), set_(set), from_(from), to_(to)
{
}

Relation::Relation(const std::string& name,
				   const boost::shared_ptr<const DirectionPoint> from,
				   const boost::shared_ptr<const DirectionPoint> to)
	: name_(name), from_(from), to_(to)
{
}

boost::shared_ptr<Relation> Relation::clone_shared(){
	relation_=boost::shared_ptr<Relation>(new Relation(name_, set_, from_,to_));
	return relation_;
}

void Relation::set_to(const DirectionPoint& dp){
	boost::shared_ptr<const DirectionPoint> temp(new DirectionPoint(dp.sentence_id(),dp.channel_name(),dp.annotation_number()));
	to_=temp;

}

void Relation::set_from(const DirectionPoint& dp){
	boost::shared_ptr<const DirectionPoint> temp(new DirectionPoint(dp.sentence_id(),dp.channel_name(),dp.annotation_number()));
        from_=temp;

}

void Relation::set_name(const std::string& s){
	name_=s;
}

void Relation::set_set(const std::string& s){
	set_=s;
}

Relation::Relation(const std::string& name,
				   const std::string& set,
				   const DirectionPoint& from,
				   const DirectionPoint& to)
	: name_(name),
	set_(set),
	from_(boost::make_shared<const DirectionPoint>(from)),
	to_(boost::make_shared<const DirectionPoint>(to))
{
}

Relation::Relation(const std::string& name,
				   const DirectionPoint& from,
				   const DirectionPoint& to)
	: name_(name),
	from_(boost::make_shared<const DirectionPoint>(from)),
	to_(boost::make_shared<const DirectionPoint>(to))
{
	set_ = "";
}

Relation::~Relation()
{
}

} // whole ns
} // Corpus2 ns
