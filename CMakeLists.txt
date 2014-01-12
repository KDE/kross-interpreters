cmake_minimum_required(VERSION 2.6)
project(KrossPlugins)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules )

# search packages used by KDE
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories (${KDE4_INCLUDES})

# TODO: PYTHON3 Port: Use "find_package(PythonLibs 2 REQUIRED)" (CMAKE MODULE not KDELIBS)
find_package(PythonLibrary 2 REQUIRED QUIET)
if(PYTHONLIBRARY_FOUND)
    if(PYTHON_VERSION_MAJOR EQUAL 3)
        message(STATUS "Found Python: ${PYTHON_EXECUTABLE} (found version \"${PYTHON_VERSION_STRING}\") but only 2.x supported")
        set(PYTHONINTERP_FOUND False)
        set(PYTHONLIBRARY_FOUND False)
    else()
        message(STATUS "Found Python: ${PYTHON_EXECUTABLE} (found version \"${PYTHON_VERSION_STRING}\")")
    endif()
endif()


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

