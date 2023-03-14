#pragma once
#include <d3d9.h>

namespace gui
{
	constexpr int WIDTH = 500;
	constexpr int HEIGHT = 300;

	inline bool exit = true;

	// winapi window vars
	inline HWND window = nullptr;
	inline WNDCLASSEXA window_class = { };

	// points for window movement
	inline POINTS position = { };

	// direct x state vars
	inline PDIRECT3D9 d3d = nullptr;
	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline D3DPRESENT_PARAMETERS present_parameters = { };

	// handle window creation & destruction
	void CreateHWindow(
		const char* window_name,
		const char* class_name) noexcept;
	void DestroyHWWindow() noexcept;

	// handle device creation & destruction
	bool CreateDevice() noexcept;
	void ResetDevice() noexcept;
	void DestroyDevice() noexcept;

	// handle ImGui creation & destruction
	void CreateImGui() noexcept;
	void DestroyImGui() noexcept;

	void BeginRender() noexcept;
	void EndRender() noexcept;
	void Render() noexcept;
}