project(test_lib_list)

SET(UNIT_TEST_SRC test_lib_list.c)

#SET(PROJECT_LINK_LIBRARIES lib_test_framework_EMBUNIT)

# if (TARGET os_basic)
# 	LIST(APPEND PROJECT_LINK_LIBRARIES os_basic)
# else()
# 	LIST(APPEND PROJECT_LINK_LIBRARIES lib_convention lib_thread lib_log lib_clock)
# endif()

INSTALL_TEST_CASE(${UNIT_TEST_SRC})