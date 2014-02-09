include(${PROJECT_SOURCE_DIR}/btree/btree.cmake)

include_directories("${PROJECT_SOURCE_DIR}/3rdparty/gtest/include/")
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/gtest/")

set(TESTS_SOURCES
    bnode_test.cpp
    btree_test.cpp
    recfile_test.cpp
    treetestfix.h)

set(GTEST_SOURCES
    ${PROJECT_SOURCE_DIR}/3rdparty/gtest/src/gtest-all.cc
    ${PROJECT_SOURCE_DIR}/3rdparty/gtest/src/gtest_main.cc)
    
add_library(gtest STATIC ${GTEST_SOURCES})

source_group(sources FILES ${TESTS_SOURCES})
source_group(sources\\btree FILES ${BTREE_LIB_SOURCES})

add_executable(tests ${TESTS_SOURCES} ${BTREE_LIB_SOURCES})

target_link_libraries (tests gtest)

add_custom_target(tests_check
                    COMMAND $<TARGET_FILE:tests>)
ADD_DEPENDENCIES(tests_check tests)
