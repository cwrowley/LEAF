################################################################################
### Selecting CMake minimum version

cmake_minimum_required(VERSION 3.26)

################################################################################
### Setting Global Parameters

SET(PROJECT_NAME "LEAF")
SET(BINARY_NAME "LEAF")

PROJECT("${PROJECT_NAME}")

SET(LIBRARY_VERSION_MAJOR 0)
SET(LIBRARY_VERSION_STRING 0.1)

include(GNUInstallDirs)

SET(LIBRARY_BASE_PATH "${PROJECT_SOURCE_DIR}")

##############################################################################


################################################################################
### Project definition

#INCLUDE_DIRECTORIES (
#        "${LIBRARY_BASE_PATH}/leaf"
#)

SET(PUBLIC_SOURCES_FILES
        "${LIBRARY_BASE_PATH}/leaf/src/leaf.cpp"
        "${LIBRARY_BASE_PATH}/leaf/src/leaf-math.c"
        "${LIBRARY_BASE_PATH}/leaf/src/leaf-analysis.c"
        "${LIBRARY_BASE_PATH}/leaf/src/leaf-delay.c"
        "${LIBRARY_BASE_PATH}/leaf/src/leaf-distortion.c"
        "${LIBRARY_BASE_PATH}/leaf/src/leaf-dynamics.c"
        "${LIBRARY_BASE_PATH}/leaf/src/leaf-effects.c"
        "${LIBRARY_BASE_PATH}/leaf/src/leaf-envelopes.c"
        "${LIBRARY_BASE_PATH}/leaf/src/leaf-electrical.c"
        "${LIBRARY_BASE_PATH}/leaf/src/leaf-filters.c"
        "${LIBRARY_BASE_PATH}/leaf/src/leaf-instruments.c"
        "${LIBRARY_BASE_PATH}/leaf/src/leaf-mempool.cpp"
        "${LIBRARY_BASE_PATH}/leaf/src/leaf-midi.c"
        "${LIBRARY_BASE_PATH}/leaf/src/leaf-oscillators.cpp"
        "${LIBRARY_BASE_PATH}/leaf/src/leaf-physical.cpp"
        "${LIBRARY_BASE_PATH}/leaf/src/physical/StiffString.cpp"
        "${LIBRARY_BASE_PATH}/leaf/src/leaf-reverb.c"
        "${LIBRARY_BASE_PATH}/leaf/src/leaf-sampling.c"
        "${LIBRARY_BASE_PATH}/leaf/src/leaf-tables.c"
        "${LIBRARY_BASE_PATH}/leaf/src/leaf-vocal.c"

)

SET(PRIVATE_SOURCES_FILES
        "${LIBRARY_BASE_PATH}/leaf/Externals/d_fft_mayer.c"
)

SET(PUBLIC_HEADERS_FILES
        "${LIBRARY_BASE_PATH}/leaf/leaf-config.h"
        "${LIBRARY_BASE_PATH}/leaf/leaf.h"
        "${LIBRARY_BASE_PATH}/leaf/include/leaf-global.h"
        "${LIBRARY_BASE_PATH}/leaf/include/leaf-math.h"
        "${LIBRARY_BASE_PATH}/leaf/include/leaf-analysis.h"
        "${LIBRARY_BASE_PATH}/leaf/include/leaf-delay.h"
        "${LIBRARY_BASE_PATH}/leaf/include/leaf-distortion.h"
        "${LIBRARY_BASE_PATH}/leaf/include/leaf-dynamics.h"
        "${LIBRARY_BASE_PATH}/leaf/include/leaf-effects.h"
        "${LIBRARY_BASE_PATH}/leaf/include/leaf-envelopes.h"
        "${LIBRARY_BASE_PATH}/leaf/include/leaf-electrical.h"
        "${LIBRARY_BASE_PATH}/leaf/include/leaf-filters.h"
        "${LIBRARY_BASE_PATH}/leaf/include/leaf-instruments.h"
        "${LIBRARY_BASE_PATH}/leaf/include/leaf-mempool.h"
        "${LIBRARY_BASE_PATH}/leaf/include/leaf-midi.h"
        "${LIBRARY_BASE_PATH}/leaf/include/leaf-oscillators.h"
        "${LIBRARY_BASE_PATH}/leaf/include/leaf-physical.h"
        "${LIBRARY_BASE_PATH}/leaf/include/leaf-reverb.h"
        "${LIBRARY_BASE_PATH}/leaf/include/leaf-sampling.h"
        "${LIBRARY_BASE_PATH}/leaf/include/leaf-tables.h"
        "${LIBRARY_BASE_PATH}/leaf/include/leaf-vocal.h"
        "${LIBRARY_BASE_PATH}/leaf/include/physical/StiffString.h"

)

SET(PRIVATE_HEADERS_FILES
        "${LIBRARY_BASE_PATH}/leaf/Externals/d_fft_mayer.h"
        "${LIBRARY_BASE_PATH}/leaf/Externals/trigtbl.h"
)


#SET(CMAKE_C_VISIBILITY_PRESET hidden)
#
ADD_LIBRARY (
        ${BINARY_NAME} STATIC ${PUBLIC_SOURCES_FILES} ${PRIVATE_SOURCES_FILES}

)

TARGET_SOURCES( ${BINARY_NAME} PRIVATE  ${PRIVATE_SOURCES_FILES} ${PRIVATE_HEADERS_FILES}  PUBLIC ${PUBLIC_SOURCES_FILES})

target_include_directories(${BINARY_NAME} PUBLIC  "${LIBRARY_BASE_PATH}/leaf"
        "${LIBRARY_BASE_PATH}/leaf/include"
        "${LIBRARY_BASE_PATH}/leaf/Externals")
target_compile_options(${BINARY_NAME} PRIVATE "-Wno-narrowing")
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#add_subdirectory(test)
add_compile_options( -w )
add_compile_options(-Wno-implicit-float-conversion)
#ENABLE_TESTING()
SET_TARGET_PROPERTIES (
        ${BINARY_NAME} PROPERTIES
        VERSION		${LIBRARY_VERSION_STRING}
        SOVERSION	${LIBRARY_VERSION_MAJOR}
        PUBLIC_HEADER  "${PUBLIC_HEADERS_FILES}"
)

################################################################################
### Adding compilator definitions, like debug symbols

#ADD_DEFINITIONS("-g")

################################################################################
### Library installation directives

install(TARGETS ${BINARY_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY "${LIBRARY_BASE_PATH}/leaf/include/"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(FILES
        "${LIBRARY_BASE_PATH}/leaf/leaf.h"
        "${LIBRARY_BASE_PATH}/leaf/leaf-config.h"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

################################################################################