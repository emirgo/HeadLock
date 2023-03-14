#include "Gui.h"

#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wide_parameter,
	LPARAM long_parameter
);

long __stdcall WindowProcess(
	HWND window,
	UINT message,
	WPARAM wide_parameter,
	LPARAM long_parameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wide_parameter, long_parameter))
		return true;

	switch (message)
	{
	case WM_SIZE: {
		if (gui::device && wide_parameter != SIZE_MINIMIZED)
		{
			gui::present_parameters.BackBufferWidth = LOWORD(long_parameter);
			gui::present_parameters.BackBufferHeight = HIWORD(long_parameter);
			gui::ResetDevice();
		}
	} return 0;
	case WM_SYSCOMMAND: {
		if ((wide_parameter & 0xffff0) == SC_KEYMENU)
			return 0;
	} break;
	case WM_DESTROY: {
		PostQuitMessage(0);
	} return 0;
	case WM_LBUTTONDOWN: {
		gui::position = MAKEPOINTS(long_parameter); // set click points
	} return 0;
	case WM_MOUSEMOVE: {
		if (wide_parameter == MK_LBUTTON)
		{
			const auto points = MAKEPOINTS(long_parameter);
			auto rect = ::RECT{};

			GetWindowRect(gui::window, &rect);

			rect.left += points.x - gui::position.x;
			rect.top += points.y - gui::position.y;

			if (gui::position.x >= 0 &&
				gui::position.x <= gui::WIDTH &&
				gui::position.y >= 0 && gui::position.y <= 19)
				SetWindowPos(
					gui::window,
					HWND_TOPMOST,
					rect.left,
					rect.top,
					0, 0,
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
		}
	} return 0;
	}

	return DefWindowProcW(window, message, wide_parameter, long_parameter);
}

void gui::CreateHWindow(const char* window_name, const char* class_name) noexcept
{
	window_class.cbSize = sizeof(WNDCLASSEXA);
	window_class.style = CS_CLASSDC;
	window_class.lpfnWndProc = WindowProcess;
	window_class.cbClsExtra = 0;
	window_class.cbWndExtra = 0;
	window_class.hInstance = GetModuleHandleA(0);
	window_class.hIcon = 0;
	window_class.hCursor = 0;
	window_class.hbrBackground = 0;
	window_class.lpszMenuName = 0;
	window_class.lpszClassName = class_name;
	window_class.hIconSm = 0;

	RegisterClassExA(&window_class);

	window = CreateWindow(
		class_name,
		window_name,
		WS_POPUP,
		100,
		100,
		WIDTH,
		HEIGHT,
		0, 0,
		window_class.hInstance,
		0
	);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void gui::DestroyHWWindow() noexcept
{
}

bool gui::CreateDevice() noexcept
{
	return false;
}

void gui::ResetDevice() noexcept
{
}

void gui::DestroyDevice() noexcept
{
}

void gui::CreateImGui() noexcept
{
}

void gui::DestroyImGui() noexcept
{
}

void gui::BeginRender() noexcept
{
}

void gui::EndRender() noexcept
{
}

void gui::Render() noexcept
{
}
