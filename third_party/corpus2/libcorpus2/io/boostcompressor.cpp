#include "boostcompressor.h"
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
BoostCompressor::BoostCompressor()
{

}


void BoostCompressor::compress(std::stringstream &ssStream, std::ostream& os){
    std::stringstream compressed;
    boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
    out.push(boost::iostreams::gzip_compressor(boost::iostreams::gzip_params(9)));
    out.push(ssStream);
    boost::iostreams::copy(out,compressed);
    os << compressed.str();
}

void BoostCompressor::decompress(std::string &input_string, std::istream*& is){
    std::stringstream decompressed, sstream;
    sstream << input_string;
    boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    in.push(boost::iostreams::gzip_decompressor());
    in.push(sstream);
    boost::iostreams::copy(in,decompressed);
    is = new std::istringstream(decompressed.str());
}

