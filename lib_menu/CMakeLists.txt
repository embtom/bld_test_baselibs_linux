project(lib_menu)

######################################################################################
# Selector to choose menu base
######################################################################################

# Global Registration of the available list lock practices 
FUNCTION(lib_menu_base)
	FOREACH(ARG ${ARGV})
  		set_property(GLOBAL APPEND PROPERTY LIB_MENU_BASES ${ARG})
	ENDFOREACH(ARG)
ENDFUNCTION(lib_menu_base)

lib_menu_base("pkg_embtom")
lib_menu_base("osbase_target")
lib_menu_base("split_target")

set(LIB_MENU_BASE ${LIB_MENU_BASE} CACHE STRING "Selection of the base dependency")
get_property(bases GLOBAL PROPERTY LIB_MENU_BASES)
list(SORT bases)
set_property(CACHE LIB_MENU_BASE PROPERTY STRINGS None ${bases})

#######################################################################################
# Evaluate selection
#######################################################################################
SET(LIB_MENU_SOURCE_C   		"src/lib_menu.c")
SET(LIB_MENU_ITF_HEADER         "include")

if("${LIB_MENU_BASE}" STREQUAL "pkg_embtom")
	find_package(osbase REQUIRED)
	find_package(iotty REQUIRED)
	SET(PROJECT_LINK_LIBRARIES EMBTOM::osbase EMBTOM::iotty)
elseif("${LIB_MENU_BASE}" STREQUAL "osbase_target")
	SET(PROJECT_LINK_LIBRARIES itf_lib_convention lib_tty_portmux lib_log)
	LIST(APPEND PROJECT_LINK_LIBRARIES os_basic)	
elseif("${LIB_MENU_BASE}" STREQUAL "split_target")
	SET(PROJECT_LINK_LIBRARIES itf_lib_convention lib_tty_portmux lib_log)
	LIST(APPEND PROJECT_LINK_LIBRARIES lib_thread)
elseif("${LIB_MENU_BASE}" STREQUAL "None")
	message(FATAL_ERROR "No valid cmp base selected! Please check LIB_MENU_BASE.")
else()
	message(FATAL_ERROR "No valid cmp base selected! Please check LIB_MENU_BASE.")
endif("${LIB_MENU_BASE}" STREQUAL "pkg_embtom")

message(STATUS "LIB_MENU_BASE : selected base ${LIB_MENU_BASE}")

######################################################
#Build target settings
######################################################

add_library(${PROJECT_NAME} STATIC ${LIB_MENU_SOURCE_C})
target_link_libraries(${PROJECT_NAME} ${PROJECT_LINK_LIBRARIES}) 
target_include_directories(${PROJECT_NAME} PUBLIC ${LIB_MENU_ITF_HEADER})
