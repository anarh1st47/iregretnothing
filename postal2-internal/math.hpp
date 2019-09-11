#pragma once

struct vec_3d_int {
	int x, y, z;
	vec_3d_int operator-(const vec_3d_int& r) {
		return { x - r.x, y - r.y, z - r.z };
	}
	vec_3d_int operator+(const vec_3d_int& r) {
		return vec_3d_int{ x + r.x, y + r.y, z + r.z };
	}
	void operator+=(const vec_3d_int& r) {
		x += r.x, y += r.y, z += r.z;
	}
	int dot(const vec_3d_int& v)
	{
		return (x * v.x + y * v.y + z * v.z);
	}
};

struct vec_3d {
	float x, y, z;
	vec_3d operator-(const vec_3d& r) {
		return { x - r.x, y - r.y, z - r.z };
	}
	vec_3d operator+(const vec_3d& r) {
		return { x + r.x, y + r.y, z + r.z };
	}
	void operator+=(const vec_3d& r) {
		x += r.x, y += r.y, z += r.z;
	}
	float dot(const vec_3d& v)
	{
		return (x * v.x + y * v.y + z * v.z);
	}
	vec_3d(const vec_3d_int& v) {
		x = static_cast<float>(v.x);
		y = static_cast<float>(v.y);
		z = static_cast<float>(v.z);
	};
	vec_3d(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	};
	vec_3d(){
		x = y = z = 0.f;
	};
};

namespace math {
	vec_3d world_to_screen(vec_3d pos);
}