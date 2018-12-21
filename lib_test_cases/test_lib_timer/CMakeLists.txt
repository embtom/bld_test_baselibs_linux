project(test_lib_timer)

#However, the file(GLOB...) allows for wildcard additions:

SET(UNIT_TEST_SRC lib_timer_test.c)


# SET(PROJECT_LINK_LIBRARIES lib_test_framework_EMBUNIT)

#file(GLOB SOURCES "*.c")
add_definitions(-DTRACE)

if("${TEST_CASES_BASE}" STREQUAL "DEP_EMBTOM_INTREE")
    LIST(APPEND PROJECT_LINK_LIBRARIES lib_clock lib_timer)
endif("${TEST_CASES_BASE}" STREQUAL "DEP_EMBTOM_INTREE")    

INSTALL_TEST_CASE(${UNIT_TEST_SRC})
