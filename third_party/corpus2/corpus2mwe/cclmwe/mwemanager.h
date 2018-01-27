#ifndef CORPUS2_MWE_MWEMANAGER_H
#define CORPUS2_MWE_MWEMANAGER_H

#include <loki/Singleton.h>

namespace Corpus2MWE {

class MWEManager {
public:
	MWEManager();
	
	const std::string get_mwes(const std::string &name);
private:
	std::string search_path_;
};

typedef Loki::SingletonHolder<MWEManager> MWEManagerSingl;

inline const std::string get_mwe_dict(const std::string &name) {
	return MWEManagerSingl::Instance().get_mwes(name);
}

} /* end ns Corpus2MWE */

#endif //CORPUS2_MWE_MWEMANAGER_H
