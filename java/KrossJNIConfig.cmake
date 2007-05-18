# - Find JNI java libraries.
# This module finds if Java is installed and determines where the
# include files and libraries are. It also determines what the name of
# the library is. This code sets the following variables:
#  JAVA_AWT_LIB_PATH     = the path to the jawt library
#  JAVA_JVM_LIB_PATH     = the path to the jvm library
#  JAVA_INCLUDE_PATH     = the include path to jni.h
#  JAVA_INCLUDE_PATH2    = the include path to jni.h
#  JAVA_AWT_INCLUDE_PATH = the include path to jawt.h

# first we try to use the FindJNI.cmake shipped with cmake
macro_optional_find_package(JNI)
if(JAVA_INCLUDE_PATH AND JAVA_JVM_LIBRARY)
    # seems FindJNI.cmake did detect it successful
    message(STATUS "using FindJNI.cmake")
else(JAVA_INCLUDE_PATH AND JAVA_JVM_LIBRARY)
    # FindJNI.cmake failed to detect our jvm, so we do try some custom stuff now
    message(STATUS "using FindKrossJNI.cmake")

    SET(JAVA_AWT_LIBRARY_DIRECTORIES
        /usr/lib/gcj-4.1/
    )

    SET(JAVA_JVM_LIBRARY_DIRECTORIES)
    FOREACH(dir ${JAVA_AWT_LIBRARY_DIRECTORIES})
        SET(JAVA_JVM_LIBRARY_DIRECTORIES
            ${JAVA_JVM_LIBRARY_DIRECTORIES}
            "${dir}"
            "${dir}/client"
            "${dir}/server"
            )
    ENDFOREACH(dir)

    SET(JAVA_AWT_INCLUDE_DIRECTORIES
        /usr/lib/gcc/i486-linux-gnu/4.1.2/include
    )

    FOREACH(JAVA_PROG "${JAVA_RUNTIME}" "${JAVA_COMPILE}" "${JAVA_ARCHIVE}")
        GET_FILENAME_COMPONENT(jpath "${JAVA_PROG}" PATH)
        FOREACH(JAVA_INC_PATH ../include ../java/include ../share/java/include)
            IF(EXISTS ${jpath}/${JAVA_INC_PATH})
                SET(JAVA_AWT_INCLUDE_DIRECTORIES ${JAVA_AWT_INCLUDE_DIRECTORIES} "${jpath}/${JAVA_INC_PATH}")
            ENDIF(EXISTS ${jpath}/${JAVA_INC_PATH})
        ENDFOREACH(JAVA_INC_PATH)
        FOREACH(JAVA_LIB_PATH 
            ../lib ../jre/lib ../jre/lib/i386 
            ../java/lib ../java/jre/lib ../java/jre/lib/i386 
            ../share/java/lib ../share/java/jre/lib ../share/java/jre/lib/i386)
            IF(EXISTS ${jpath}/${JAVA_LIB_PATH})
                SET(JAVA_AWT_LIBRARY_DIRECTORIES ${JAVA_AWT_LIBRARY_DIRECTORIES} "${jpath}/${JAVA_LIB_PATH}")
            ENDIF(EXISTS ${jpath}/${JAVA_LIB_PATH})
        ENDFOREACH(JAVA_LIB_PATH)
    ENDFOREACH(JAVA_PROG)

    FIND_LIBRARY(JAVA_AWT_LIBRARY jawt
        PATHS ${JAVA_AWT_LIBRARY_DIRECTORIES}
    )
    FIND_LIBRARY(JAVA_JVM_LIBRARY NAMES jvm JavaVM
        PATHS ${JAVA_JVM_LIBRARY_DIRECTORIES}
    )

    # add in the include path
    FIND_PATH(JAVA_INCLUDE_PATH jni.h
        ${JAVA_AWT_INCLUDE_DIRECTORIES}
    )
    FIND_PATH(JAVA_INCLUDE_PATH2 jni_md.h
        ${JAVA_AWT_INCLUDE_DIRECTORIES}
        ${JAVA_INCLUDE_PATH}/win32
        ${JAVA_INCLUDE_PATH}/linux
    )

    FIND_PATH(JAVA_AWT_INCLUDE_PATH jawt.h
        ${JAVA_AWT_INCLUDE_DIRECTORIES} ${JAVA_INCLUDE_PATH}
    )

    MARK_AS_ADVANCED(
        JAVA_AWT_LIBRARY
        JAVA_JVM_LIBRARY
        JAVA_AWT_INCLUDE_PATH
        JAVA_INCLUDE_PATH
        JAVA_INCLUDE_PATH2
    )

endif(JAVA_INCLUDE_PATH AND JAVA_JVM_LIBRARY)
