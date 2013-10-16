cmake_minimum_required (VERSION 2.8)
project (BTree)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BIINARY_DIR}/bin)

include_directories("${PROJECT_SOURCE_DIR}/include/")

set(BTREE_HEADERS
    include/btree/btree.h
    include/btree/btreenode.h
    include/btree/defpack.h
    include/btree/filelocation.h
    include/btree/importexport.h
    include/btree/podfilerecord.h
    include/btree/recordfile.h
    include/btree/stream.h)

set(BTREE_SOURCES
    src/btree.cpp
    src/recordfile.cpp)    

add_library(btree SHARED ${BTREE_HEADERS} ${BTREE_SOURCES})
set_target_properties(btree PROPERTIES COMPILE_FLAGS "-DBTREE_USE_DLL -DBTREE_BUILDING_PROJECT -D_SCL_SECURE_NO_WARNINGS")

add_subdirectory(tests)