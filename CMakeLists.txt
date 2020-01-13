cmake_minimum_required(VERSION 3.0)
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

find_package(Qt5 5.2.0 CONFIG REQUIRED Core Gui Widgets)
find_package(KF5 5.11.0 REQUIRED Kross)

# ========================= Find Python 3 =========================
find_package(PythonLibs 3 EXACT)

if(PYTHONLIBS_FOUND)
    message(STATUS "Found Python 3: ${PYTHON_LIBRARIES} (found version \"${PYTHONLIBS_VERSION_STRING}\")")
endif()

# ========================= Find FALCON =========================

### Falcon support disabled because it does not work with most modern falcon-0.9.x
# find_package(FALCON)

# ========================= Log features =========================

# TODO: How to differentiate between Python 2 and Python 3?
set_package_properties(PythonLibs PROPERTIES
    DESCRIPTION "Python is a programming language that lets you work more quickly and integrate your systems more effectively."
    URL         "http://python.org"
    TYPE        OPTIONAL
    PURPOSE     "Kross Python support.")

### Falcon support disabled because it does not work with most modern falcon-0.9.x
# set_package_properties(Falcon PROPERTIES
#     DESCRIPTION "Falcon is an Open Source, simple, fast and powerful programming language, easy to learn and to feel comfortable with."
#     URL         "http://www.falconpl.org/"
#     TYPE        OPTIONAL
#     PURPOSE     "Kross Falcon support.")

# ========================= Traverse subdirectories for actual targets =========================

if(PYTHONLIBS_FOUND)
    ecm_optional_add_subdirectory(python)
endif(PYTHONLIBS_FOUND)

### Falcon support disabled because it does not work with most modern falcon-0.9.x
# if(FALCON_FOUND)
#     ecm_optional_add_subdirectory(falcon)
# endif(FALCON_FOUND)

ecm_optional_add_subdirectory(ruby)
# ecm_optional_add_subdirectory(java)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

add_subdirectory(tests)
