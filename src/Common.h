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

struct matrix3f
{
	matrix3f()
	{
		mat[0] = 1.0f; mat[1] = 0.0f; mat[2] = 0.0f;
		mat[3] = 0.0f; mat[4] = 1.0f; mat[5] = 0.0f;
		mat[6] = 0.0f; mat[7] = 0.0f; mat[8] = 1.0f;
	}

	matrix3f(float* v)
	{
		mat[0] = v[0]; mat[1] = v[1]; mat[2] = v[2];
		mat[3] = v[3]; mat[4] = v[4]; mat[5] = v[5];
		mat[6] = v[6]; mat[7] = v[7]; mat[8] = v[8];
	}

	matrix3f transpose() const
	{
		matrix3f ret;

		ret[0] = mat[0]; ret[3] = mat[1]; ret[6] = mat[2];
		ret[1] = mat[3]; ret[4] = mat[4]; ret[7] = mat[5];
		ret[2] = mat[6]; ret[5] = mat[7]; ret[8] = mat[8];

		return ret;
	}

	float3 operator*(const float3 & v) const 
	{
		float3 ret;
		
		ret.x = mat[0] * v.x + mat[3] * v.y + mat[6] * v.z;
		ret.y = mat[1] * v.x + mat[4] * v.y + mat[7] * v.z;
		ret.z = mat[2] * v.x + mat[5] * v.y + mat[8] * v.z;

		return ret;
	}

	float operator[] (uint32_t idx) const
	{
		assert(idx < 9);

		return mat[idx];
	}

	float& operator[] (uint32_t idx)
	{
		assert(idx < 9);

		return mat[idx];
	}

	float mat[9];
};
