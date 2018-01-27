#include "mwemanager.h"
#include <libpwrutils/exception.h>
#include <cclmwe/mwemanager.h>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace Corpus2MWE {

MWEManager::MWEManager() {
#ifdef CORPUS2MWE_DATA_DIR
	search_path_ = CORPUS2MWE_DATA_DIR;
#else
	search_path_ = "./";
#endif
}

const std::string MWEManager::get_mwes(const std::string &name) {
	boost::filesystem::path p(name + ".xml");
	boost::filesystem::path dict_path = search_path_ / p;
	if (boost::filesystem::exists(dict_path) &&
			!boost::filesystem::is_directory(dict_path)) {
		return dict_path.string();
	}
	else {
		throw std::runtime_error("Default MWE operators were not found!");
	}
}

} /* end ns Corpus2MWE */
