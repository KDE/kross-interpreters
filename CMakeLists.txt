cmake_minimum_required(VERSION 2.6)
project(KrossPlugins)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules )

# search packages used by KDE
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DQT3_SUPPORT)
include_directories (${KDE4_INCLUDES})

find_package(PythonLibrary)
find_package(FALCON)

if(PYTHONLIBRARY_FOUND)
	macro_optional_add_subdirectory(python)
endif(PYTHONLIBRARY_FOUND)
macro_log_feature(PYTHONLIBRARY_FOUND "Python" "Python is a programming language that lets you work more quickly and integrate your systems more effectively." "http://python.org" FALSE "2.0" "Kross Python support.")

if(FALCON_FOUND)
	macro_optional_add_subdirectory(falcon)
endif(FALCON_FOUND)
macro_log_feature(FALCON_FOUND "FALCON" "Falcon is an Open Source, simple, fast and powerful programming language, easy to learn and to feel comfortable with." "http://www.falconpl.org/" FALSE "0.9.6.8" "Kross Falcon support.")

macro_optional_add_subdirectory(ruby)
macro_optional_add_subdirectory(java)
macro_display_feature_log()

