cmake_minimum_required(VERSION 3.13)
if(UNIX)
SET(NETSIM_LIB_NAME netsim)
else()
MESSAGE("NOT UNIX!")
SET(NETSIM_LIB_NAME libnetsim-windows.a)
endif()

MESSAGE("NETSIM_LIB_NAME: ${NETSIM_LIB_NAME}")

project(gbn)

SET(CMAKE_C_COMPTLER GCC)
add_definitions(-std=c++11)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/shared SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/${PROJECT_NAME} SRC_LIST)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST})
FIND_LIBRARY(NETSIM_LIB ${NETSIM_LIB_NAME} ${PROJECT_SOURCE_DIR}/lib)
MESSAGE("========NETSIM_LIB: ${NETSIM_LIB}")
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  ${NETSIM_LIB})

project(sr)
SET(CMAKE_C_COMPTLER GCC)
SET(SRC_LIST)
add_definitions(-std=c++11)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/shared SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/${PROJECT_NAME} SRC_LIST)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST})
FIND_LIBRARY(NETSIM_LIB ${NETSIM_LIB_NAME} ${PROJECT_SOURCE_DIR}/lib)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  ${NETSIM_LIB})

project(stopwait)
SET(CMAKE_C_COMPTLER GCC)
SET(SRC_LIST)
add_definitions(-std=c++11)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/shared SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/${PROJECT_NAME} SRC_LIST)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST})
FIND_LIBRARY(NETSIM_LIB ${NETSIM_LIB_NAME} ${PROJECT_SOURCE_DIR}/lib)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  ${NETSIM_LIB})

project(tcp)
SET(CMAKE_C_COMPTLER GCC)
SET(SRC_LIST)
add_definitions(-std=c++11)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/shared SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/${PROJECT_NAME} SRC_LIST)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST})
FIND_LIBRARY(NETSIM_LIB ${NETSIM_LIB_NAME} ${PROJECT_SOURCE_DIR}/lib)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  ${NETSIM_LIB})