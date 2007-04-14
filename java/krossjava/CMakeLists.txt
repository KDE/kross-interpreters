#include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${KDE4_ENABLE_EXCEPTIONS}")
#add_subdirectory( cxx )

########### next target ###############

set(krossjava_PART_SRCS ${libkrossjavacxx_SRCS} jvmscript.cpp jvminterpreter.cpp)
kde4_automoc(${krossjava_PART_SRCS})
kde4_add_plugin(krossjava ${krossjava_PART_SRCS})

# -ldl seems to be needed (on Linux at least).
# see http://lists.kde.org/?l=koffice-devel&m=116333261328694&w=2
# and http://websvn.kde.org/branches/work/kwin_composite/CMakeLists.txt?rev=603677&r1=603384&r2=603677
#target_link_libraries(krossjava ${KDE4_KROSSCORE_LIBS} ${PYTHON_LIBRARIES} ${CMAKE_DL_LIBS})
target_link_libraries(krossjava ${KDE4_KROSSCORE_LIBS} ${} )

#libjava libjavaplugin_jni
#target_link_libraries(krossjava ${KDE4_KROSSCORE_LIBS} java)
#target_link_libraries(krossjava ${KDE4_KROSSCORE_LIBS} javaplugin_jni)
target_link_libraries(krossjava ${KDE4_KROSSCORE_LIBS} ${JAVA_JVM_LIBRARY})

install(TARGETS krossjava  DESTINATION ${PLUGIN_INSTALL_DIR})
