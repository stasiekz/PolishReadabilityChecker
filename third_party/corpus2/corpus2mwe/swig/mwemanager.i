#ifndef SWIG_CORPUS2MWE_MWEMANAGER_I
#define SWIG_CORPUS2MWE_MWEMANAGER_I

%module cclmwereader
%{
        #include <cclmwe/mwemanager.h>
%}

%include "std_string.i"

namespace Corpus2MWE {
    class MWEManager {
        public:
            MWEManager();

            const std::string get_mwes(const std::string &name);
    };

    inline const std::string get_mwe_dict(const std::string &name);
}

#endif /* SWIG_CORPUS2MWE_MWEMANAGER_I */
