project (krossfal_hello)
 
find_package(KDE4 REQUIRED)
include_directories( ${KDE4_INCLUDES} )
 
set(krossfal_hello_SRCS main.cpp mainwindow.cpp)
 
kde4_add_executable(krossfal_hello ${krossfal_hello_SRCS})
target_link_libraries(krossfal_hello ${KDE4_KDEUI_LIBS} ${KDE4_KROSSUI_LIBS})
