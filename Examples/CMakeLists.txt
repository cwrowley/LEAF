add_executable(pool-allocation-demo pool-allocation-demo.cpp)
target_link_libraries(pool-allocation-demo PRIVATE LEAF)

add_executable(minimal minimal.c)
target_link_libraries(minimal PRIVATE LEAF)

add_executable(minimal_new minimal_new.cpp)
target_link_libraries(minimal_new PRIVATE LEAF)

add_executable(minimal_stack minimal_stack.cpp)
target_link_libraries(minimal_stack PRIVATE LEAF)