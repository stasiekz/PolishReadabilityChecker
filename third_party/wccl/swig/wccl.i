#ifndef SWIG_WCCL_I
#define SWIG_WCCL_I

%module wccl
%{
  //
%}
%include <libcorpus2/unicodestring.i>
%include "actionexeccontext.i"
%include "annotationmatch.i"
%include "bool.i"
%include "expression.i"
%include "functionaloperator.i"
%include "matchdata.i"
%include "match.i"
%include "matchrule.i"
%include "matchvector.i"
%include "operator.i"
%include "parsedexpression.i"
%include "parser.i"
%include "position.i"
%include "sentencecontext.i"
%include "strset.i"
%include "tagaction.i"
%include "tagrule.i"
%include "tagrulesequence.i"
%include "tokenmatch.i"
%include "tset.i"
%include "value.i"
%include "variables.i"
%include "functionalopsequence.i"
%include "wcclfileopsections.i"
%include "matchrulesequence.i"
%include "lexicon.i"
%include "lexicons.i"
%include "wcclfile.i"
%include "exception.i"

#endif /* SWIG_LIBWCCL_I */
