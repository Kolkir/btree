#ifndef _IMPORT_EXPORT_H_
#define _IMPORT_EXPORT_H_

#ifdef BTREE_USE_DLL
    #ifdef BTREE_BUILDING_PROJECT
        #define BTREE_API __declspec(dllexport)
    #else
        #define BTREE_API __declspec(dllimport)
    #endif
#else
    #define BTREE_API
#endif

#endif