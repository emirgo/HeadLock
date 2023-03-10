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

struct ViewMatrix
{
	ViewMatrix() noexcept : data() {}

	float* operator[](int index) noexcept
	{
		return data[index];
	}

	const float* operator[](int index) const noexcept
	{
		return data[index];
	}

	float data[4][4];
};

static bool world_to_screen(const Vector3& world, Vector3& screen, const ViewMatrix& vm) noexcept
{
    float w = vm[3][0] * world.x + vm[3][1] * world.y + vm[3][2] * world.z + vm[3][3];

    if (w < 0.001f) 
	{
        return false;
    }

    const float x = world.x * vm[0][0] + world.y * vm[0][1] + world.z * vm[0][2] + vm[0][3];
    const float y = world.x * vm[1][0] + world.y * vm[1][1] + world.z * vm[1][2] + vm[1][3];

    w = 1.f / w;
    float nx = x * w;
    float ny = y * w;

    const ImVec2 size = ImGui::GetIO().DisplaySize;

    screen.x = (size.x * 0.5f * nx) + (nx + size.x * 0.5f);
    screen.y = -(size.y * 0.5f * ny) + (ny + size.y * 0.5f);

    return true;
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

	// init memory class
	const auto memory = Memory{ "csgo.exe" };

	// module addresses
	const auto client = memory.GetModuleAddress("client.dll");
	const auto engine = memory.GetModuleAddress("engine.dll");
	std::uintptr_t local_player = NULL;

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

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// ImGui::GetBackgroundDrawList()->AddCircleFilled({ 500, 500 }, 10.f, ImColor(1.0f, 0.f, 0.f));
		// render

		if (!client || !engine)
		{
			ImGui::GetBackgroundDrawList()->AddText({ 25, 50 }, ImColor{ 1.0f, 0.f, 0.f }, "ERROR: Game not found!");
		}

		local_player = memory.Read<std::uintptr_t>(client + offset::dwLocalPlayer);
		
		if (local_player)
		{
			const auto local_team = memory.Read<std::int32_t>(local_player + offset::m_iTeamNum);
			const auto view_matrix = memory.Read<ViewMatrix>(client + offset::dwViewMatrix);

			for (int i = 0; i <= 32; ++i)
			{
				const auto& player = memory.Read<std::uintptr_t>(client + offset::dwEntityList + i * 0x10);

				if (memory.Read<bool>(player + offset::m_bDormant))
				{
					continue;
				}

				if (memory.Read<std::int32_t>(player + offset::m_iTeamNum) == local_team)
				{
					continue;
				}

				if (memory.Read<int>(player + offset::m_lifeState))
				{
					continue;
				}

				const auto bone_matrix = memory.Read<std::uintptr_t>(player + offset::m_dwBoneMatrix);

				const auto player_head_position = Vector3 {
					memory.Read<float>(bone_matrix + 0x30 * 8 + 0x0C),
					memory.Read<float>(bone_matrix + 0x30 * 8 + 0x1C),
					memory.Read<float>(bone_matrix + 0x30 * 8 + 0x2C)
				};

				auto feet_pos = memory.Read<Vector3>(player + offset::m_vecOrigin);

				Vector3 top;
				Vector3 bottom;
				if (world_to_screen(player_head_position + Vector3{ 0, 0, 10.f }, top, view_matrix) && world_to_screen(feet_pos - Vector3{ -5.f, 0, 5.f }, bottom, view_matrix))
				{
					const float h = abs(bottom.y - top.y);
					const float w = h * 0.35f;

					ImGui::GetBackgroundDrawList()->AddRect({ top.x - w, top.y }, { top.x + w, bottom.y }, ImColor(0.f, 1.f, 0.f));
				}
			}
		}
		else
		{
			ImGui::GetBackgroundDrawList()->AddText({ 1920/2-50, 1080-25 }, ImColor(1.0f, 0.f, 0.f), "Waiting for game");
		}

		ImGui::Render();

		constexpr float color[4]{ 0.f, 0.f, 0.f, 0.f };
		device_context->OMSetRenderTargets(1U, &render_target_view, nullptr);
		device_context->ClearRenderTargetView(render_target_view, color);

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		swap_chain->Present(1U, 0U);
	}

	// clean up and closing
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();

	if (swap_chain)
	{
		swap_chain->Release();
	}

	if (device_context)
	{
		device_context->Release();
	}
	
	if (device)
	{
		device->Release();
	}
	
	if (render_target_view)
	{
		render_target_view->Release();
	}

	DestroyWindow(window);
	UnregisterClassW(wc.lpszClassName, wc.hInstance);

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