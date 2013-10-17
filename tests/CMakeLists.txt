set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

include_directories("${PROJECT_SOURCE_DIR}/include/")
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/gtest/include/")
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/gtest/")

set(TESTS_SOURCES
    main.cpp
    bnode_test.cpp
    init_test.cpp
    recfile_test.cpp)    

add_library(gtest STATIC "${PROJECT_SOURCE_DIR}/3rdparty/gtest/src/gtest-all.cc")

add_executable(btree_tests ${TESTS_SOURCES})

IF(WIN32)
    set_target_properties(btree_tests PROPERTIES COMPILE_FLAGS "-DBTREE_USE_DLL -D_SCL_SECURE_NO_WARNINGS")
ENDIF(WIN32)

add_custom_command(TARGET btree_tests
                    POST_BUILD
                    COMMAND btree_tests
                    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

target_link_libraries (btree_tests gtest btree)