#include "Memory.h"
#include "Offset.h"
#include "Vector.h"
#include <thread>

int main(void)
{
	// init memory class
	const auto memory = Memory{ "csgo.exe" };

	// module addresses
	const auto client = memory.GetModuleAddress("client.dll");
	const auto engine = memory.GetModuleAddress("engine.dll");

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (!GetAsyncKeyState(VK_LSHIFT))
			continue;
		
		const auto& local_player = memory.Read<std::uintptr_t>(client + offset::dwLocalPlayer);
		const auto& local_team = memory.Read<std::int32_t>(local_player + offset::m_iTeamNum);

		const auto local_eye_position = memory.Read<Vector3>(local_player + offset::m_vecOrigin) +
			memory.Read<Vector3>(local_player + offset::m_vecViewOffset);

		const auto& client_state = memory.Read<std::uintptr_t>(engine + offset::dwClientState);

		const auto& view_angles = memory.Read<Vector3>(client_state + offset::dwClientState_ViewAngles);

		const auto& aim_punch = memory.Read<Vector3>(local_player + offset::m_aimPunchAngle) * 2;


	}

	return 0;
}