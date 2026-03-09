include(AddCatch2)

add_executable(leaf-tests
    allocation/test_allocation.cpp
    oscillators/test_Cycle.cpp
    physical/test_StiffString.cpp
)

target_include_directories(leaf-tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(leaf-tests PRIVATE LEAF Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(leaf-tests)
