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

#ifndef WCRFT_LAYER_H
#define WCRFT_LAYER_H

#include <vector>

#include <boost/shared_ptr.hpp>

#include <libcorpus2/tag.h>
#include <libcorpus2/tagset.h>
#include <libwccl/ops/operator.h>
#include <libwccl/ops/tagrulesequence.h>
#include <libwccl/wcclfile.h>

#include "config.h"

namespace Wcrft {

typedef boost::shared_ptr<Wccl::FunctionalOperator> fun_op_ptr_t;
typedef std::vector<fun_op_ptr_t> fun_op_ptr_v;

/**
 * @brief Simple class containing attribute name and corresponding tags
 * and operators used while tagging.
 */
class Layer {

public:
	/// Creates Layer instance and sets instance fields.
	Layer(const std::string attr_name, const Corpus2::Tag attr_tag, const fun_op_ptr_v& attr_operators)
	: attribute_name_(attr_name), attribute_tag_(attr_tag), attribute_operators_(attr_operators)
	{}

	/// Returns this layer attribute name.
	inline std::string get_attribute_name() const
	{
		return this->attribute_name_;
	}

	/// Returns @c Corpus2::Tag corresponding to layer attribute.
	inline Corpus2::Tag get_attribute_tag() const
	{
		return this->attribute_tag_;
	}

	/// Returns Functional Operators corresponding to layer attribute.
	inline fun_op_ptr_v get_attribute_operators() const
	{
		return this->attribute_operators_;
	}

private:
	const std::string attribute_name_;
	const Corpus2::Tag attribute_tag_;
	const fun_op_ptr_v attribute_operators_;
};

typedef boost::shared_ptr<Layer> LayerPtr;

/**
 * @brief Class for storing layersfor all attributes and tag rules.
 *
 * Class for storing tag rules (@c Wccl::TagRule) used to disambiguate sentence
 * when tagging and layers for all attributes. Also responsible for reading
 * WCCL file containing rules and operators to use.
 */
class Layers {

typedef boost::shared_ptr<Wccl::WcclFile> WcclFilePtr;

public:
	/**
	 * Reads wccl file (@a config_name @c .ccl) stored in config directory and
	 * creates Layer for every attribute found in config (see
	 * <a href="http://nlp.pwr.wroc.pl/redmine/projects/wcrft/wiki/#Configurations-and-trained-models">
	 * Configuration</a>). Reads Tag Rules as well, if the file contains them.
	 */
	Layers(const WcrftConfig& tagger_conf, const Corpus2::Tagset& tagset);

	/// Returns collection of layers (one for every attribute)
	inline std::vector<LayerPtr> get_layers()
	{
		return this->layers_;
	}

	/// Returns @c WCCL::TagRule s used while disambiguating sentence.
	inline boost::shared_ptr<Wccl::TagRuleSequence> get_tag_rules()
	{
		return this->tag_rules_;
	}

private:
	WcclFilePtr parse_wccl_file_(const WcrftConfig &tagger_conf, const Corpus2::Tagset& tagset);
	fun_op_ptr_v get_wccl_operators(WcclFilePtr wccl_file, std::string attr_name);
	fun_op_ptr_v get_section_ops_(WcclFilePtr wccl_file, std::string section_name);

	std::vector<LayerPtr> layers_;
	boost::shared_ptr<Wccl::TagRuleSequence> tag_rules_;
};

}

#endif // WCRFT_LAYER_H
