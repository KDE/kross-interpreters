cmake_minimum_required(VERSION 2.8.12)
project(KrossPlugins)

# Include KF5 cmake modules
find_package(ECM 0.0.10 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)
include(FeatureSummary)
include(ECMOptionalAddSubdirectory)

# Where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)

find_package(Qt5 5.2.0 CONFIG REQUIRED Core Gui Widgets Test)
find_package(KF5 REQUIRED Kross KDELibs4Support)

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

if(PYTHONLIBRARY_FOUND)
    ecm_optional_add_subdirectory(python)
endif(PYTHONLIBRARY_FOUND)

set_package_properties(PythonLibrary PROPERTIES
    DESCRIPTION "Python is a programming language that lets you work more quickly and integrate your systems more effectively."
    URL         "http://python.org"
    TYPE        OPTIONAL
    PURPOSE     "Kross Python support.")

### Falcon support disabled because it does not work with most modern falcon-0.9.x
# find_package(FALCON)
# if(FALCON_FOUND)
#     ecm_optional_add_subdirectory(falcon)
# endif(FALCON_FOUND)
#
# set_package_properties(Falcon PROPERTIES
#     DESCRIPTION "Falcon is an Open Source, simple, fast and powerful programming language, easy to learn and to feel comfortable with."
#     URL         "http://www.falconpl.org/"
#     TYPE        OPTIONAL
#     PURPOSE     "Kross Falcon support.")

# ecm_optional_add_subdirectory(ruby)
# ecm_optional_add_subdirectory(java)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
