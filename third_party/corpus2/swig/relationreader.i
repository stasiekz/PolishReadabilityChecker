#ifndef SWIG_LIBCORPUS2_RELATIONREADER_I
#define SWIG_LIBCORPUS2_RELATIONREADER_I

%module libcorpusrelationreader
%{
  #include <libcorpus2_whole/io/relreader.h>
%}

%constant std::string RELATION_TAG = Corpus2::whole::RELATION_TAG;
%constant std::string RELATIONS_TAG = Corpus2::whole::RELATIONS_TAG;
%constant std::string RELATION_DIRECT_FROM = Corpus2::whole::RELATION_DIRECT_FROM;
%constant std::string RELATION_DIRECT_TO = Corpus2::whole::RELATION_DIRECT_TO;

%constant std::string RELATION_NAME = Corpus2::whole::RELATION_NAME;
%constant std::string RELATION_SET = Corpus2::whole::RELATION_SET;
%constant std::string RELATION_SENTENCE_ID = Corpus2::whole::RELATION_SENTENCE_ID;
%constant std::string RELATION_CHANNEL_NAME = Corpus2::whole::RELATION_CHANNEL_NAME;

namespace Corpus2 {
namespace whole {
  class RelationReader {
  public:
    RelationReader(const std::string &rela_path);
    const std::vector< boost::shared_ptr<Relation> >& relations();
  };
} // whole ns
} // Corpus2 ns

using namespace std;
using namespace Corpus2;
using namespace Corpus2::whole;

#endif /* SWIG_LIBCORPUS2_RELATIONREADER_I */
