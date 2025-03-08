cmake_minimum_required(VERSION 3.16)
project(obs-speech-timer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Add cmake directory to module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Force static runtime libraries for all configurations
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Set Qt path and prefix path
set(CMAKE_PREFIX_PATH "C:/Qt/6.6.3/msvc2019_64")
set(QT_DIR "C:/Qt/6.6.3/msvc2019_64")

# Set OBS Studio SDK paths
set(OBS_STUDIO_DIR "C:/Program Files/obs-studio")
set(OBS_STUDIO_SRC "D:/Obs-studio/obs-studio")
set(LIBOBS_INCLUDE_DIR "${OBS_STUDIO_DIR}/include/obs")
set(LIBOBS_LIB_DIR "${OBS_STUDIO_DIR}/bin/64bit")

if(WIN32)
    set(OBS_FRONTEND_LIB "${LIBOBS_LIB_DIR}/obs-frontend-api.lib")
    set(LIBOBS_LIB "${LIBOBS_LIB_DIR}/obs.lib")
else()
    set(OBS_FRONTEND_LIB "obs-frontend-api")
    set(LIBOBS_LIB "libobs")
endif()

find_package(Qt6 COMPONENTS Widgets Core Gui REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${OBS_STUDIO_DIR}/include
    ${OBS_STUDIO_DIR}/include/obs
    ${OBS_STUDIO_DIR}/include/obs-frontend-api
    ${OBS_STUDIO_SRC}/libobs
    ${OBS_STUDIO_SRC}/UI/obs-frontend-api
    ${LIBOBS_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

link_directories(
    ${LIBOBS_LIB_DIR}
)

set(speech_timer_SOURCES
    src/plugin-main.cpp
    src/timer-dock.cpp
    src/obs-dock-wrapper.cpp
    src/appreciation-dialog.cpp
)

set(speech_timer_HEADERS
    src/timer-dock.hpp
    src/obs-dock-wrapper.hpp
    src/timer-record.hpp
)

add_library(obs-speech-timer MODULE
    ${speech_timer_SOURCES}
    ${speech_timer_HEADERS}
    resources.qrc
)

target_link_libraries(obs-speech-timer PRIVATE
    ${LIBOBS_LIB}
    ${OBS_FRONTEND_LIB}
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

if(WIN32)
    # Add Windows-specific definitions
    target_compile_definitions(obs-speech-timer PRIVATE 
        UNICODE 
        _UNICODE 
        _CRT_SECURE_NO_WARNINGS
        QT_NO_KEYWORDS
        EXPORT_LIB
        DLL_EXPORT
    )
    
    # Configure compiler options
    target_compile_options(obs-speech-timer PRIVATE 
        /Zi      # Generate debug info
        /Od      # Disable optimization
        /W3      # Warning level
        /WX-     # Treat warnings as errors: disabled
        /MT      # Use static runtime library
        /GS      # Buffer security check
        /Gy      # Function-level linking
    )
    
    # Configure linker options
    set_target_properties(obs-speech-timer PROPERTIES
        PREFIX ""
        SUFFIX ".dll"
        LINK_FLAGS "/DEBUG /SUBSYSTEM:WINDOWS /DYNAMICBASE:NO /OPT:REF"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    # Create a list of Qt DLLs to copy
    set(QT_DLLS
        "${QT_DIR}/bin/Qt6Core.dll"
        "${QT_DIR}/bin/Qt6Gui.dll"
        "${QT_DIR}/bin/Qt6Widgets.dll"
    )

    # Copy Qt runtime libraries to build directory
    foreach(QT_DLL ${QT_DLLS})
        add_custom_command(TARGET obs-speech-timer POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${QT_DLL}"
                "${CMAKE_BINARY_DIR}/bin/Release"
        )
    endforeach()

    # Copy Qt plugins to build directory
    set(QT_PLUGIN_PATHS
        "platforms/qwindows.dll"
        "styles/qwindowsvistastyle.dll"
    )

    foreach(PLUGIN ${QT_PLUGIN_PATHS})
        get_filename_component(PLUGIN_DIR "${PLUGIN}" DIRECTORY)
        add_custom_command(TARGET obs-speech-timer POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E make_directory
                "${CMAKE_BINARY_DIR}/bin/Release/${PLUGIN_DIR}"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${QT_DIR}/plugins/${PLUGIN}"
                "${CMAKE_BINARY_DIR}/bin/Release/${PLUGIN_DIR}/"
        )
    endforeach()
endif()

install(TARGETS obs-speech-timer
    LIBRARY DESTINATION "${CMAKE_BINARY_DIR}/bin/Release"
    RUNTIME DESTINATION "${CMAKE_BINARY_DIR}/bin/Release"
)

install(DIRECTORY locale
    DESTINATION "${CMAKE_BINARY_DIR}/bin/Release/data/obs-plugins/obs-speech-timer"
) 