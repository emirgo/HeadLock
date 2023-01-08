#pragma once
#include <iostream>

/* Notes
* 0040ADB7 for entity toggle code cave
*/

struct Offset {
    static const std::uintptr_t local_player = 0x10F4F4;
    static const std::uintptr_t local_entity_base = 0x109B74;
    static const std::uintptr_t health = 0x109B74;
};

