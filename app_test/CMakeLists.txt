project(app_test)

######################################################################################
# Selector to choose app_test base
######################################################################################

FUNCTION(app_test_base)
	FOREACH(ARG ${ARGV})
  		set_property(GLOBAL APPEND PROPERTY APP_TEST_BASES ${ARG})
	ENDFOREACH(ARG)
ENDFUNCTION(app_test_base)

app_test_base("pkg_embtom")
app_test_base("osbase_target")
app_test_base("split_target")

set(APP_TEST_BASE ${APP_TEST_BASE} CACHE STRING "Selection of the base dependency")
get_property(bases GLOBAL PROPERTY APP_TEST_BASES)
list(SORT bases)
set_property(CACHE APP_TEST_BASE PROPERTY STRINGS None ${bases})

#######################################################################################
# Evaluate selection
#######################################################################################
SET(APP_TEST_SOURCE_C   		"src/app_test.c")
SET(APP_TEST_ITF_HEADER         "include")

if("${APP_TEST_BASE}" STREQUAL "pkg_embtom")
	find_package(osbase REQUIRED)
	find_package(iotty REQUIRED)
	SET(PROJECT_LINK_LIBRARIES EMBTOM::osbase EMBTOM::iotty)
elseif("${APP_TEST_BASE}" STREQUAL "osbase_target")
	SET(PROJECT_LINK_LIBRARIES os_basic lib_tty_portmux)	
elseif("${APP_TEST_BASE}" STREQUAL "split_target")
	SET(PROJECT_LINK_LIBRARIES itf_lib_convention lib_tty_portmux lib_thread lib_log)
elseif("${APP_TEST_BASE}" STREQUAL "None")
	message(FATAL_ERROR "No valid cmp base selected! Please check APP_TEST_BASE.")
else()
	message(FATAL_ERROR "No valid cmp base selected! Please check APP_TEST_BASE.")
endif("${APP_TEST_BASE}" STREQUAL "pkg_embtom")

message(STATUS "APP_TEST_BASE : selected base ${APP_TEST_BASE}")
LIST(APPEND PROJECT_LINK_LIBRARIES lib_menu test_cases)
######################################################
#Build target settings
######################################################

#Generate the static library from the sources
add_library(${PROJECT_NAME} STATIC ${APP_TEST_SOURCE_C})
target_include_directories(${PROJECT_NAME} PUBLIC ${APP_TEST_ITF_HEADER})
target_link_libraries(${PROJECT_NAME} ${PROJECT_LINK_LIBRARIES})


