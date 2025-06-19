#pragma once

namespace Offsets
{
    // SE: 2F4C910
    // AE: ???
    static constexpr auto WorldToCamMatrix = RELOCATION_ID(519579, 406126);
    
    // SE: 2F4DED0
    // AE: ???
    static constexpr auto ViewPort = RELOCATION_ID(519618, 406160);
    
    // Raycast (from SmoothCam)
    static constexpr auto CameraCaster = RELOCATION_ID(32270, 33007);
    static constexpr auto GetNiAVObject = RELOCATION_ID(76160, 77988);
    
    // SE: 605f70
    // AE: 62E390
	static constexpr auto DismountActor = RELOCATION_ID(36882, 37906);
	typedef __int64(__fastcall RE::Actor::* DismountActor_func)() const;

	// SE: 1D1530
	// 1.6.640: 1DE910
	static constexpr auto HKPCollidableToNiAVObject = RELOCATION_ID(15644, 15870);
	typedef RE::NiAVObject*(__fastcall RE::hkpCollidable::* HKPCollidableToNiAVObject_func)() const;
}

