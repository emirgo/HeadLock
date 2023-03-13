#include <iostream>
#include "Memory.h"
#include "Offset.h"
#include "Vector.h"
#include <thread>
#include <math.h>
#include <cmath>

int main(void)
{
	// init memory class
	const auto memory = Memory{ "csgo.exe" };

	// module addresses
	const auto client = memory.GetModuleAddress("client.dll");
	std::cout << "\nclient.dll - " << (client) ? "OK" : "FAIL";
	const auto engine = memory.GetModuleAddress("engine.dll");
	std::cout << "\nengine.dll - " << (client) ? "OK" : "FAIL";

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (!GetAsyncKeyState(VK_RBUTTON))
			continue;
		
		const auto& local_player = memory.Read<std::uintptr_t>(client + offset::dwLocalPlayer);
		const auto& local_team = memory.Read<std::int32_t>(local_player + offset::m_iTeamNum);

		const auto local_eye_position = memory.Read<Vector3>(local_player + offset::m_vecOrigin) +
			memory.Read<Vector3>(local_player + offset::m_vecViewOffset);

		const auto& client_state = memory.Read<std::uintptr_t>(engine + offset::dwClientState);

		const auto& view_angles = memory.Read<Vector3>(client_state + offset::dwClientState_ViewAngles);

		const auto& aim_punch = memory.Read<Vector3>(local_player + offset::m_aimPunchAngle) * 2;

		// aimbot fov
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

			if (!memory.Read<std::int32_t>(player + offset::m_iHealth))
				continue;

			if (!memory.Read<bool>(player + offset::m_bSpottedByMask))
				continue;

			const auto bone_matrix = memory.Read<std::uintptr_t>(player + offset::m_dwBoneMatrix);

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

		// if we have a best angle
		// do aimbot
		if (!best_angle.IsZero())
			memory.Write<Vector3>(client_state + offset::dwClientState_ViewAngles, view_angles + best_angle / 3.f);
	}

	return 0;
}