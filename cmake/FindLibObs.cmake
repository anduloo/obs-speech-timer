# FindLibObs.cmake
# Find the OBS library and its dependencies

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_lib_suffix 64)
else()
    set(_lib_suffix 32)
endif()

if(DEFINED LIBOBS_INCLUDE_DIR AND DEFINED LIBOBS_LIB)
    set(LibObs_FOUND TRUE)
    return()
endif()

find_path(LIBOBS_INCLUDE_DIR
    NAMES obs-module.h
    HINTS
        ENV obsPath
        ENV OBS_PATH
        ENV DepsPath
        ENV DEPS_PATH
        ENV obsPath${_lib_suffix}
        ENV OBS_PATH${_lib_suffix}
        ENV DepsPath${_lib_suffix}
        ENV DEPS_PATH${_lib_suffix}
    PATHS
        /usr/include /usr/local/include /opt/local/include /sw/include
        "C:/Program Files/obs-studio/include"
        "C:/Program Files (x86)/obs-studio/include"
    PATH_SUFFIXES
        libobs
)

find_library(LIBOBS_LIB
    NAMES ${LIBOBS_LIB_NAME} obs libobs
    HINTS
        ENV obsPath
        ENV OBS_PATH
        ENV DepsPath
        ENV DEPS_PATH
        ENV obsPath${_lib_suffix}
        ENV OBS_PATH${_lib_suffix}
        ENV DepsPath${_lib_suffix}
        ENV DEPS_PATH${_lib_suffix}
    PATHS
        /usr/lib /usr/local/lib /opt/local/lib /sw/lib
        "C:/Program Files/obs-studio/bin/${_lib_suffix}bit"
        "C:/Program Files (x86)/obs-studio/bin/${_lib_suffix}bit"
    PATH_SUFFIXES
        lib${_lib_suffix} lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibObs DEFAULT_MSG LIBOBS_INCLUDE_DIR LIBOBS_LIB)
mark_as_advanced(LIBOBS_INCLUDE_DIR LIBOBS_LIB)

# Find OBS libraries and headers
find_path(OBS_FRONTEND_API_INCLUDE_DIR
	NAMES obs-frontend-api.h
	PATHS
		"${LIBOBS_INCLUDE_DIR}/obs-frontend-api"
	NO_DEFAULT_PATH
)

# Find OBS libraries
find_library(OBS_FRONTEND_API_LIB
	NAMES obs-frontend-api
	PATHS
		"${LIBOBS_LIB}"
	NO_DEFAULT_PATH
)

# Check if files exist
if(NOT LIBOBS_LIB OR NOT LIBOBS_INCLUDE_DIR)
	message(FATAL_ERROR "Could not find OBS library.")
endif()

if(NOT OBS_FRONTEND_API_LIB)
	message(FATAL_ERROR "Could not find OBS Frontend API library.")
endif()

# Handle standard arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibObs DEFAULT_MSG LIBOBS_INCLUDE_DIR OBS_FRONTEND_API_INCLUDE_DIR LIBOBS_LIB OBS_FRONTEND_API_LIB)

# Create imported targets
add_library(libobs STATIC IMPORTED)
set_target_properties(libobs PROPERTIES
	IMPORTED_LOCATION "${LIBOBS_LIB}"
	INTERFACE_INCLUDE_DIRECTORIES "${LIBOBS_INCLUDE_DIR};${LIBOBS_INCLUDE_DIR}/libobs/util"
)

add_library(obs-frontend-api STATIC IMPORTED)
set_target_properties(obs-frontend-api PROPERTIES
	IMPORTED_LOCATION "${OBS_FRONTEND_API_LIB}"
	INTERFACE_INCLUDE_DIRECTORIES "${LIBOBS_INCLUDE_DIR}"
)

# Set variables for find_package
if(LIBOBS_FOUND)
	set(LIBOBS_INCLUDE_DIRS 
		${LIBOBS_INCLUDE_DIR}
		${OBS_FRONTEND_API_INCLUDE_DIR}
		${LIBOBS_INCLUDE_DIR}/util
		${LIBOBS_INCLUDE_DIR}/libobs
	)
	set(LIBOBS_LIBRARIES ${LIBOBS_LIB} ${OBS_FRONTEND_API_LIB})
	mark_as_advanced(LIBOBS_INCLUDE_DIR OBS_FRONTEND_API_INCLUDE_DIR LIBOBS_LIB OBS_FRONTEND_API_LIB)
endif() 