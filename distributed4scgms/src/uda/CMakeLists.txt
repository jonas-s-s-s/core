CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
SET(PROJ "uda_wrapper")

set(SRC_FILES "./uda_wrapper.cpp" "./uda_wrapper.h")

IF(WIN32)
    FILE(GLOB SRC_WIN_FILES "./win/*.cpp" "./win/*.h" "./win/*.def")
    SET(SRC_FILES "${SRC_FILES};${SRC_WIN_FILES}")
ENDIF()

SCGMS_ADD_LIBRARY(${PROJ} SHARED ${SRC_FILES})

# Linking against distributed might not be necessary
target_link_libraries(${PROJ} distributed pagmo)

SCGMS_LINK_COMMON_LIBRARIES(${PROJ})
APPLY_SCGMS_LIBRARY_BUILD_SETTINGS(${PROJ})
#CONFIGURE_FILTER_OUTPUT(${PROJ})

# Put this dll into the compiled directory
set_target_properties(${PROJ}
        PROPERTIES
        PREFIX ""
        OUTPUT_NAME ${PROJ}
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/compiled/"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/compiled/"
)

# Add the current directory into target's include dirs, anything that links against this lib at build time will be able
# to include our headers
target_include_directories(uda_wrapper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# The example executable
add_executable(uda_example uda_exmaple_main.cpp)
target_link_libraries(uda_example ${PROJ})
set_target_properties(uda_example
        PROPERTIES
        PREFIX ""
        OUTPUT_NAME uda_example
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/compiled/"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/compiled/"
)