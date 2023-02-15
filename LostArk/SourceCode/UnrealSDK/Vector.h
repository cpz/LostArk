#ifndef VECTOR_H__
#define VECTOR_H__

#include "..\Include.h"

#define vec3_origin Vector ( 0, 0, 0 )

class Vector
{
public:
	float x, y, z;

	Vector(void)
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	Vector(float X, float Y, float Z)
	{
		x = X;
		y = Y;
		z = Z;
	}

	Vector(float XYZ)
	{
		x = XYZ;
		y = XYZ;
		z = XYZ;
	}

	inline void Init(float X = 0.0f, float Y = 0.0f, float Z = 0.0f)
	{
		x = X;
		y = Y;
		z = Z;
	}

	inline float* Base(void)
	{
		return (float*)this;
	}

	inline const float* Base(void) const
	{
		return (const float*)this;
	}

	inline float operator [] (int i) const
	{
		return ((float*)this)[i];
	}

	inline float& operator [] (int i)
	{
		return ((float*)this)[i];
	}

	inline Vector& operator += (const Vector& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}

	inline Vector& operator -= (const Vector& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}

	inline Vector& operator *= (const Vector& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;

		return *this;
	}

	inline Vector& operator /= (const Vector& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;

		return *this;
	}

	inline Vector& operator += (float v)
	{
		x += v;
		y += v;
		z += v;

		return *this;
	}

	inline Vector& operator -= (float v)
	{
		x -= v;
		y -= v;
		z -= v;

		return *this;
	}

	inline Vector& operator *= (float v)
	{
		x *= v;
		y *= v;
		z *= v;

		return *this;
	}

	inline Vector& operator /= (float v)
	{
		x /= v;
		y /= v;
		z /= v;

		return *this;
	}

	inline Vector operator - (void) const
	{
		return Vector(-x, -y, -z);
	}

	inline Vector operator - (const Vector& v) const
	{
		return Vector(x - v.x, y - v.y, z - v.z);
	}

	inline Vector operator + (const Vector& v) const
	{
		return Vector(x + v.x, y + v.y, z + v.z);
	}

	inline Vector operator * (const Vector& v) const
	{
		return Vector(x * v.x, y * v.y, z * v.z);
	}

	inline Vector operator / (const Vector& v) const
	{
		return Vector(x / v.x, y / v.y, z / v.z);
	}

	inline float Length(void) const
	{
		return sqrt(x * x + y * y + z * z);
	}

	inline float LengthSqr(void) const
	{
		return (x * x + y * y + z * z);
	}

	inline float Length2D(void) const
	{
		return sqrt(x * x + y * y);
	}

	inline float Length2DSqr(void) const
	{
		return (x * x + y * y);
	}

	inline bool IsZero(float tolerance = 0.01f) const
	{
		return (x > -tolerance && x < tolerance && y > -tolerance && y < tolerance && z > -tolerance && z < tolerance);
	}

	inline float DistTo(const Vector& v) const
	{
		return (*this - v).Length();
	}

	inline float DistToSqr(const Vector& v) const
	{
		return (*this - v).LengthSqr();
	}

	inline float DotProduct(const Vector& v) const
	{
		return (x * v.x + y * v.y + z * v.z);
	}

	inline Vector CrossProduct(const Vector& v) const
	{
		return Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

inline void CrossProduct(const float* a, const float* b, float* r)
{
	r[0] = a[1] * b[2] - a[2] * b[1];
	r[1] = a[2] * b[0] - a[0] * b[2];
	r[2] = a[0] * b[1] - a[1] * b[0];
}

inline Vector operator * (float f, const Vector& v)
{
	return v * f;
}

inline float VectorNormalize(Vector& v)
{
	float flLength = v.Length();

	if (flLength != 0.0f)
		v /= flLength;
	else
		v.Init();

	return flLength;
}

inline float VectorNormalize(float* v)
{
	return VectorNormalize(*(Vector*)v);
}

class VectorAligned : public Vector
{
public:
	inline VectorAligned(void)
	{

	}

	inline VectorAligned(float X, float Y, float Z)
	{
		Init(X, Y, Z);
	}

public:
	explicit VectorAligned(const Vector& v)
	{
		Init(v.x, v.y, v.z);
	}

	VectorAligned& operator = (const Vector& v)
	{
		Init(v.x, v.y, v.z);
		return *this;
	}

	float w;
};

class Vector4
{
public:
	Vector4::Vector4()
	{
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
		this->w = 0.0f;
	}

	Vector4::Vector4(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	Vector4::Vector4(const Vector4& v)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
		this->w = v.w;
	}

	Vector4::Vector4(const float* v)
	{
		this->x = v[0];
		this->y = v[1];
		this->z = v[2];
		this->w = v[3];
	}

	void Vector4::Set(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	bool Vector4::operator==(const Vector4& v) const
	{
		return (this->x == v.x &&
			this->y == v.y &&
			this->z == v.z &&
			this->w == v.w);
	}

	bool Vector4::operator!=(const Vector4& v) const
	{
		return (this->x != v.x ||
			this->y != v.y ||
			this->z != v.z ||
			this->w != v.w);
	}

	float& Vector4::operator[ ](unsigned int i)
	{
		return ((float*)this)[i];
	}

	float Vector4::operator[ ](unsigned int i) const
	{
		return ((float*)this)[i];
	}

	Vector4& Vector4::operator=(const Vector4& v)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
		this->w = v.w;

		return *this;
	}

	Vector4& Vector4::operator=(const float* v)
	{
		this->x = v[0];
		this->y = v[1];
		this->z = v[2];
		this->w = v[3];

		return *this;
	}

	Vector4& Vector4::operator+=(const Vector4& v)
	{
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
		this->w += v.w;

		return *this;
	}

	Vector4& Vector4::operator-=(const Vector4& v)
	{
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;
		this->w -= v.w;

		return *this;
	}

	Vector4& Vector4::operator*=(const Vector4& v)
	{
		this->x *= v.x;
		this->y *= v.y;
		this->z *= v.z;
		this->w *= v.w;

		return *this;
	}

	Vector4& Vector4::operator/=(const Vector4& v)
	{
		this->x /= v.x;
		this->y /= v.y;
		this->z /= v.z;
		this->w /= v.w;

		return *this;
	}

	Vector4& Vector4::operator+=(float v)
	{
		this->x += v;
		this->y += v;
		this->z += v;
		this->w += v;

		return *this;
	}

	Vector4& Vector4::operator-=(float v)
	{
		this->x -= v;
		this->y -= v;
		this->z -= v;
		this->w -= v;

		return *this;
	}

	Vector4& Vector4::operator*=(float v)
	{
		this->x *= v;
		this->y *= v;
		this->z *= v;
		this->w *= v;

		return *this;
	}

	Vector4& Vector4::operator/=(float v)
	{
		this->x /= v;
		this->y /= v;
		this->z /= v;
		this->w /= v;

		return *this;
	}

	Vector4 Vector4::operator+(const Vector4& v) const
	{
		return Vector4(this->x + v.x, this->y + v.y, this->z + v.z, this->w + v.w);
	}

	Vector4 Vector4::operator-(const Vector4& v) const
	{
		return Vector4(this->x - v.x, this->y - v.y, this->z - v.z, this->w - v.w);
	}

	Vector4 Vector4::operator*(const Vector4& v) const
	{
		return Vector4(this->x * v.x, this->y * v.y, this->z * v.z, this->w * v.w);
	}

	Vector4 Vector4::operator/(const Vector4& v) const
	{
		return Vector4(this->x / v.x, this->y / v.y, this->z / v.z, this->w / v.w);
	}

	Vector4 Vector4::operator+(float v) const
	{
		return Vector4(this->x + v, this->y + v, this->z + v, this->w + v);
	}

	Vector4 Vector4::operator-(float v) const
	{
		return Vector4(this->x - v, this->y - v, this->z - v, this->w - v);
	}

	Vector4 Vector4::operator*(float v) const
	{
		return Vector4(this->x * v, this->y * v, this->z * v, this->w * v);
	}

	Vector4 Vector4::operator/(float v) const
	{
		return Vector4(this->x / v, this->y / v, this->z / v, this->w / v);
	}

	float Vector4::Dot(const Vector4& v) const
	{
		return (this->x * v.x + this->y * v.y + this->z * v.z + this->w * v.w);
	}

	float Vector4::Length() const
	{
		return sqrt(this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w);
	}

	float Vector4::LengthSqr() const
	{
		return (this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w);
	}

	float Vector4::DistTo(const Vector4& v) const
	{
		return (*this - v).Length();
	}

	float Vector4::DistToSqr(const Vector4& v) const
	{
		return (*this - v).LengthSqr();
	}

	bool Vector4::IsZero() const
	{
		return (this->x > -0.01f && this->x < 0.01f &&
			this->y > -0.01f && this->y < 0.01f &&
			this->z > -0.01f && this->z < 0.01f &&
			this->w > -0.01f && this->w < 0.01f);
	}

	Vector Vector4::ToVector() const
	{
		return Vector(x, y, z);
	}

public:
	float		x;
	float		y;
	float		z;
	float		w;
};

__forceinline Vector4 operator*(float f, const Vector4& v)
{
	return v * f;
}

static Vector vec3_zero(0.0f, 0.0f, 0.0f);
static Vector4 vec4_zero(0.0f, 0.0f, 0.0f, 0.0f);
#endif // VECTOR_H