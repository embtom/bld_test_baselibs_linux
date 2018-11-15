project(lib_test_framework_EMBUNIT)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.c")



#Generate the static library from the sources
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ./)

if(TARGET lib_tty_portmux)
   message(STATUS "lib_trace_CORTEXM output")
   target_link_libraries(${PROJECT_NAME} lib_tty_portmux)
   target_compile_definitions(${PROJECT_NAME} PRIVATE DEV_TRACE_VPRINTF DEV_TRACE_PRINTF)
endif()

#target_link_libraries(${PROJECT_NAME} lib_convention itf_lib_clock)