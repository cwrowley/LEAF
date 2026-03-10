cmake_minimum_required(VERSION 3.26)
project(LEAF)

include(GNUInstallDirs)

set(LIBRARY_BASE_PATH "${PROJECT_SOURCE_DIR}")
set(LIBRARY_SRC_PATH "${LIBRARY_BASE_PATH}/leaf/src")
set(LIBRARY_INCLUDE_PATH "${LIBRARY_BASE_PATH}/leaf/include/leaf")

set(PUBLIC_SOURCES_FILES
        "${LIBRARY_SRC_PATH}/leaf.cpp"
        "${LIBRARY_SRC_PATH}/leaf-math.c"
        "${LIBRARY_SRC_PATH}/leaf-analysis.c"
        "${LIBRARY_SRC_PATH}/leaf-delay.c"
        "${LIBRARY_SRC_PATH}/leaf-distortion.c"
        "${LIBRARY_SRC_PATH}/leaf-dynamics.c"
        "${LIBRARY_SRC_PATH}/leaf-effects.c"
        "${LIBRARY_SRC_PATH}/leaf-envelopes.c"
        "${LIBRARY_SRC_PATH}/leaf-electrical.c"
        "${LIBRARY_SRC_PATH}/leaf-filters.c"
        "${LIBRARY_SRC_PATH}/leaf-instruments.c"
        "${LIBRARY_SRC_PATH}/Mempool.cpp"
        "${LIBRARY_SRC_PATH}/leaf-midi.c"
        "${LIBRARY_SRC_PATH}/leaf-oscillators.cpp"
        "${LIBRARY_SRC_PATH}/leaf-physical.cpp"
        "${LIBRARY_SRC_PATH}/physical/StiffString.cpp"
        "${LIBRARY_SRC_PATH}/oscillators/Cycle.cpp"
        "${LIBRARY_SRC_PATH}/leaf-reverb.c"
        "${LIBRARY_SRC_PATH}/leaf-sampling.c"
        "${LIBRARY_SRC_PATH}/leaf-tables.c"
        "${LIBRARY_SRC_PATH}/leaf-vocal.c"
)

set(PRIVATE_SOURCES_FILES
        "${LIBRARY_BASE_PATH}/leaf/Externals/d_fft_mayer.c"
)

set(PUBLIC_HEADERS_FILES
        "${LIBRARY_INCLUDE_PATH}/leaf-config.h"
        "${LIBRARY_INCLUDE_PATH}/leaf.h"
        "${LIBRARY_INCLUDE_PATH}/leaf-global.h"
        "${LIBRARY_INCLUDE_PATH}/leaf-math.h"
        "${LIBRARY_INCLUDE_PATH}/leaf-analysis.h"
        "${LIBRARY_INCLUDE_PATH}/leaf-delay.h"
        "${LIBRARY_INCLUDE_PATH}/leaf-distortion.h"
        "${LIBRARY_INCLUDE_PATH}/leaf-dynamics.h"
        "${LIBRARY_INCLUDE_PATH}/leaf-effects.h"
        "${LIBRARY_INCLUDE_PATH}/leaf-envelopes.h"
        "${LIBRARY_INCLUDE_PATH}/leaf-electrical.h"
        "${LIBRARY_INCLUDE_PATH}/leaf-filters.h"
        "${LIBRARY_INCLUDE_PATH}/leaf-instruments.h"
        "${LIBRARY_INCLUDE_PATH}/leaf-mempool.h"
        "${LIBRARY_INCLUDE_PATH}/Mempool.h"
        "${LIBRARY_INCLUDE_PATH}/PoolAllocated.h"
        "${LIBRARY_INCLUDE_PATH}/leaf-midi.h"
        "${LIBRARY_INCLUDE_PATH}/leaf-oscillators.h"
        "${LIBRARY_INCLUDE_PATH}/leaf-physical.h"
        "${LIBRARY_INCLUDE_PATH}/leaf-reverb.h"
        "${LIBRARY_INCLUDE_PATH}/leaf-sampling.h"
        "${LIBRARY_INCLUDE_PATH}/leaf-tables.h"
        "${LIBRARY_INCLUDE_PATH}/leaf-vocal.h"
        "${LIBRARY_INCLUDE_PATH}/physical/StiffString.h"
        "${LIBRARY_INCLUDE_PATH}/oscillators/Cycle.h"
)

set(PRIVATE_HEADERS_FILES
        "${LIBRARY_BASE_PATH}/leaf/Externals/d_fft_mayer.h"
        "${LIBRARY_BASE_PATH}/leaf/Externals/trigtbl.h"
)

add_library(${PROJECT_NAME} STATIC ${PUBLIC_SOURCES_FILES} ${PRIVATE_SOURCES_FILES})

target_sources(${PROJECT_NAME} PRIVATE ${PRIVATE_SOURCES_FILES} ${PRIVATE_HEADERS_FILES} ${PUBLIC_SOURCES_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
        "${LIBRARY_BASE_PATH}/leaf/include"         # public: <leaf/leaf.h>, <leaf/physical/StiffString.h>
        "${LIBRARY_BASE_PATH}/leaf/include/leaf"    # backward-compat: <leaf.h>, <leaf-global.h> etc.
        "${LIBRARY_BASE_PATH}/leaf/Externals")
target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-narrowing")
set_target_properties(${PROJECT_NAME} PROPERTIES
        VERSION   0.1
        SOVERSION 0
)

enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

add_subdirectory(test)
add_subdirectory(Examples)
add_compile_options( -w )
add_compile_options(-Wno-implicit-float-conversion)

install(TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY "${LIBRARY_BASE_PATH}/leaf/include/leaf"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)