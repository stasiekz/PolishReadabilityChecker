/**
 * This module contains definitions on standard types such as 
 * list, vector, string f.e.:
 * attr_map_t is defined as std::map<std::string, std::string>
 */
#ifndef SWIG_LIBCORPUS2_STD_DEFS_I
#define SWIG_LIBCORPUS2_STD_DEFS_I

%module libdefs
%{
%}

// %include "std_set.i"
// %include "std_pair.i"
%include "std_map.i"
%include "std_string.i"
%include "std_vector.i"
%include "boost_shared_ptr.i"

%template(attr_map_t) std::map<std::string, std::string>;
%template(IntVector) std::vector<int>;
%template(StdStringVector) std::vector<std::string>;

using namespace std;

#endif /* SWIG_LIBCORPUS2_STD_DEFS_I */
