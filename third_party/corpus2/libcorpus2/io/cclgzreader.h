#ifndef CCLOREADER_H
#define CCLOREADER_H
#include "cclreader.h"

namespace Corpus2 {

struct CclGzReader : public CclReader
{

    CclGzReader(const Tagset& tagset, std::istream& is,
            bool disamb_only = false, bool disamb_sh = false);

    CclGzReader(const Tagset& tagset, const std::string& filename,
            bool disamb_only = false, bool disamb_sh = false);
    ~CclGzReader();

    static bool registered;
};

}

#endif // CCLOREADER_H
