#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <sstream>

class Compressor
{
public:
    Compressor();
    virtual void compress(std::stringstream &ssStream, std::ostream& os) = 0;
    virtual void decompress(std::string &s, std::istream*  & is) = 0;
    virtual void finish_compression(std::ostream &os){
        os << "";
    }
    virtual ~Compressor(){

    }
};

#endif // COMPRESSOR_H
