PROJECT(KROSSPYTHON)

find_package(PythonLibrary REQUIRED)
include(PythonMacros)

include_directories(
    ${PYTHON_INCLUDE_PATH}
    ${QT_INCLUDE_DIR}
    ${QT_QT_INCLUDE_DIR}
    ${QT_QTCORE_INCLUDE_DIR}
    ${QT_QTGUI_INCLUDE_DIR}
    #${QT_QTNETWORK_INCLUDE_DIR}
    #${QT_QTXML_INCLUDE_DIR}
    ${KDE4_INCLUDE_DIR}
    ${KDE4_INCLUDE_DIR}/kross
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")

########### next target ###############

add_subdirectory( cxx )

set(libkrosspythoncxx_SRCS
    ${CMAKE_SOURCE_DIR}/python/krosspython/cxx/cxxsupport.cxx
    ${CMAKE_SOURCE_DIR}/python/krosspython/cxx/cxx_extensions.cxx
    ${CMAKE_SOURCE_DIR}/python/krosspython/cxx/cxxextensions.c
    ${CMAKE_SOURCE_DIR}/python/krosspython/cxx/IndirectPythonInterface.cxx
)

########### next target ###############

set(krosspython_PART_SRCS ${libkrosspythoncxx_SRCS}
   pythonvariant.cpp
   pythonextension.cpp
   pythonmodule.cpp
   pythonscript.cpp
   pythoninterpreter.cpp
   pythonobject.cpp
)

kde4_add_plugin(krosspython ${krosspython_PART_SRCS})

# -ldl seems to be needed (on Linux at least).
# see http://lists.kde.org/?l=koffice-devel&m=116333261328694&w=2
# and http://websvn.kde.org/branches/work/kwin_composite/CMakeLists.txt?rev=603677&r1=603384&r2=603677
target_link_libraries(krosspython ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KPTY_LIBS} ${KDE4_KROSSCORE_LIBS} ${PYTHON_LIBRARIES} ${CMAKE_DL_LIBS})

install(TARGETS krosspython  DESTINATION ${PLUGIN_INSTALL_DIR})

