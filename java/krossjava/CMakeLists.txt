include_directories(${KDE4_KDECORE_INCLUDES} ${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${KDE4_ENABLE_EXCEPTIONS}")

########### next target ###############

set(libkrossjava_PART_SRCS jvmvariant.cpp jvmextension.cpp jvmscript.cpp jvminterpreter.cpp jvmclasswriter.cpp)
kde4_add_plugin(libkrossjava ${libkrossjava_PART_SRCS})

#target_link_libraries(libkrossjava ${KDE4_KROSSCORE_LIBS} ${CMAKE_DL_LIBS})

#libjava libjavaplugin_jni
#target_link_libraries(libkrossjava ${KDE4_KROSSCORE_LIBS} java)
#target_link_libraries(libkrossjava ${KDE4_KROSSCORE_LIBS} javaplugin_jni)
target_link_libraries(libkrossjava ${QT_QTCORE_LIBRARY} ${KDE4_KDECORE_LIBS} ${KDE4_KROSSCORE_LIBS} ${JAVA_JVM_LIBRARY})

install(TARGETS libkrossjava DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kross.jar DESTINATION ${PLUGIN_INSTALL_DIR}/kross)
