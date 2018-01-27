#include "cclgzreader.h"

namespace Corpus2 {


bool CclGzReader::registered = TokenReader::register_reader<CclGzReader>("ccl:gz",
    "ign,loose,strict,disamb_only,no_warn_inconsistent,no_warn_unexpected_xml,autogen_chunk_id,autogen_sent_id");

CclGzReader::CclGzReader(const Tagset& tagset, std::istream& is,
        bool disamb_only, bool disamb_sh) : CclReader(tagset,is,disamb_only,disamb_sh){

}

CclGzReader::CclGzReader(const Tagset& tagset, const std::string& filename,
        bool disamb_only, bool disamb_sh) : CclReader(tagset,filename,disamb_only,disamb_sh){

}

CclGzReader::~CclGzReader(){

}

}
