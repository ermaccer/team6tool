#pragma once
// ms
#include <Windows.h>
enum eFlags {
	DDSD_CAPS = 0x1,
	DDSD_HEIGHT = 0x2,
	DDSD_WIDTH = 0x4,
	DDSD_PITCH = 0x8,
	DDSD_PIXELFORMAT = 0x1000,
	DDSD_MIPMAPCOUNT = 0x20000,
	DDSD_LINEARSIZE = 0x80000,
	DDSD_DEPTH = 0x800000
};

enum ePixelFormat {
	DDPF_ALPHAPIXELS = 0x1,
	DDPF_ALPHA = 0x2,
	DDPF_FOURCC = 0x4,
	DDPF_RGB = 0x40,
	DDPF_YUV = 0x200,
	DDPF_LUMINANCE = 0x20000
};

enum eCaps {
	DDSCAPS_COMPLEX = 0x8,
	DDSCAPS_MIPMAP = 0x400000,
	DDSCAPS_TEXTURE = 0x1000
};

struct DDS_PIXELFORMAT {
	int dwSize;
	int dwFlags;
	char dwFourCC[4];
	int dwRGBBitCount;
	int dwRBitMask;
	int dwGBitMask;
	int dwBBitMask;
	int dwABitMask;
};


struct dds_header {
	int           magic;
	int           dwSize;
	int           dwFlags;
	int           dwHeight;
	int           dwWidth;
	int           dwPitchOrLinearSize;
	int           dwDepth;
	int           dwMipMapCount;
	int           dwReserved1[11] = { 0,0,0,0,0,0,0,0,0,0,0};
	DDS_PIXELFORMAT ddspf;
	int           dwCaps;
	int           dwCaps2;
	int           dwCaps3;
	int           dwCaps4;
	int           dwReserved2;
};
