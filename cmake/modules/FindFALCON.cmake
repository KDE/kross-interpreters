# - Try to find the FALCON library
# Once done this will define
#
#  FALCON_FOUND - system has the FALCON library
#  FALCON_INCLUDE_DIR - the FALCON include directory
#  FALCON_LIBRARIES - The libraries needed to use FALCON
#
# Copyright (c) 2007, Laurent Montel , <montel@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


if (FALCON_INCLUDE_DIR AND FALCON_LIBRARIES)
  # Already in cache, be silent
  set(FALCON_FIND_QUIETLY TRUE)
endif (FALCON_INCLUDE_DIR AND FALCON_LIBRARIES)

FIND_PATH(FALCON_INCLUDE_DIR falcon/allocator.h )

FIND_LIBRARY(FALCON_LIBRARIES NAMES falcon_engine)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FALCON DEFAULT_MSG FALCON_INCLUDE_DIR FALCON_LIBRARIES )

MARK_AS_ADVANCED(FALCON_INCLUDE_DIR FALCON_LIBRARIES)
