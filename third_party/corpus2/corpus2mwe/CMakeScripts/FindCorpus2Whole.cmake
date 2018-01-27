IF (Corpus2Whole_INCLUDE_DIR AND Corpus2Whole_LIBRARY)
#Already in cache
   SET(Corpus2Whole_FOUND TRUE)
ELSE (Corpus2_INCLUDE_DIR AND Corpus2_LIBRARY)
   FIND_PATH(Corpus2Whole_INCLUDE_DIR libcorpus2_whole/document.h /usr/include /usr/local/include)

   FIND_LIBRARY(Corpus2Whole_LIBRARY corpus2_whole /usr/lib /usr/local/lib) 

   MARK_AS_ADVANCED(Corpus2Whole_LIBRARY)
   MARK_AS_ADVANCED(Corpus2Whole_INCLUDE_DIR)

   IF (Corpus2Whole_INCLUDE_DIR AND Corpus2Whole_LIBRARY)
      SET(Corpus2Whole_FOUND TRUE)
   ENDIF (Corpus2Whole_INCLUDE_DIR AND Corpus2Whole_LIBRARY)

ENDIF (Corpus2Whole_INCLUDE_DIR AND Corpus2Whole_LIBRARY)

IF (Corpus2Whole_FOUND)
	MESSAGE(STATUS "Found libcorpus2_whole")
ELSE (Corpus2Whole_FOUND)
	MESSAGE(FATAL_ERROR "Could not find libcorpus2_whole")
ENDIF (Corpus2Whole_FOUND)
