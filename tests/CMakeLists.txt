set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

IF(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x -Wall")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

include_directories("${PROJECT_SOURCE_DIR}/include/")
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/gtest/include/")
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/gtest/")

set(BTREE_HEADERS
    ${PROJECT_SOURCE_DIR}/include/btree/exceptions.h
    ${PROJECT_SOURCE_DIR}/include/btree/btree.h
    ${PROJECT_SOURCE_DIR}/include/btree/btreenode.h
    ${PROJECT_SOURCE_DIR}/include/btree/defpack.h
    ${PROJECT_SOURCE_DIR}/include/btree/keypack.h
    ${PROJECT_SOURCE_DIR}/include/btree/filelocation.h
    ${PROJECT_SOURCE_DIR}/include/btree/podfilerecord.h
    ${PROJECT_SOURCE_DIR}/include/btree/recordfile.h
    ${PROJECT_SOURCE_DIR}/include/btree/stream.h)  

set(TESTS_SOURCES
    bnode_test.cpp
    btree_test.cpp
    recfile_test.cpp)    

set(GTEST_SOURCES
    ${PROJECT_SOURCE_DIR}/3rdparty/gtest/src/gtest-all.cc
    ${PROJECT_SOURCE_DIR}/3rdparty/gtest/src/gtest_main.cc)
    
add_library(gtest STATIC ${GTEST_SOURCES})

source_group(btree FILES ${BTREE_HEADERS})
source_group(tests FILES ${TESTS_SOURCES})
add_executable(tests ${TESTS_SOURCES} ${BTREE_HEADERS})

IF(WIN32)
    set_target_properties(tests PROPERTIES COMPILE_FLAGS "-D_SCL_SECURE_NO_WARNINGS")
ENDIF(WIN32)

target_link_libraries (tests gtest)

add_custom_target(tests_check
                    COMMAND $<TARGET_FILE:tests>)
ADD_DEPENDENCIES(tests_check tests)
