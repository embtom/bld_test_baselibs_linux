project(lib_gpio)

cmake_minimum_required(VERSION 2.6)
add_subdirectory(arch)

if("${LIB_GPIO_ARCHITECTURE}" STREQUAL "linux")    
    add_subdirectory(doc/test_app_gpio)
endif()

######################################################################################
# Build configurator
######################################################################################

#Selector to choose architecture to build lib_thread
set(LIB_GPIO_ARCHITECTURE ${LIB_GPIO_ARCHITECTURE} CACHE STRING "Architecture to build lib_gpio on")
get_property(architectures GLOBAL PROPERTY LIB_GPIO_ARCHITECTURES)
list(SORT architectures)
set_property(CACHE LIB_GPIO_ARCHITECTURE PROPERTY STRINGS None ${architectures})

#######################################################################################
# Check LIB_THREAD_ARCHITECTURE has a valid value
#######################################################################################
if("${LIB_GPIO_ARCHITECTURE}" STREQUAL "None")
message(FATAL_ERROR "No valid architecture chosen! Please check LIB_GPIO_ARCHITECTURE.")
endif("${LIB_GPIO_ARCHITECTURE}" STREQUAL "None")

#######################################################################################
# Get Architecture specific build paramters
#######################################################################################
GET_PROPERTY(LIB_GPIO_SOURCE_C            GLOBAL PROPERTY LIB_GPIO_ARCH_SOURCE_C)
GET_PROPERTY(LIB_GPIO_PUBLIC_HEADER       GLOBAL PROPERTY LIB_GPIO_ARCH_PUBLIC_HEADER)
GET_PROPERTY(LIB_GPIO_PRIVATE_HEADER      GLOBAL PROPERTY LIB_GPIO_ARCH_INTERNAL_HEADER)
GET_PROPERTY(LIB_GPIO_DEPEND              GLOBAL PROPERTY LIB_GPIO_ARCH_DEPEND)
GET_PROPERTY(LIB_GPIO_PUBLIC_DEFINITION  GLOBAL PROPERTY LIB_GPIO_ARCH_PUBLIC_DEFINITION)

SET(LIB_GPIO_PUBLIC_HEADER ${LIB_GPIO_PUBLIC_HEADER} include)
SET(LIB_GPIO_PRIVATE_HEADER ${LIB_GPIO_PRIVATE_HEADER} arch/internal_include)
SET(LIB_GPIO_SOURCE_C 
    ${LIB_GPIO_SOURCE_C} 
    src/lib_gpio.c 
    src/lib_gpio_config.c
    )

if("${LIB_GPIO_ARCHITECTURE}" STREQUAL "linux")    
    find_package(iotty REQUIRED)
    find_package(osbase REQUIRED)
endif()

######################################################
#Build target settings
######################################################

add_library(${PROJECT_NAME} STATIC ${LIB_GPIO_SOURCE_C})
target_link_libraries(${PROJECT_NAME} ${LIB_GPIO_DEPEND})
target_include_directories(${PROJECT_NAME} PUBLIC ./include ${LIB_GPIO_PUBLIC_HEADER})
target_include_directories(${PROJECT_NAME} PRIVATE ${LIB_GPIO_PRIVATE_HEADER})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${LIB_GPIO_PUBLIC_DEFINITION})
