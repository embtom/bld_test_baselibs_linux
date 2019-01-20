project(lib_test_framework_EMBUNIT)

#######################################################################################
# SOURCE FILES
#######################################################################################
SET(EMBUNIT_SRC AssertImpl.c
                RepeatedTest.c
                stdImpl.c
                TestCaller.c
                TestCase.c
                TestResult.c
                TestRunner.c
                TestSuite.c)

#######################################################################################
#Build target settings
#######################################################################################
add_library(${PROJECT_NAME} STATIC ${EMBUNIT_SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ./)

if(TARGET lib_ttyportmux)
   target_link_libraries(${PROJECT_NAME} lib_ttyportmux lib_mini_printf)
   target_compile_definitions(${PROJECT_NAME} PRIVATE MINI_PRINTF TTY_PORTMUX)
endif()

#target_link_libraries(${PROJECT_NAME} lib_convention itf_lib_clock)