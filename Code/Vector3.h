#ifndef RAYTRACER_VECTOR3_H
#define RAYTRACER_VECTOR3_H

#include <algorithm>
#include <cmath>
#include <ostream>
#include <cassert>


struct Vector3 {

	Vector3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : x(_x), y(_y), z(_z) {}	//TODO: should this be explicit?

	explicit Vector3(float f) {
		x = y = z = f;
	}

	Vector3(const Vector3&) = default;
	Vector3& operator=(const Vector3&) = default;
	~Vector3() = default;

	float& operator[](size_t idx) {
		assert(idx <= 2);
		return data[idx];
	}
	float operator[](size_t idx) const {
		assert(idx <= 2);
		return data[idx];
	}

	[[nodiscard]] size_t size() const {	//TODO: will this causes issues due to name size?
		return 3;
	}

	Vector3 operator+=(Vector3 v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	Vector3 operator-=(Vector3 v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	Vector3 operator*=(Vector3 v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
	Vector3 operator/=(Vector3 v) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	Vector3 operator+=(float s) {
		x += s;
		y += s;
		z += s;
		return *this;
	}
	Vector3 operator-=(float s) {
		x -= s;
		y -= s;
		z -= s;
		return *this;
	}
	Vector3 operator*=(float s) {
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}
	Vector3 operator/=(float s) {
		x /= s;
		y /= s;
		z /= s;
		return *this;
	}

	Vector3 operator+(Vector3 v) const {
		return Vector3(x + v.x, y + v.y, z + v.z);
	}
	Vector3 operator-(Vector3 v) const {
		return Vector3(x - v.x, y - v.y, z - v.z);
	}
	Vector3 operator*(Vector3 v) const {
		return Vector3(x * v.x, y * v.y, z * v.z);
	}
	Vector3 operator/(Vector3 v) const {
		return Vector3(x / v.x, y / v.y, z / v.z);
	}

	Vector3 operator+(float s) const {
		return Vector3(x + s, y + s, z + s);
	}
	Vector3 operator-(float s) const {
		return Vector3(x - s, y - s, z - s);
	}
	Vector3 operator*(float s) const {
		return Vector3(x * s, y * s, z * s);
	}
	Vector3 operator/(float s) const {
		return Vector3(x / s, y / s, z / s);
	}

	bool operator==(Vector3 v) const {
		return x == v.x && y == v.y && z == v.z;
	}
	bool operator!=(Vector3 v) const {
		return x != v.x || y != v.y || z != v.z;
	}
	// Negation
	Vector3 operator-() const {
		return Vector3(-x, -y, -z);
	}
	/*
	// Absolute value
	Vector3 abs() const {
		return Vector3(std::abs(x), std::abs(y), std::abs(z));
	}
	// Negation
	Vec3 operator-() const {
		return Vec3(-x, -y, -z);
	}
	/// Are all members real numbers?
	bool valid() const {
		return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
	}*/


	Vector3 normalize() {
		float n = norm();
		x /= n;
		y /= n;
		z /= n;
		return *this;
	}


	/*
	/// Return unit length vec in the same direction
	Vector3 unit() const {
		float n = norm();
		return Vector3(x / n, y / n, z / n);
	}

	float norm_squared() const {
		return x * x + y * y + z * z;
	}*/
	[[nodiscard]] float norm() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	[[nodiscard]] std::string to_string() const {
		return "{" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + "}";
	}

	bool operator<(Vector3 r) const {
		if (x == r.x) {
			if (y == r.y) {
				return z < r.z;
			}
			return y < r.y;
		}
		return x < r.x;
	}

/*
	/// Returns first two components
	Vec2 xy() const {
		return Vec2(x, y);
	}

	/// Make sure all components are in the range [min,max) with floating point mod logic
	Vec3 range(float min, float max) const {
		if (!valid()) return Vec3();
		Vec3 r = *this;
		float range = max - min;
		while (r.x < min) r.x += range;
		while (r.x >= max) r.x -= range;
		while (r.y < min) r.y += range;
		while (r.y >= max) r.y -= range;
		while (r.z < min) r.z += range;
		while (r.z >= max) r.z -= range;
		return r;
	}
*/
	union {
		struct {
			float x;
			float y;
			float z;
		};
		float data[3] = {};
	};
};

// 3D dot product
inline float dotProduct(Vector3 l, Vector3 r) {
	return l.x * r.x + l.y * r.y + l.z * r.z;
}

// 3D cross product
inline Vector3 crossProduct(Vector3 l, Vector3 r) {
	return Vector3(l.y * r.z - l.z * r.y, l.z * r.x - l.x * r.z, l.x * r.y - l.y * r.x);
}

inline std::ostream& operator<<(std::ostream& out, Vector3 v) {
	out << "{" << v.x << "," << v.y << "," << v.z << "}";
	return out;
}

inline void calculateTangentSpace(const Vector3& normal, Vector3& tangent, Vector3& bitangent) {
	// Choose an arbitrary vector perpendicular to the normal
	Vector3 arbitraryVector = (fabs(normal.x) > 0.9f) ? Vector3(0.0f, 1.0f, 0.0f) : Vector3(1.0f, 0.0f, 0.0f);

	tangent = crossProduct(arbitraryVector, normal).normalize();
	bitangent = crossProduct(normal, tangent).normalize();
}

inline Vector3 lerp(const Vector3& a, const Vector3& b, float t) {
	return  a * (1.0f - t) + b * t;
}


#endif //RAYTRACER_VECTOR3_H

/*
inline Vector3 operator+(float s, Vector3 v) {
	return Vector3(v.x + s, v.y + s, v.z + s);
}
inline Vector3 operator-(float s, Vector3 v) {
	return Vector3(v.x - s, v.y - s, v.z - s);
}
inline Vector3 operator*(float s, Vector3 v) {
	return Vector3(v.x * s, v.y * s, v.z * s);
}
inline Vector3 operator/(float s, Vector3 v) {
	return Vector3(s / v.x, s / v.y, s / v.z);
}*/
/*
/// Take minimum of each component
inline Vec3 hmin(Vec3 l, Vec3 r) {
	return Vec3(std::min(l.x, r.x), std::min(l.y, r.y), std::min(l.z, r.z));
}

/// Take maximum of each component
inline Vec3 hmax(Vec3 l, Vec3 r) {
	return Vec3(std::max(l.x, r.x), std::max(l.y, r.y), std::max(l.z, r.z));
}*/


/*
inline std::string to_string(Vector3 const &v) {
	return "{" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + "}";
}*/


/*
inline bool operator<(Vector3 l, Vector3 r) {
	if (l.x == r.x) {
		if (l.y == r.y) {
			return l.z < r.z;
		}
		return l.y < r.y;
	}
	return l.x < r.x;
}
*/


