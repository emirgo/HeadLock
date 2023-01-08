#pragma once
#include <iostream>

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}


/* Notes
* 0040ADB7 for entity toggle code cave
*/

struct Offset {
    static const std::uintptr_t local_player = 0x10F4F4;
    static const std::uintptr_t local_entity_base = 0x109B74;
    static const std::uintptr_t health = 0x109B74;
};

