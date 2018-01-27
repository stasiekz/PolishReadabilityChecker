#ifndef CCLOWRITER_H
#define CCLOWRITER_H

#include "cclwriter.h"

namespace Corpus2{

struct CclGzWriter : public CclWriter
{
    CclGzWriter(std::ostream& os, const Tagset& tagset,
            const string_range_vector& params);
    ~CclGzWriter();
    static bool registered;
};

}

#endif // CCLOWRITER_H
