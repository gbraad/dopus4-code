#include "config.h"

USHORT pageflip_data1[5] = { 0x4000, 0xa000, 0x9000, 0x8800, 0xfc00 };

USHORT pageflip_data2[3] = { 0x4000, 0x6000, 0x7000 };

USHORT dropbox_data[] =
{
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
     0x0000,0x0000,0x0000,0x0000,0x3f3f,0x1f3f,0x0000,0x0000,
     0x3f80,0x0000,0x31b1,0xb1b1,0x8000,0x0000,0x3f80,0x0000,
     0x31b1,0xb1b1,0x8000,0x0003,0xf800,0x0000,0x31bf,0x31bf,
     0x0000,0x0007,0xf800,0x0000,0x31b6,0x31b0,0x0000,0x0e0f,
     0x8000,0x0000,0x31b3,0x31b0,0x0000,0x0e0f,0x0000,0x0000,
     0x3f31,0x9f30,0x0000,0x1e0e,0x0000,0x0000,0x0000,0x0000,
     0x0000,0x1e0e,0x0000,0x0000,0x0000,0x0000,0x00f0,0x1c06,
     0x0f00,0x0000,0x0000,0x0000,0x00f0,0x1c06,0x0700,0x0000,
     0x0000,0x0000,0x01e0,0x1c03,0x0000,0x0000,0x0000,0x000f,
     0xe1e0,0x0c00,0x0010,0x0000,0x0000,0x01ff,0xf1c0,0x0c00,
     0x0000,0x0000,0x0000,0x01fe,0x01c0,0x0e00,0x0000,0x0000,
     0x0000,0x0fc0,0x00c0,0x0e00,0x00e0,0x0000,0x0000,0x0f80,
     0x00c0,0x0207,0x83e0,0x0000,0x007e,0x0e00,0x3060,0x0207,
     0xffe0,0x0000,0x007e,0x0e00,0x3860,0x0383,0x0f80,0x0000,
     0x07f0,0x0e00,0x7860,0x0383,0x0000,0x0000,0x1ff0,0x0e03,
     0xf060,0x0381,0x0000,0x0000,0x1f00,0x060f,0xf060,0x3f80,
     0x0000,0x0000,0x3c00,0x0606,0x0070,0x7f80,0x0000,0x0000,
     0x3c03,0x0300,0x0070,0x3800,0x0000,0x0000,0x3803,0x8300,
     0x0030,0x0000,0x0000,0x0000,0x3801,0x8300,0x0000,0x0000,
     0x0000,0x0000,0x3801,0x8100,0x0000,0x0000,0x0000,0x0000,
     0x3801,0xc100,0x0000,0x0000,0x0000,0x0000,0x3801,0xc1c0,
     0x0000,0x0000,0x0000,0x0000,0x3801,0xc1c0,0x0000,0x0000,
     0x0000,0x0000,0x3801,0xc1c0,0x0000,0x0000,0x0000,0x0000,
     0x3801,0xc1c0,0x0000,0x0000,0x0000,0x0000,0x1807,0xc0c0,
     0x0000,0x007e,0x3e61,0x8000,0x181f,0xc000,0x0000,0x0063,
     0x6333,0x0000,0x0c1f,0x0000,0x0000,0x0063,0x631e,0x0000,
     0x0c00,0x0000,0x0000,0x007e,0x630c,0x0000,0x0000,0x0000,
     0x0000,0x0063,0x631e,0x0000,0x0000,0x0000,0x0000,0x0063,
     0x6333,0x0000,0x0000,0x0000,0x0000,0x007e,0x3e61,0x8000,
     0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
     0x0000,0x0000,0x0000,0x0000,
     0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
     0x0000,0x0000,0x0000,0x0000,0x3f3f,0x1f3f,0x0000,0x0000,
     0x0000,0x0000,0x31b1,0xb1b1,0x8000,0x0000,0x0000,0x0000,
     0x31b1,0xb1b1,0x8000,0x0000,0x07f0,0x0000,0x31bf,0x31bf,
     0x0000,0x0000,0x07f0,0x0000,0x31b6,0x31b0,0x0000,0x0000,
     0x7ffc,0x0000,0x31b3,0x31b0,0x0000,0x0000,0xfffe,0x0000,
     0x3f31,0x9f30,0x0000,0x01c1,0xf03c,0x0000,0x0000,0x0000,
     0x0000,0x01c1,0xf03c,0x0000,0x0000,0x0000,0x0000,0x03e1,
     0xf000,0x0000,0x0000,0x0000,0x0000,0x03e1,0xf800,0x0000,
     0x0000,0x0000,0x001e,0x03e0,0xffe0,0x0000,0x0000,0x0000,
     0x001e,0x03e0,0xffe0,0x0000,0x0000,0x0000,0x003e,0x03f0,
     0x7ffc,0x0000,0x0000,0x0001,0xfc3f,0x01f0,0x0ffe,0x0000,
     0x0000,0x003f,0xfe3f,0x01f0,0x001f,0x0000,0x0000,0x003f,
     0xfe3f,0x01f0,0x001f,0x0000,0x0000,0x01ff,0x8f9f,0x01f0,
     0x001f,0x0000,0x0000,0x01f0,0x079f,0x007c,0xf07f,0x0000,
     0x000f,0xc1f0,0x079f,0x807c,0xfffe,0x0000,0x000f,0xc1f0,
     0x079f,0x807c,0xfffe,0x0000,0x00ff,0xf9f0,0x0f9f,0x807c,
     0xfffc,0x0000,0x03ff,0xf9f8,0x7e0f,0x807c,0x3f80,0x0000,
     0x03f0,0xfcff,0xfe0f,0xc7fc,0x0000,0x0000,0x07c0,0x7cff,
     0xc00f,0xfff0,0x0000,0x0000,0x07c0,0x7cff,0xc00f,0xfff0,
     0x0000,0x0000,0x07c0,0x7cf8,0x0007,0xffe0,0x0000,0x0000,
     0x07c0,0x3cfe,0x0000,0xfc00,0x0000,0x0000,0x07c0,0x3e3e,
     0x0000,0x0000,0x0000,0x0000,0x07c0,0x3e3e,0x0000,0x0000,
     0x0000,0x0000,0x07c0,0x3e3e,0x0000,0x0000,0x0000,0x0000,
     0x07c0,0x3e3f,0x0000,0x0000,0x0000,0x0000,0x07e0,0x3e3f,
     0x0000,0x007e,0x3e61,0x8000,0x07e0,0x3e3f,0x0000,0x0063,
     0x6333,0x0000,0x03e0,0xfe1e,0x0000,0x0063,0x631e,0x0000,
     0x03ff,0xfc00,0x0000,0x007e,0x630c,0x0000,0x01ff,0xf800,
     0x0000,0x0063,0x631e,0x0000,0x01ff,0xf800,0x0000,0x0063,
     0x6333,0x0000,0x003f,0x0000,0x0000,0x007e,0x3e61,0x8000,
     0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
     0x0000,0x0000,0x0000,0x0000
};
//USHORT glass_data[2][18] = 
USHORT glass_data[2][18] = 
{
	{ 0x0fe0, 0x0000, 0x701c, 0x0000, 0xc286, 0x0000, 0xc806, 0x0000, 0xc006, 0x0000, 0x701c, 0x0000, 0x0fef, 0x0000, 0x0003, 0xc000, 0x0000, 0xf000 },
	{ 0x0000, 0x0000, 0x0fe0, 0x0000, 0x3d78, 0x0000, 0x37f8, 0x0000, 0x3ff8, 0x0000, 0x0fe0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }
};

USHORT uparrow_data[6] = { 0x0600, 0x0f00, 0x1f80, 0x3fc0, 0x7fe0, 0xfff0 };

USHORT downarrow_data[6] = { 0xfff0, 0x7fe0, 0x3fc0, 0x1f80, 0x0f00, 0x0600 };