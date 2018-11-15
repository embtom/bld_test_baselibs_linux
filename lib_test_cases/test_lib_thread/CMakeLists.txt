project(test_lib_thread)

SET(UNIT_TEST_SRC lib_thread_test.c)
INSTALL_TEST_CASE(${UNIT_TEST_SRC})


# add_custom_command(
#     OUTPUT ${PROJECT_BINARY_DIR}/my_generated_header.h
#     COMMAND "echo" ">" "my_generated_header.h"
#     COMMAND make lib_thread_test.c.i
# )
# add_custom_target(generate_header DEPENDS 
#     ${PROJECT_BINARY_DIR}/my_generated_header.h
# )

#add_custom_command(${PROJECT_NAME}
#   	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} 
#    COMMAND make lib_thread_test.c.i 
#)

#add_dependencies(start_preprocessor ${PROJECT_NAME})
#add_dependencies(test_lib_thread generate_header)


