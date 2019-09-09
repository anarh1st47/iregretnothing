#pragma once

struct vec_3d {
	float x, y, z;
	vec_3d operator-(const vec_3d& r) {
		return vec_3d{ x - r.x, y - r.y, z - r.z };
	}
	vec_3d operator+(const vec_3d& r) {
		return vec_3d{ x + r.x, y + r.y, z + r.z };
	}
	void operator+=(const vec_3d& r) {
		x += r.x, y += r.y, z += r.z;
	}
};