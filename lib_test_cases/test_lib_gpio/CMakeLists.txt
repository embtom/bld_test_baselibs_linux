project(test_lib_gpio)

SET(UNIT_TEST_SRC lib_gpio_test.c)

#if("${TEST_CASES_BASE}" STREQUAL "DEP_EMBTOM_INTREE")
#endif("${TEST_CASES_BASE}" STREQUAL "DEP_EMBTOM_INTREE")    

if (TARGET lib_gpio)
    LIST(APPEND PROJECT_LINK_LIBRARIES lib_gpio)
    INSTALL_TEST_CASE(${UNIT_TEST_SRC})
    target_compile_definitions(${PROJECT_NAME} PRIVATE TRACE)
endif()
