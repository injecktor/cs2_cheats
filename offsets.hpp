#include <windows.h>
namespace offsets {
	namespace entity_handle {
		enum : DWORD64 {
			entity = 0x520,
			validation = 0x10
		};
	}
	namespace entity {
		enum : DWORD64 {
			offset_1 = 0xb00,
			offset_2 = 0x30,
			hp = 0x2c8,
			armor = 0x1a0c,
			pos_x = 0x58,
			pos_z = 0x5c,
			pos_y = 0x60,
			is_dead = 0x878,
			area_string = 0xe10,
			team = 0x334
		};
	}
}