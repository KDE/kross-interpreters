include_directories(${CMAKE_SOURCE_DIR} ${KDE4_KDECORE_INCLUDES} ${KDE4_KDEUI_INCLUDES} ${KDE4_KIO_INCLUDES})
set(krosstest_SRCS testobject.cpp testwindow.cpp main.cpp)
kde4_add_executable(krosstest ${krosstest_SRCS})
target_link_libraries(krosstest ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} krosscore)
