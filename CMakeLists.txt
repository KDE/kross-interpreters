cmake_minimum_required(VERSION 2.6)                                                                                                                                                 
project(KrossPlugins)                                                                                                                                                               
                                                                                                                                                                                    
                                                                                                                                                                                    
# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked                                                                                                   
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_SOURCE_DIR}/generator/cmake )                                                                  
                                                                                                                                                                                    
# search packages used by KDE                                                                                                                                                       
find_package(KDE4 REQUIRED)                                                                                                                                                         
include(KDE4Defaults)                                                                                                                                                               
include(MacroLibrary)                                                                                                                                                               
                                                                                                                                                                                    
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DQT3_SUPPORT)                                                                                                               
include_directories (${KDE4_INCLUDES})                                                                                                                                              
                                                                                                                                                                                    
find_package(PythonLibrary)                                                                                                                                          

if(PYTHONLIBRARY_FOUND)
	macro_optional_add_subdirectory(python)
endif(PYTHONLIBRARY_FOUND)
macro_log_feature(PYTHONLIBRARY_FOUND "Python" "Python is a programming language that lets you work more quickly and integrate your systems more effectively." "http://python.org" FALSE "2.0" "Kross Python support.")

macro_optional_add_subdirectory(java)
macro_optional_add_subdirectory(falcon)
macro_optional_add_subdirectory(ruby)
macro_display_feature_log()

