#include "cclgzwriter.h"

namespace Corpus2{

bool CclGzWriter::registered = TokenWriter::register_writer<CclGzWriter>("ccl:gz",
        "flat,chunk,nochunk,nodisamb,sorttags,split,ws");

CclGzWriter::CclGzWriter(std::ostream& os, const Tagset& tagset,
        const string_range_vector& params): CclWriter(os,tagset,params,true){

}

CclGzWriter::~CclGzWriter(){

}


}
