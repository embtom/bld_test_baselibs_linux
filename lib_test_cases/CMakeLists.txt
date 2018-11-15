project(test_cases)

######################################################################################
# Selector to choose the linkage base
######################################################################################

FUNCTION(test_cases_base)
	FOREACH(ARG ${ARGV})
  		set_property(GLOBAL APPEND PROPERTY TEST_CASES_BASES ${ARG})
	ENDFOREACH(ARG)
ENDFUNCTION(test_cases_base)

test_cases_base("pkg_embtom")
test_cases_base("osbase_target")
test_cases_base("split_target")

set(TEST_CASES_BASE ${TEST_CASES_BASE} CACHE STRING "Selection of the base dependency")
get_property(bases GLOBAL PROPERTY TEST_CASES_BASES)
list(SORT bases)
set_property(CACHE TEST_CASES_BASE PROPERTY STRINGS None ${bases})

#######################################################################################
# Evaluate selection
#######################################################################################
SET(TEST_CASES_SOURCE_C   		"${PROJECT_BINARY_DIR}/test_cases.c")
SET(TEST_CASES_ITF_HEADER       "include")
SET(PROJECT_LINK_LIBRARIES      lib_test_framework_EMBUNIT )

if("${TEST_CASES_BASE}" STREQUAL "pkg_embtom")
	find_package(osbase REQUIRED)
	find_package(iotty REQUIRED)
    LIST(APPEND PROJECT_LINK_LIBRARIES EMBTOM::osbase EMBTOM::iotty)
elseif("${TEST_CASES_BASE}" STREQUAL "osbase_target")
	LIST(APPEND PROJECT_LINK_LIBRARIES itf_lib_convention os_basic lib_log lib_tty_portmux)	
elseif("${TEST_CASES_BASE}" STREQUAL "split_target")
	LIST(APPEND PROJECT_LINK_LIBRARIES itf_lib_convention lib_thread lib_log lib_tty_portmux)
elseif("${TEST_CASES_BASE}" STREQUAL "None")
	message(FATAL_ERROR "No valid cmp base selected! Please check TEST_CASES_BASE.")
else()
	message(FATAL_ERROR "No valid cmp base selected! Please check TEST_CASES_BASE.")
endif("${TEST_CASES_BASE}" STREQUAL "pkg_embtom")

message(STATUS "TEST_CASES_BASE : selected lock ${TEST_CASES_BASE}")


######################################################################################
# Installation of new test cases at the test container
######################################################################################
#Clear variables
SET(GEN_TEST_CASE_INCLUDES CACHE INTERNAL "" FORCE)
SET(GEN_TEST_CASE_INIT_CALLS CACHE INTERNAL "" FORCE) 
SET(GEN_TEST_CASE_LIST CACHE INTERNAL "" FORCE )

FUNCTION (INSTALL_TEST_CASE _file_unit_test)
    file(READ ${PROJECT_SOURCE_DIR}/${_file_unit_test} FILE_READ_UNIT_TEST)
    STRING(REGEX MATCH "(TEST_CASE_INIT\\([a-zA-Z0-9_&, ]*\\))" INIT_STRING ${FILE_READ_UNIT_TEST})
    STRING(REGEX REPLACE "(\\()(.*,)(.*,)(.*)(\\))" "\\1;\\2;\\3;\\4;\\5" RESULT ${INIT_STRING})
    
    #get init function name
    list(GET RESULT 2 INIT_FUNCTION)
    string (REPLACE "," "" INIT_FUNCTION "${INIT_FUNCTION}")
    string (REPLACE " " "" INIT_FUNCTION "${INIT_FUNCTION}")    
    string (APPEND GEN_INIT_FUNCTION "init_" ${INIT_FUNCTION} "()")
    
    #get header name
    list(GET RESULT 1 INC_HEADER)
    string (REPLACE "," "" INC_HEADER "${INC_HEADER}")
    string (REPLACE " " "" INC_HEADER "${INC_HEADER}")
    string (APPEND GEN_INC_HEADER "test_" ${INC_HEADER} ".h")    

	STRING(APPEND GEN_TEST_CASE_INCLUDES "#include <${GEN_INC_HEADER}>\\")
    STRING(APPEND GEN_TEST_CASE_INIT_CALLS "\t${GEN_INIT_FUNCTION};\\") 
    LIST(APPEND GEN_TEST_CASE_LIST "${PROJECT_NAME}")  
    configure_file(${test_cases_SOURCE_DIR}/src/test_case_init.h.in ${PROJECT_BINARY_DIR}/${GEN_INC_HEADER})
    set(GEN_TEST_CASE_INCLUDES ${GEN_TEST_CASE_INCLUDES} CACHE INTERNAL "" FORCE)
    set(GEN_TEST_CASE_INIT_CALLS ${GEN_TEST_CASE_INIT_CALLS} CACHE INTERNAL "" FORCE)    
    set(GEN_TEST_CASE_LIST ${GEN_TEST_CASE_LIST} CACHE INTERNAL "" FORCE)
    message(STATUS "TEST CASES to attached - ${PROJECT_NAME}")
    message(STATUS "SOURCES ${_file_unit_test}")
    add_library(${PROJECT_NAME} STATIC ${_file_unit_test})
    target_include_directories(${PROJECT_NAME} PUBLIC ./ ${PROJECT_BINARY_DIR})
    target_link_libraries(${PROJECT_NAME} ${PROJECT_LINK_LIBRARIES} test_cases)
ENDFUNCTION()

######################################################
#Sub test cases
######################################################
add_subdirectory(test_lib_thread)
add_subdirectory(test_lib_list)
add_subdirectory(test_lib_timer)
add_subdirectory(test_lib_log)

######################################################
#Generation of the initilizer for the installed test cases
######################################################
string (REPLACE "\\" "\n" GEN_TEST_CASE_INCLUDES "${GEN_TEST_CASE_INCLUDES}")
string (REPLACE "\\" "\n" GEN_TEST_CASE_INIT_CALLS "${GEN_TEST_CASE_INIT_CALLS}")
configure_file(${test_cases_SOURCE_DIR}/src/test_cases.c.in ${PROJECT_BINARY_DIR}/test_cases.c)

message(STATUS "TEST CASES to link - ${PROJECT_LINK_LIBRARIES}")

######################################################
#Build target settings
######################################################
add_library(${PROJECT_NAME} STATIC ${TEST_CASES_SOURCE_C})
target_include_directories(${PROJECT_NAME} PUBLIC ${TEST_CASES_ITF_HEADER})
target_link_libraries(${PROJECT_NAME} ${PROJECT_LINK_LIBRARIES} ${GEN_TEST_CASE_LIST})



