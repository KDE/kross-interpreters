project(krossjava)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

# search packages used by KDE
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)
include(MacroOptionalAddSubdirectory)
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

# if(JAVA_INCLUDE_PATH AND JAVA_JVM_LIBRARY)
#     # if already defined, do nothing but try to use them
#     message(STATUS "using defined JAVA_INCLUDE_PATH and JAVA_JVM_LIBRARY")
# else(JAVA_INCLUDE_PATH AND JAVA_JVM_LIBRARY)
    # else we try to detect the jvm
    set(KrossJNI_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    macro_optional_find_package(KrossJNI)
# endif(JAVA_INCLUDE_PATH AND JAVA_JVM_LIBRARY)

#the JAVA_HOME directory
message(STATUS "  JAVA_HOME: " $ENV{JAVA_HOME})
#the path to the jawt library
message(STATUS "  JAVA_AWT_LIB_PATH: " ${JAVA_AWT_LIB_PATH})
#the path to the jvm library
message(STATUS "  JAVA_JVM_LIB_PATH: " ${JAVA_JVM_LIB_PATH})
#the include path to jni.h
message(STATUS "  JAVA_INCLUDE_PATH: " ${JAVA_INCLUDE_PATH})
#the include path to jni.h
message(STATUS "  JAVA_INCLUDE_PATH2: " ${JAVA_INCLUDE_PATH2})
#the include path to jawt.h
message(STATUS "  JAVA_AWT_INCLUDE_PATH: " ${JAVA_AWT_INCLUDE_PATH})
#the libjvm.so library
message(STATUS "  JAVA_JVM_LIBRARY: " ${JAVA_JVM_LIBRARY})

#message(STATUS "KDE4_INCLUDES: " ${KDE4_INCLUDES})

if(JAVA_INCLUDE_PATH AND JAVA_JVM_LIBRARY)
    add_subdirectory(krossjava)
endif(JAVA_INCLUDE_PATH AND JAVA_JVM_LIBRARY)

add_subdirectory(test)
