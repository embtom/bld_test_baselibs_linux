project(test_runner)
cmake_minimum_required(VERSION 2.8)

SET(APP_TEST_BASE "pkg_embtom")
SET(LIB_MENU_BASE "pkg_embtom")
SET(TEST_CASES_BASE "pkg_embtom")

find_package(osbase REQUIRED)
find_package(iotty REQUIRED) 

add_subdirectory(app_test)
add_subdirectory(lib_menu)
add_subdirectory(lib_test_cases)
add_subdirectory(lib_test_framework_EMBUNIT)

#However, the file(GLOB...) allows for wildcard additions:
set(SOURCES main.c )
 
#link_directories(${lib_log_BINARY_DIR})
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} app_test EMBTOM::osbase EMBTOM::iotty)
