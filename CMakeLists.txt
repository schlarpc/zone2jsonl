cmake_minimum_required(VERSION 3.10)
project(zone2jsonl C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
find_package(simdzone REQUIRED)
pkg_check_modules(JANSSON REQUIRED jansson)
pkg_check_modules(LDNS REQUIRED ldns)

include_directories(
    ${LIBDEFLATE_INCLUDE_DIRS}
    ${JANSSON_INCLUDE_DIRS}
    ${LDNS_INCLUDE_DIRS}
)

link_directories(
    ${JANSSON_LIBRARY_DIRS}
    ${LDNSLIBRARY_DIRS}
)

set(SOURCES
    zone2jsonl.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    simdzone::zone
    ${JANSSON_LIBRARIES}
    ${LDNS_LIBRARIES}
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
