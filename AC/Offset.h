#pragma once
#include <iostream>

/* Notes
* 0040ADB7 for entity toggle code cave
*/

struct Offset {
    static std::uintptr_t module_base;
    static const std::uintptr_t local_player = 0x50F4F4;
    static const std::uintptr_t local_entity_base = 0x109B74;
};

