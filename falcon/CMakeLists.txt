project(krossfalcon)

include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

include_directories(
	${FALCON_INCLUDE_PATH}
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${KDE4_ENABLE_EXCEPTIONS}")

########### next target ###############

set(krossfalcon_PART_SRCS
   metatypeflex.cpp
   falconinterpreter.cpp
   falconerrhand.cpp
   falconscript.cpp
   falconkfvm.cpp
   falconmodule.cpp
   falconkerror.cpp
   falconfunction.cpp
   falconobjman.cpp
   falconkopaque.cpp
   
   falconkross.cpp
   falconkrossobject.cpp
)

kde4_add_plugin(krossfalcon ${krossfalcon_PART_SRCS})

# -ldl seems to be needed (on Linux at least).
# see http://lists.kde.org/?l=koffice-devel&m=116333261328694&w=2
# and http://websvn.kde.org/branches/work/kwin_composite/CMakeLists.txt?rev=603677&r1=603384&r2=603677
target_link_libraries(krossfalcon ${QT_QTCORE_LIBRARY} ${KDE4_KROSSCORE_LIBS} ${FALCON_LIBRARIES} ${CMAKE_DL_LIBS})

install(TARGETS krossfalcon DESTINATION ${PLUGIN_INSTALL_DIR})

