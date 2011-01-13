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
                                                                                                                                                                                    
macro_optional_find_package(PythonLibrary)                                                                                                                                          
OPTION(ENABLE_KROSSPYTHON "build Kross Python scripting library" ON)
#OPTION(ENABLE_KROSSPYTHON "build Kross Python scripting library" OFF)

set(KROSSPYTHON_ENABLED "no")
if(ENABLE_KROSSPYTHON)
    if(PYTHON_LIBRARIES AND PYTHON_INCLUDE_PATH)
        #EXEC_PROGRAM(python ARGS -c "import distutils.sysconfig;print distutils.sysconfig.get_config_var('VERSION')" OUTPUT_VARIABLE PythonVersionOutVar RETURN_VALUE PythonVersionRetVal)
        add_subdirectory(python)
        set(KROSSPYTHON_ENABLED "yes")
    endif(PYTHON_LIBRARIES AND PYTHON_INCLUDE_PATH)
    message(STATUS "Python Libraries: " ${PYTHON_LIBRARIES})
    message(STATUS "Python Include Path: " ${PYTHON_INCLUDE_PATH})
endif(ENABLE_KROSSPYTHON)
message(STATUS "Build Kross Python... " ${KROSSPYTHON_ENABLED})

macro_optional_add_subdirectory(java)
macro_optional_add_subdirectory(falcon)
macro_optional_add_subdirectory(ruby)
macro_display_feature_log()

