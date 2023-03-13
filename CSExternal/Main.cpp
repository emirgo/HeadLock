#include <Windows.h>
#include "Memory.h"
#include "Offset.h"
#include "Vector.h"
#include <thread>
#include <math.h>
#include <cmath>

#include <dwmapi.h>
#include <d3d11.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK window_rocedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param))
	{
		return 0;
	}

	if (message == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0L;
	}

	return DefWindowProc(window, message, w_param, l_param);
}

INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE, PSTR, INT cmd_show)
{
	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = window_rocedure;
	wc.hInstance = instance;
	wc.lpszClassName = L"Apalis Overlay Class";

	RegisterClassExW(&wc);

	const HWND window = CreateWindowExW(
		WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
		wc.lpszClassName,
		L"Apalisware",
		WS_POPUP,
		0,
		0,
		1920,
		1080,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);

	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

	{
		RECT client_area{};
		GetClientRect(window, &client_area);

		RECT window_area{};
		GetWindowRect(window, &window_area);

		POINT diff{};
		ClientToScreen(window, &diff);

		const MARGINS margins{
			window_area.left + (diff.x - window_area.left),
			window_area.top + (diff.y - window_area.top),
			client_area.right,
			client_area.bottom
		};

		DwmExtendFrameIntoClientArea(window, &margins);
	}

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.RefreshRate.Numerator = 60U;
	sd.BufferDesc.RefreshRate.Denominator = 1U;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1U;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2U;
	sd.OutputWindow = window;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	constexpr D3D_FEATURE_LEVEL levels[2]
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0
	};

	ID3D11Device* device{ nullptr };
	ID3D11DeviceContext* device_context{ nullptr };
	IDXGISwapChain* swap_chain{ nullptr };
	ID3D11RenderTargetView* render_target_view{ nullptr };
	D3D_FEATURE_LEVEL level{};

	// create d3d device
	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0U,
		levels,
		2U,
		D3D11_SDK_VERSION,
		&sd,
		&swap_chain,
		&device,
		&level,
		&device_context
	);

	ID3D11Texture2D* back_buffer{ nullptr };
	swap_chain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));

	if (back_buffer)
	{
		device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
		back_buffer->Release();
	}
	else
	{
		return 1;
	}

	ShowWindow(window, cmd_show);
	UpdateWindow(window);

	// set theme
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	// init imgui
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, device_context);

	bool running = true;

	while (running)
	{
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				running = false;
			}
		}

		if (!running)
		{
			break;
		}
	}

	// clean up

	return 0;
}

/*
int main(void)
{
	// init memory class
	const auto memory = Memory{ "csgo.exe" };

	// module addresses
	const auto client = memory.GetModuleAddress("client.dll");
	std::cout << "\nclient.dll - ";
	(client) ? std::cout << "OK" : std::cout << "FAIL";
	const auto engine = memory.GetModuleAddress("engine.dll");
	std::cout << "\nengine.dll - ";
	(engine) ? std::cout << "OK" : std::cout << "FAIL";

	while (client && engine)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (GetAsyncKeyState(VK_ESCAPE))
			break;

		if (!GetAsyncKeyState(VK_LSHIFT))
			continue;
		
		const auto local_player = memory.Read<std::uintptr_t>(client + offset::dwLocalPlayer);
		const auto local_team = memory.Read<std::int32_t>(local_player + offset::m_iTeamNum);

		const auto local_eye_position = memory.Read<Vector3>(local_player + offset::m_vecOrigin) +
			memory.Read<Vector3>(local_player + offset::m_vecViewOffset);

		const auto client_state = memory.Read<std::uintptr_t>(engine + offset::dwClientState);

		const auto local_player_id = memory.Read<std::int32_t>(client_state + offset::dwClientState_GetLocalPlayer);

		const auto view_angles = memory.Read<Vector3>(client_state + offset::dwClientState_ViewAngles);
		const auto aim_punch = memory.Read<Vector3>(local_player + offset::m_aimPunchAngle) * 2;

		// fov
		auto best_fov = 5.f;
		auto best_angle = Vector3{};
		for (auto i = 0; i <= 32; ++i)
		{
			const auto& player = memory.Read<std::uintptr_t>(client + offset::dwEntityList + i * 0x10);

			// entity checks
			if (memory.Read<std::int32_t>(player + offset::m_iTeamNum) == local_team)
				continue;

			if (memory.Read<bool>(player + offset::m_bDormant))
				continue;

			if (memory.Read<std::int32_t>(player + offset::m_lifeState))
				continue;

			if (memory.Read<bool>(player + offset::m_bSpottedByMask) & (1 << local_player_id))
			{
				const auto bone_matrix = memory.Read<std::uintptr_t>(player + offset::m_dwBoneMatrix);

				// pos of player in 3D
				// 8 is the bone index
				const auto player_head_position = Vector3{
					memory.Read<float>(bone_matrix + 0x30 * 8 + 0x0C),
					memory.Read<float>(bone_matrix + 0x30 * 8 + 0x1C),
					memory.Read<float>(bone_matrix + 0x30 * 8 + 0x2C)
				};

				const auto angle = CalculateAngle(
					local_eye_position,
					player_head_position,
					view_angles + aim_punch
				);

				const auto fov = std::hypot(angle.x, angle.y);

				if (fov < best_fov)
				{
					best_fov = fov;
					best_angle = angle;
				}
			}
		}

		// if we have a best angle
		// do aim
		if (!best_angle.IsZero())
			memory.Write<Vector3>(client_state + offset::dwClientState_ViewAngles, view_angles + best_angle / 1.f); // smoothing
	}
	
	std::cout << std::endl;
	std::cout << "Exiting application";
	for (std::size_t i = 0; i < 5; i++)
	{
		std::cout << ".";
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

	}
	std::cout << std::endl;

	return 0;
}
*/