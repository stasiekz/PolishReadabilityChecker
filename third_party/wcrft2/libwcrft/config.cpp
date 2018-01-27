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

#include "config.h"
#include "pathsearcher.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>

namespace Wcrft {

WcrftConfig::WcrftConfig(const std::string &config_name,
						 const std::string &model_dir) {
	WcrftPathSearcher& path_searcher = WcrftPathSearcherSingleton::Instance();

	std::string config_ini_path(config_name);
	const static std::string INI_SUFFIX("." + CONFIG_EXT_INI);
	if(!boost::iends_with(config_name, INI_SUFFIX)) {
		config_ini_path += INI_SUFFIX;
	}

	// find full path to directory containing the INI file
	boost::filesystem::path ini_path (path_searcher.find_file_or_throw(
										config_ini_path, "tagger config (INI)"));
	config_ini_path = ini_path.string();

	this->config_dir_ = ini_path.parent_path().string();
	this->config_name_ = ini_path.filename().stem().string();

	this->config_ = read_config_file(config_ini_path);

	std::string model_path;
	if(model_dir.empty()) {
		model_path = get_config_section_option<std::string>(
					CONFIG_S_GLOBAL, CONFIG_O_MODELNAME, std::string("."));
	}
	else
		model_path = model_dir;
	model_path = path_searcher.find_dir_or_throw(model_path, "model dir");
	this->model_dir_ = model_path;

	std::string wccl_cfg = get_config_section_option<std::string>(
								CONFIG_S_GLOBAL, CONFIG_O_WCCLCFG, "");
	if(wccl_cfg.empty())
		wccl_cfg = this->get_config_filename(CONFIG_EXT_WCCL);
	else
		wccl_cfg = this->get_filename(config_dir_, wccl_cfg, CONFIG_EXT_WCCL, "");
	this->set_config_section_option(CONFIG_S_GLOBAL, CONFIG_O_WCCLCFG, wccl_cfg);
}

const std::string WcrftConfig::get_filename(
		const std::string& dir,
		const std::string& base_name,
		const std::string& extension,
		const std::string& suffix) const
{
	std::string base =
			base_name + (suffix.empty() ? "" : "-" + suffix) + "." + extension;
	boost::filesystem::path wanted_path(dir);
	wanted_path /= base;
	return wanted_path.native();
}

}
