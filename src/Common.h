#pragma once

#include <stdint.h>
#include <assert.h>

struct int2
{
	int2() : x(0), y(0) {};
	int2(uint32_t x, uint32_t y) : x(x), y(y) {}
	int2(int32_t x, int32_t y) : x(x), y(y) {}

	int2 operator+(int2 const& v) const { return int2(x + v.x, y + v.y); }
	int2 operator-(int2 const& v) const { return int2(x - v.x, y - v.y); }

	int32_t x;
	int32_t y;
};

struct uint2
{
	uint2() : x(0), y(0) {};
	uint2(int32_t x, int32_t y) : x(x), y(y) {}

	uint2 operator+(int2 const& v) const { return uint2(x + v.x, y + v.y); }
	uint2 operator-(uint2 const& v) const { return uint2(x - v.x, y - v.y); }

	uint32_t x;
	uint32_t y;
};

struct float2
{
	float2() : x(0.0f), y(0.0f) {};
	float2(float x, float y) : x(x), y(y) {}

	float2 operator+(float2 const& v) const { return float2(x + v.x, y + v.y); }
	float2 operator-(float2 const& v) const { return float2(x - v.x, y - v.y); }

	float x;
	float y;
};

struct float3
{
	float3() : x(0.0f), y(0.0f), z(0.0f) {};
	float3(float x, float y, float z) : x(x), y(y), z(z) {}

	float3 operator+(float3 const& v) const { return float3(x + v.x, y + v.y, z + v.z); }
	float3 operator-(float3 const& v) const { return float3(x - v.x, y - v.y, z - v.z); }

	float x;
	float y;
	float z;
};
