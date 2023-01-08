#include "pch.h"
#include "Driver.h"

void Driver::Patch(BYTE* dst, BYTE* src, std::size_t size)
{
	DWORD old_protect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &old_protect);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, old_protect, &old_protect);
}

void Driver::Nop(BYTE* dst, std::size_t size)
{
	DWORD old_protect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &old_protect);
	memset(dst, 0x90, size); // nop function
	VirtualProtect(dst, size, old_protect, &old_protect);
}

std::uintptr_t Driver::ResolveAddr(std::uintptr_t ptr, std::vector<std::size_t> offsets)
{
	std::uintptr_t addr = ptr;
	for (std::size_t i = 0; i < offsets.size(); i++) {
		addr = *(std::uintptr_t*)addr;
		addr += offsets[i];
	}

	return addr;
}