#pragma once
// ac_client.exe + 109B74
#include <vector>

struct Vector3 {
	float x, y, z;
};

class entity {
public:
	Vector3 pos_head; //0x0004
	char pad_0010[36]; //0x0010
	Vector3 pos_body; //0x0034
	Vector3 angle; //0x0040
	char pad_004C[172]; //0x004C
	int32_t health; //0x00F8
	char pad_00FC[1884]; //0x00FC

	virtual void Function0();
	virtual void Function1();
	virtual void Function2();
	virtual void Function3();
	virtual void Function4();
	virtual void Function5();
	virtual void Function6();
	virtual void Function7();
	virtual void Function8();
	virtual void Function9();
}; //Size: 0x0858