cmake_minimum_required(VERSION 3.28)

project(samesies
    VERSION 1.0.0
    LANGUAGES CXX
)

option(SAMESIES_BUILD_TESTS "Build the Criterion test suite" ON)
option(SAMESIES_STATIC "Link the samesies binary statically" OFF)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wconversion
)

file(GLOB SAMESIES_LIBRARY_SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)
list(FILTER SAMESIES_LIBRARY_SOURCES EXCLUDE REGEX "/main\\.cpp$")

add_executable(samesies
    src/main.cpp
    ${SAMESIES_LIBRARY_SOURCES}
)

target_include_directories(samesies
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_definitions(samesies
    PRIVATE
        SAMESIES_VERSION="${PROJECT_VERSION}"
)

find_package(Threads REQUIRED)
target_link_libraries(samesies PRIVATE Threads::Threads)

if(SAMESIES_STATIC)
    target_link_options(samesies PRIVATE -static)
endif()

install(TARGETS samesies RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(SAMESIES_BUILD_TESTS)
    file(GLOB SAMESIES_TEST_SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/tests/test_*.cpp)

    if(SAMESIES_TEST_SOURCES)
        enable_testing()

        find_package(PkgConfig REQUIRED)
        pkg_check_modules(CRITERION REQUIRED criterion)

        add_executable(samesies_tests
            ${SAMESIES_TEST_SOURCES}
            ${SAMESIES_LIBRARY_SOURCES}
        )

        target_include_directories(samesies_tests
            PRIVATE
                ${PROJECT_SOURCE_DIR}/include
                ${CRITERION_INCLUDE_DIRS}
        )

        target_link_directories(samesies_tests
            PRIVATE
                ${CRITERION_LIBRARY_DIRS}
        )

        target_link_libraries(samesies_tests
            PRIVATE
                Threads::Threads
                ${CRITERION_LIBRARIES}
        )

        add_test(
            NAME samesies_tests
            COMMAND samesies_tests
        )
    endif()
endif()
