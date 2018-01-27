########## libwccl ###############

PROJECT(wccl)

configure_file(version.in include/libwccl/version.h)
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/include/ )
add_definitions(-DHAVE_VERSION_H)

find_package(Corpus2 1.0.12 REQUIRED)
include_directories(${Corpus2_INCLUDE_DIR})
set(LIBS ${LIBS} ${Corpus2_LIBRARY})

find_package(PwrUtils 1.0.1 REQUIRED)
set(LIBS ${LIBS} ${PwrUtils_LIBRARY})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
set(LIBS ${LIBS} ${Boost_LIBRARIES})

find_package(ICU REQUIRED)
include_directories(${ICU_INCLUDE_DIR})
set(LIBS ${LIBS} ${ICU_LIBRARIES} ${ICU_I18N_LIBRARIES})

find_package(ANTLR REQUIRED)
include_directories(${ANTLR_INCLUDE_DIR})

# On UNIX this should be included only in final executables
# Unfortunatelly g++ is not doing so good with ANTLR being static lib
set(LIBS ${LIBS} ${ANTLR_LIBRARY})

SET(libwccl_STAT_SRC
	exception.cpp
	lexicon/lexfilegrammar.g
	lexicon/lexicon.cpp
	lexicon/lexiconparser.cpp
	lexicon/lexicons.cpp
	ops/formatters.cpp
	ops/functions/bool/iteration.cpp
	ops/functions/bool/iterations/atleast.cpp
	ops/functions/bool/iterations/leftlook.cpp
	ops/functions/bool/iterations/only.cpp
	ops/functions/bool/iterations/rightlook.cpp
	ops/functions/bool/iterations/skip.cpp
	ops/functions/bool/predicate.cpp
	ops/functions/bool/predicates/ambiguous.cpp
	ops/functions/bool/predicates/and.cpp
	ops/functions/bool/predicates/ann.cpp
	ops/functions/bool/predicates/annsub.cpp
	ops/functions/bool/predicates/anntoken.cpp
	ops/functions/bool/predicates/debug.cpp
	ops/functions/bool/predicates/isinside.cpp
	ops/functions/bool/predicates/isoutside.cpp
	ops/functions/bool/predicates/issingular.cpp
	ops/functions/bool/predicates/logicalpredicate.cpp
	ops/functions/bool/predicates/nor.cpp
	ops/functions/bool/predicates/or.cpp
	ops/functions/bool/predicates/pointagreement.cpp
	ops/functions/bool/predicates/regex.cpp
	ops/functions/bool/predicates/strongagreement.cpp
	ops/functions/bool/predicates/weakagreement.cpp
	ops/functions/match/submatch.cpp
	ops/functions/position/firsttoken.cpp
	ops/functions/position/lasttoken.cpp
	ops/functions/position/relativeposition.cpp
	ops/functions/strset/affix.cpp
	ops/functions/strset/getlemmas.cpp
	ops/functions/strset/getorth.cpp
	ops/functions/strset/lextranslator.cpp
	ops/functions/strset/tolower.cpp
	ops/functions/strset/toupper.cpp
	ops/functions/strset/anninter.cpp
	ops/functions/strset/propval.cpp
	ops/functions/tset/agrfilter.cpp
	ops/functions/tset/catfilter.cpp
	ops/functions/tset/getsymbols.cpp
	ops/functions/tset/getsymbolsinrange.cpp
	ops/match/actions/markbase.cpp
	ops/match/actions/markmatch.cpp
	ops/match/actions/overwritematch.cpp
	ops/match/actions/unmarkmatch.cpp
	ops/match/actions/setpropmatch.cpp
	ops/match/applyoperator.cpp
	ops/match/conditions/conjconditions.cpp
	ops/match/conditions/isannotatedas.cpp
	ops/match/conditions/longest.cpp
	ops/match/conditions/matchtext.cpp
	ops/match/conditions/oneof.cpp
	ops/match/conditions/optionalmatch.cpp
	ops/match/conditions/repeatedmatch.cpp
	ops/match/conditions/tokencondition.cpp
	ops/matchrule.cpp
	ops/matchrulesequence.cpp
	ops/opsequence.cpp
	ops/tagaction.cpp
	ops/tagactions/delete.cpp
	ops/tagactions/mark.cpp
	ops/tagactions/relabel.cpp
	ops/tagactions/select.cpp
	ops/tagactions/unify.cpp
	ops/tagactions/unmark.cpp
	ops/tagrule.cpp
	ops/tagrulesequence.cpp
	parser/grammar.g
	parser/Parser.cpp
	parser/ParserException.cpp
	sentencecontext.cpp
	values/annotationmatch.cpp
	values/bool.cpp
	values/match.cpp
	values/matchvector.cpp
	values/position.cpp
	values/strset.cpp
	values/tokenmatch.cpp
	values/tset.cpp
	values/value.cpp
	variables.cpp
	wcclfile.cpp
)

SET(libwccl_STAT_SRC ${libwccl_STAT_SRC}
	${PROJECT_BINARY_DIR}/parser/ANTLRLexer.cpp
	${PROJECT_BINARY_DIR}/parser/ANTLRParser.cpp
	${PROJECT_BINARY_DIR}/lexicon/ANTLRLexiconLexer.cpp
	${PROJECT_BINARY_DIR}/lexicon/ANTLRLexiconParser.cpp
)

include_directories(${PROJECT_BINARY_DIR}/parser)

ADD_CUSTOM_COMMAND(
	OUTPUT ${PROJECT_BINARY_DIR}/parser/ANTLRLexer.cpp
		${PROJECT_BINARY_DIR}/parser/ANTLRLexer.hpp
		${PROJECT_BINARY_DIR}/parser/ANTLRParser.cpp
		${PROJECT_BINARY_DIR}/parser/ANTLRParser.hpp
		${PROJECT_BINARY_DIR}/parser/ANTLRExprTokenTypes.hpp
		${PROJECT_BINARY_DIR}/parser/ANTLRExprTokenTypes.txt
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${PROJECT_BINARY_DIR}/parser
	COMMAND runantlr -o parser ${PROJECT_SOURCE_DIR}/parser/grammar.g
	DEPENDS ${PROJECT_SOURCE_DIR}/parser/grammar.g
	COMMENT "-- Generating ANTLR parser cpp/hpp/txt files for WCCL ---")

include_directories(${PROJECT_BINARY_DIR}/lexicon)

ADD_CUSTOM_COMMAND(
	OUTPUT
		${PROJECT_BINARY_DIR}/lexicon/ANTLRLexiconLexer.cpp
		${PROJECT_BINARY_DIR}/lexicon/ANTLRLexiconLexer.hpp
		${PROJECT_BINARY_DIR}/lexicon/ANTLRLexiconParser.cpp
		${PROJECT_BINARY_DIR}/lexicon/ANTLRLexiconParser.hpp
		${PROJECT_BINARY_DIR}/lexicon/ANTLRLexiconsTokenTypes.hpp
		${PROJECT_BINARY_DIR}/lexicon/ANTLRLexiconsTokenTypes.txt
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${PROJECT_BINARY_DIR}/lexicon
	COMMAND runantlr -o lexicon ${PROJECT_SOURCE_DIR}/lexicon/lexfilegrammar.g
	DEPENDS ${PROJECT_SOURCE_DIR}/lexicon/lexfilegrammar.g
	COMMENT "-- Generating ANTLR parser cpp/hpp/txt files for lexicon files ---")
	
	
file(GLOB_RECURSE INCS "*.h")

if(WIN32)
#need to explicitly specify exports/external API for win DLL
#until that's done, build STATIC for WIN32
	add_library(wccl SHARED ${libwccl_STAT_SRC} ${INCS})
else(WIN32)
	add_library(wccl SHARED ${libwccl_STAT_SRC} ${INCS})
endif(WIN32)

target_link_libraries ( wccl ${LIBS})

set_target_properties(wccl PROPERTIES
	VERSION "${wccl_ver_major}.${wccl_ver_minor}"
	SOVERSION ${wccl_ver_major})


	install(TARGETS wccl
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION bin
	)
	install(
		DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		DESTINATION include
		FILES_MATCHING PATTERN "*.h"
		PATTERN ".svn" EXCLUDE
		PATTERN "bin" EXCLUDE
		PATTERN "build" EXCLUDE
		PATTERN "CMake*" EXCLUDE
	)
