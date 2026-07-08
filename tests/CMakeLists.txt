# tests/CMakeLists.txt — populated by later tasks

add_executable(test_options test_options.cpp)
target_link_libraries(test_options PRIVATE sendspind_lib)
add_test(NAME options COMMAND test_options)

add_executable(test_volume_scale test_volume_scale.cpp)
target_link_libraries(test_volume_scale PRIVATE sendspind_lib)
add_test(NAME volume_scale COMMAND test_volume_scale)
