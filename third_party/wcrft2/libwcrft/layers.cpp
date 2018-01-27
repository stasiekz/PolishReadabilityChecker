/* This file is part of WCRFT
  Copyright (C) 2014 Radosław Warzocha, Adam Radziszewski.
  WCRFT is free software; you can redistribute and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option)
  any later version.

  WCRFT is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.

  See the LICENCE and COPYING files for more details
 */

#include <boost/algorithm/string.hpp>

#include <libcorpus2/tagging.h>
#include <libwccl/ops/opsequence.h>
#include <libwccl/parser/Parser.h>

#include "corpusio.h"
#include "layers.h"

namespace Wcrft {

Layers::Layers(const WcrftConfig& tagger_conf, const Corpus2::Tagset &tagset)
{
	std::string attributes =
			tagger_conf.get_config_section_option<std::string>(
				CONFIG_S_GLOBAL, CONFIG_O_ATTRS);
	std::vector<std::string> attributes_names;
	attributes_names = boost::algorithm::split(attributes_names, attributes, 
											   boost::algorithm::is_any_of(","),
											   boost::algorithm::token_compress_on);

	WcclFilePtr wccl_file = this->parse_wccl_file_(tagger_conf, tagset);
	if(wccl_file->has_tag_rules())
		this->tag_rules_ = wccl_file->get_tag_rules_ptr();

	for(unsigned int i = 0;i < attributes_names.size();++i) {
		std::string attribute_name = attributes_names[i];

		Corpus2::Tag attribute_tag;
		if(attribute_name.compare(CONFIG_WORDCLASS) == 0)
			attribute_tag = Corpus2::get_attribute_mask(tagset, "");
		else
			attribute_tag = Corpus2::get_attribute_mask(tagset, attribute_name);

		fun_op_ptr_v attribute_operators = this->get_wccl_operators(wccl_file, attribute_name);

		LayerPtr layer = boost::make_shared<Layer>(attribute_name, attribute_tag, attribute_operators);
		this->layers_.push_back(layer);
	}
}

Layers::WcclFilePtr Layers::parse_wccl_file_(const WcrftConfig& tagger_conf, const Corpus2::Tagset &tagset)
{
	std::string wccl_file_path =
		tagger_conf.get_config_section_option<std::string>(CONFIG_S_GLOBAL, CONFIG_O_WCCLCFG);
	return Wccl::Parser(tagset).parseWcclFileFromPath(
				wccl_file_path, tagger_conf.get_config_dir());
}

fun_op_ptr_v Layers::get_wccl_operators(WcclFilePtr wccl_file, std::string attr_name)
{
	fun_op_ptr_v default_ops = this->get_section_ops_(wccl_file, CONFIG_DEFAULT_OPS);
	fun_op_ptr_v attribute_ops = this->get_section_ops_(wccl_file, attr_name);
	attribute_ops.insert(attribute_ops.end(), default_ops.begin(), default_ops.end());
	return attribute_ops;
}

fun_op_ptr_v Layers::get_section_ops_(WcclFilePtr wccl_file, std::string section_name)
{
	std::vector<fun_op_ptr_t> operators;

	if(wccl_file->has_untyped_section(section_name)) {
		Wccl::UntypedOpSequence section = wccl_file->get_untyped_section(section_name);

		for(unsigned int op_id = 0;op_id < section.size();++op_id) {
			fun_op_ptr_t pointer = section.get_ptr(op_id);
			if(pointer)
				operators.push_back(pointer);
		}
	}

	return operators;
}

}
