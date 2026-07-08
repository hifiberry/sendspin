# tests/CMakeLists.txt — populated by later tasks

add_executable(test_options test_options.cpp)
target_link_libraries(test_options PRIVATE sendspind_lib)
add_test(NAME options COMMAND test_options)

add_executable(test_volume_scale test_volume_scale.cpp)
target_link_libraries(test_volume_scale PRIVATE sendspind_lib)
add_test(NAME volume_scale COMMAND test_volume_scale)

add_executable(test_report_json test_report_json.cpp)
target_link_libraries(test_report_json PRIVATE sendspind_lib nlohmann_json::nlohmann_json)
add_test(NAME report_json COMMAND test_report_json)
