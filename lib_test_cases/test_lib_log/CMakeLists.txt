project(test_lib_log)

SET(UNIT_TEST_SRC lib_log_test.c)

INSTALL_TEST_CASE(${UNIT_TEST_SRC})
target_compile_definitions(${PROJECT_NAME} PRIVATE TRACE)
