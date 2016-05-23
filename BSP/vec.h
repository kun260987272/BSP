#pragma once
#include<iostream>
//////////////////////////////////////////////////////////////////////////////
//
//  vec3.h - 3D vector
//
//////////////////////////////////////////////////////////////////////////////

struct vec3 {

	float  x;
	float  y;
	float  z;

	//
	//  --- Constructors and Destructors ---
	//

	vec3(float s = float(0.0)) :
		x(s), y(s), z(s) {}

	vec3(float x, float y, float z) :
		x(x), y(y), z(z) {}

	vec3(const vec3& v) { x = v.x;  y = v.y;  z = v.z; }

	//vec3(const vec2& v, const float f) { x = v.x;  y = v.y;  z = f; }

	//
	//  --- Indexing Operator ---
	//

	float& operator [] (int i) { return *(&x + i); }
	const float operator [] (int i) const { return *(&x + i); }

	//
	//  --- (non-modifying) Arithematic Operators ---
	//

	vec3 operator - () const  // unary minus operator
	{
		return vec3(-x, -y, -z);
	}

	vec3 operator + (const vec3& v) const
	{
		return vec3(x + v.x, y + v.y, z + v.z);
	}

	vec3 operator - (const vec3& v) const
	{
		return vec3(x - v.x, y - v.y, z - v.z);
	}

	vec3 operator * (const float s) const
	{
		return vec3(s*x, s*y, s*z);
	}

	vec3 operator * (const vec3& v) const
	{
		return vec3(x*v.x, y*v.y, z*v.z);
	}

	friend vec3 operator * (const float s, const vec3& v)
	{
		return v * s;
	}

	vec3 operator / (const float s) const {
#ifdef DEBUG
		if (std::fabs(s) < DivideByZeroTolerance) {
			std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
				<< "Division by zero" << std::endl;
			return vec3();
		}
#endif // DEBUG

		float r = float(1.0) / s;
		return *this * r;
	}

	//
	//  --- (modifying) Arithematic Operators ---
	//

	vec3& operator += (const vec3& v)
	{
		x += v.x;  y += v.y;  z += v.z;  return *this;
	}

	vec3& operator -= (const vec3& v)
	{
		x -= v.x;  y -= v.y;  z -= v.z;  return *this;
	}

	vec3& operator *= (const float s)
	{
		x *= s;  y *= s;  z *= s;  return *this;
	}

	vec3& operator *= (const vec3& v)
	{
		x *= v.x;  y *= v.y;  z *= v.z;  return *this;
	}

	vec3& operator /= (const float s) {
#ifdef DEBUG
		if (std::fabs(s) < DivideByZeroTolerance) {
			std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
				<< "Division by zero" << std::endl;
		}
#endif // DEBUG

		float r = float(1.0) / s;
		*this *= r;

		return *this;
	}

	//
	//  --- Insertion and Extraction Operators ---
	//

	friend std::ostream& operator << (std::ostream& os, const vec3& v) {
		return os << "( " << v.x << ", " << v.y << ", " << v.z << " )";
	}

	friend std::istream& operator >> (std::istream& is, vec3& v)
	{
		return is >> v.x >> v.y >> v.z;
	}

	//
	//  --- Conversion Operators ---
	//

	operator const float* () const
	{
		return static_cast<const float*>(&x);
	}

	operator float* ()
	{
		return static_cast<float*>(&x);
	}
};

//----------------------------------------------------------------------------
//
//  Non-class vec3 Methods
//

inline
float dot(const vec3& u, const vec3& v) {
	return u.x*v.x + u.y*v.y + u.z*v.z;
}

inline
float length(const vec3& v) {
	return std::sqrt(dot(v, v));
}

inline
vec3 normalize(const vec3& v) {
	return v / length(v);
}

inline
vec3 cross(const vec3& a, const vec3& b)
{
	return vec3(a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);
}

