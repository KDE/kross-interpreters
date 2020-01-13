project(KROSSPYTHON)

include_directories(${PYTHON_INCLUDE_DIRS})

kde_enable_exceptions()

add_definitions(-DPYCXX_PYTHON_2TO3)

find_package(PyCXX REQUIRED)

########### next target ###############

set(krosspython_PART_SRCS ${PYCXX_SOURCES} #${libkrosspythoncxx_SRCS}
   pythonvariant.cpp
   pythonextension.cpp
   pythonmodule.cpp
   pythonscript.cpp
   pythoninterpreter.cpp
   pythonobject.cpp
)

add_library(krosspython MODULE ${krosspython_PART_SRCS})
# the C sources use C99 features
set_property(TARGET krosspython PROPERTY C_STANDARD 11)

# -ldl seems to be needed (on Linux at least).
# see http://lists.kde.org/?l=koffice-devel&m=116333261328694&w=2
# and http://websvn.kde.org/branches/work/kwin_composite/CMakeLists.txt?rev=603677&r1=603384&r2=603677
target_link_libraries(krosspython
    KF5::KrossCore
    Qt5::Gui
    Qt5::Widgets
    ${PYTHON_LIBRARIES}
    ${CMAKE_DL_LIBS}
)

install(TARGETS krosspython DESTINATION ${KDE_INSTALL_PLUGINDIR})
