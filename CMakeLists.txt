# CMakeList.txt : CMake project for delegate tests, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15)

project ("DelegateTests")

# Set project variables

set(TESTS_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_compile_definitions(DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN)

# Add includes

include_directories(
    ${PROJECT_SOURCE_DIR}
)

list(APPEND CMAKE_MODULE_PATH ${TESTS_DIRECTORY}/doctest.CMake)
include(CTest)
include(doctest)

# Add source to this project's executable.

aux_source_directory("${TESTS_DIRECTORY}" TESTS_SOURCES)

foreach(test_source_file ${TESTS_SOURCES})
    get_filename_component(test_name ${test_source_file} NAME_WE)
    add_executable(${test_name} 
        ${test_source_file}
    )
    doctest_discover_tests(${test_name})
endforeach(test_source_file ${TEST_SOURCES})
