 
include(${PROJECT_SOURCE_DIR}/btree/btree.cmake) 

set(GUI_SOURCES
    ${PROJECT_SOURCE_DIR}/gui/main.cpp
    ${PROJECT_SOURCE_DIR}/gui/gui.cpp
    ${PROJECT_SOURCE_DIR}/gui/gui.h)  

source_group(sources FILES ${GUI_SOURCES})    
source_group(sources\\btree FILES ${BTREE_LIB_SOURCES})

add_executable(gui ${GUI_SOURCES} ${BTREE_LIB_SOURCES})

target_link_libraries (gui fltk fltk_images)
