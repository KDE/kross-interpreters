project(krossjava)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

# search packages used by KDE
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)
include(MacroOptionalAddSubdirectory)
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

macro_optional_find_package(JNI)

#the path to the jawt library
message(STATUS "JAVA_AWT_LIB_PATH: " ${JAVA_AWT_LIB_PATH})
#the path to the jvm library
message(STATUS "JAVA_JVM_LIB_PATH: " ${JAVA_JVM_LIB_PATH})
#the include path to jni.h
message(STATUS "JAVA_INCLUDE_PATH: " ${JAVA_INCLUDE_PATH})
#the include path to jni.h
message(STATUS "JAVA_INCLUDE_PATH2: " ${JAVA_INCLUDE_PATH2})
#the include path to jawt.h
message(STATUS "JAVA_AWT_INCLUDE_PATH: " ${JAVA_AWT_INCLUDE_PATH})

message(STATUS "JAVA_JVM_LIBRARY: " ${JAVA_JVM_LIBRARY})

if(JAVA_INCLUDE_PATH AND JAVA_JVM_LIBRARY)
    add_subdirectory(krossjava)
endif(JAVA_INCLUDE_PATH AND JAVA_JVM_LIBRARY)
