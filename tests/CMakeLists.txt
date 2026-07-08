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

add_executable(test_command_map test_command_map.cpp)
target_link_libraries(test_command_map PRIVATE sendspind_lib)
add_test(NAME command_map COMMAND test_command_map)

add_executable(test_http_request test_http_request.cpp)
target_link_libraries(test_http_request PRIVATE sendspind_lib)
add_test(NAME http_request COMMAND test_http_request)

add_executable(test_metadata_map test_metadata_map.cpp)
target_link_libraries(test_metadata_map PRIVATE sendspind_lib)
add_test(NAME metadata_map COMMAND test_metadata_map)

add_executable(test_alsa_format test_alsa_format.cpp)
target_link_libraries(test_alsa_format PRIVATE sendspind_lib)
add_test(NAME alsa_format COMMAND test_alsa_format)
