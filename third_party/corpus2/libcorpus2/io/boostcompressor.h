#ifndef BOOSTCOMPRESSOR_H
#define BOOSTCOMPRESSOR_H

#include "compressor.h"

class BoostCompressor : public Compressor
{
public:
    void compress(std::stringstream &ssStream, std::ostream& os);
    void decompress(std::string &s, std::istream*& is);
    BoostCompressor();
};

#endif // BOOSTCOMPRESSOR_H
