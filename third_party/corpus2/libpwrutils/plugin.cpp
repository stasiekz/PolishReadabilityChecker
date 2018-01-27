/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENCE, COPYING.LESSER and COPYING files for more details.
*/
#include <libpwrutils/plugin.h>
#ifdef __unix__
#include <dlfcn.h>
#endif
#include <iostream>

namespace PwrNlp {
namespace Plugin {

std::string make_soname(const std::string &scope, const std::string &name)
{
	if (name.size() > 1 && name.find('/') != name.npos) {
		return name;
	} else {
		return "lib" + scope + "_" + name + ".so";
	}
}

bool load(const std::string &scope, const std::string &name, bool quiet)
{
#ifdef __unix__
	std::string soname = make_soname(scope, name);
	// std::cerr << "PLUGIN LOAD " << scope << " " << name << " " << soname << "\n";
	// first check if the plugin was already loaded
	void* handle = dlopen(soname.c_str(), RTLD_NOW | RTLD_NOLOAD);
	if (handle != NULL) {
		if (!quiet) {
			std::cerr << "Warning: " << scope << " plugin '" << name
				<< "'' already loaded\n";
		}
		return false;
	}
	// actually load the library
	dlerror();
	handle = dlopen(soname.c_str(), RTLD_NOW);
	if (handle == NULL) {
		if (!quiet) {
			const char* dle = dlerror();
			std::cerr << "Error: dlopen error while loading " << scope
				<< " plugin '" << name << "' (" << soname << "): ";
			if (dle != NULL) {
				std::cerr << dle << "\n";
			}
		}
		return false;
	}
	// run plugin init function if it exists
	typedef void (*init_func_t)();
	init_func_t init_func = reinterpret_cast<init_func_t>(
			dlsym(handle, "pwrnlp_plugin_init"));
	if (init_func) {
		init_func();
	}
	if (!quiet) {
		std::cerr << "Loaded " << scope << " plugin '" << name << "'\n";
	}
#endif
	return true;
}

bool load_check(const std::string &scope, const std::string &name, bool quiet,
		boost::function<size_t (void)> counter, const std::string &what)
{
#ifdef __unix__
	size_t before = counter();
	if (load(scope, name, quiet)) {
		size_t after = counter();
		if (after <= before) {
			if (!quiet) {
				std::cerr << "Warning: " << scope << " plugin '"
					<< name << "'' loaded, but"
					<< what << " count did not increase\n";
			}
			return false;
		}
		return true;
	} else {
		return false;
	}
#else
	return true;
#endif
}

} /* end ns Plugin */
} /* end ns PwrNlp */
