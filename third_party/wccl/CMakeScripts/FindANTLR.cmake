# Finds the ANother Tool for Language Recognition (ANTLR) Library
# 
# ANTLR_FOUND - True if ANTLR has been found.
#
# ANTLR_INCLUDE_DIR - Directory to include to get ANTLR headers
#
# ANTLR_LIBRARY - libraries to link

if (ANTLR_INCLUDE_DIR AND ANTLR_LIBRARY)
# in cache already  
  set(ANTLR_FOUND TRUE)
else (ANTLR_INCLUDE_DIR AND ANTLR_LIBRARY)

# Look for the header file.
  find_path(
    ANTLR_INCLUDE_DIR
    NAMES antlr/ANTLRUtil.hpp
    DOC "Include directory for the ANTLR library")
  mark_as_advanced(ANTLR_INCLUDE_DIR)
 
# Look for the library.
  find_library(
    ANTLR_LIBRARY
    NAMES antlr-pic
    DOC "Library to link against for ANTLR")
  mark_as_advanced(ANTLR_LIBRARY)

  if (ANTLR_INCLUDE_DIR AND ANTLR_LIBRARY)
     set(ANTLR_FOUND TRUE)
  endif (ANTLR_INCLUDE_DIR AND ANTLR_LIBRARY)
endif (ANTLR_INCLUDE_DIR AND ANTLR_LIBRARY)

if (ANTLR_FOUND)
  if( NOT ANTLR_FIND_QUIETLY )
    message( STATUS "Found ANTLR header files in ${ANTLR_INCLUDE_DIR}")
    message( STATUS "Found ANTLR library: ${ANTLR_LIBRARY}")
  endif ( NOT ANTLR_FIND_QUIETLY )
else (ANTLR_FOUND)
  if (ANTLR_FIND_REQUIRED)
    message( FATAL_ERROR "Could not find ANTLR" )
  else (ANTLR_FIND_REQUIRED)
    message( STATUS "Optional package ANTLR was not found" )
  endif (ANTLR_FIND_REQUIRED)
endif (ANTLR_FOUND)
