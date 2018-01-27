FIND_PATH(WCCL_INCLUDE_DIR libwccl/wcclfile.h /usr/include /usr/local/include )

FIND_LIBRARY(WCCL_LIBRARY NAMES wccl PATH /usr/lib /usr/local/lib) 

MARK_AS_ADVANCED(WCCL_LIBRARY)
MARK_AS_ADVANCED(WCCL_INCLUDE_DIR)


IF (WCCL_INCLUDE_DIR AND WCCL_LIBRARY)
   SET(WCCL_FOUND TRUE)
ENDIF (WCCL_INCLUDE_DIR AND WCCL_LIBRARY)


IF (NOT WCCL_FOUND)
   IF (WCCL_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find libwccl")
   ELSE (WCCL_FIND_REQUIRED)
      MESSAGE(STATUS "libwccl not found")
   ENDIF (WCCL_FIND_REQUIRED)
ENDIF (NOT WCCL_FOUND)