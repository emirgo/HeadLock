#pragma once
#include "pch.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

namespace Driver {
	void Patch(BYTE* dst, BYTE* src, std::size_t size);
	void Nop(BYTE* dst, std::size_t size);
	std::uintptr_t ResolveAddr(std::uintptr_t ptr, std::vector<std::size_t> offsets);
}

/*
class Driver {
public:
	
};

*/