set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

IF(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x -Wall")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

include_directories("${PROJECT_SOURCE_DIR}/include/")
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/gtest/include/")
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/gtest/")

set(TESTS_SOURCES
    main.cpp
    bnode_test.cpp
    btree_test.cpp
    recfile_test.cpp)    

add_library(gtest STATIC "${PROJECT_SOURCE_DIR}/3rdparty/gtest/src/gtest-all.cc")

add_executable(tests ${TESTS_SOURCES})

IF(WIN32)
    set_target_properties(tests PROPERTIES COMPILE_FLAGS "-DBTREE_USE_DLL -D_SCL_SECURE_NO_WARNINGS")
ENDIF(WIN32)

target_link_libraries (tests gtest btree)

add_custom_target(tests_check
                    COMMAND $<TARGET_FILE:tests>)
ADD_DEPENDENCIES(tests_check tests)
