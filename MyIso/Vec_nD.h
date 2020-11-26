#pragma once
//#ifndef Vec_nD_H_
//#define Vec_nD_H_   

#include <math.h>

#define DOUBLE_EPS 1e-12
#define LDOUBLE_EPS 1e-6
#define LHDOUBLE_EPS 1e-5
#define	EQUALZERO(x)	(fabs((x)) < DOUBLE_EPS)
#define	LEQUALZERO(x) (fabs((x)) < LDOUBLE_EPS)
#define PI	3.141592653589793238
#define eps 1e-6


//pair
struct vec2
{
	int x;
	int y;
	vec2() {}
	vec2(int a, int b) { x = a; y = b; }
};

struct myvec3 
{
	int x;
	int y;
	int z;
	myvec3() {}
	myvec3(int a, int b, int c) { x = a; y = b; z = c; }
};

//2D point
class Vector2D
{
public:
	double x, y;

public:
	Vector2D() { x = 0; y = 0; }
	// constructions
	Vector2D(double xx, double yy) { x = xx; y = yy; }
	Vector2D(const Vector2D& v) { x = v.x; y = v.y; }


	// operator
	double	  length() { return sqrt(x*x + y*y); }
	double	  length2() { return x*x + y*y; }
	double	  normalize() 
	{ 
		double len = length();	
		if (!EQUALZERO(len)) 
		{ 
			x /= len; 
			y /= len; 
		}	
		return len; 
	}
	void Initialization()
	{
		x = 0;
		y = 0;
	}
	Vector2D& operator=(const Vector2D& v);
	Vector2D& operator+=(const Vector2D& v);
	Vector2D& operator-=(const Vector2D& v);
	Vector2D& operator*=(double u);
	Vector2D& operator/=(double u);
	//Vector2D& operator^=(const Vector2D& v);

	bool	Intersect(Vector2D v1, Vector2D v2, Vector2D v3, Vector2D v4);
	bool	Intersect(Vector2D v1, Vector2D v2);

	friend Vector2D operator+(const Vector2D& lv, const Vector2D& rv);
	friend Vector2D operator-(const Vector2D& lv, const Vector2D& rv);
	friend Vector2D operator*(const double u, const Vector2D& rv);
	friend Vector2D operator*(const Vector2D& lv, const double u);
	friend Vector2D operator/(const Vector2D& lv, const double u);
	friend double   operator*(const Vector2D& lv, const Vector2D& rv);
	friend double operator^(const Vector2D& lv, const Vector2D& rv);

	short	AtWhere(Vector2D v0, Vector2D v1);
	bool	AtRight(Vector2D v0, Vector2D v1);
	bool	AtLeft(Vector2D v0, Vector2D v1);
	bool	OnLine(Vector2D v0, Vector2D v1);
	double	GetArea(Vector2D v);


};

//3D point
class Vector3D
{
public:
	double x, y, z;

	// constructions
	Vector3D() { x = 0;	y = 0;	z = 0; }
	Vector3D(double xx, double yy, double zz) { x = xx;	y = yy;	z = zz; }
	Vector3D(const Vector3D& v) { x = v.x;	y = v.y;	z = v.z; }

	// operator
	double length() { return sqrt(x*x + y*y + z*z); }
	double length2() { return x*x + y*y + z*z; }
	double normalize()
	{
		Vector3D vn;
		double len = length();
		if (!EQUALZERO(len))
		{
			x /= len;
			y /= len;
			z /= len;
		}
		vn.x = x;
		vn.y = y;
		vn.z = z;
		return len;
	}
	void normalize(double a) 
	{
		double len = length();
		if (!EQUALZERO(len))
		{
			x = (sqrt(a)*x) / len;
			y = (sqrt(a)*y) / len;
			z = (sqrt(a)*z) / len;
		}
	}
	void Initialization() 
	{
		x = 0;
		y = 0;
		z = 0;
	}
	Vector3D& operator=(const Vector3D& v);
	Vector3D& operator+=(const Vector3D& v);
	Vector3D& operator-=(const Vector3D& v);
	Vector3D& operator*=(double u);
	Vector3D& operator/=(double u);
	Vector3D& operator^=(const Vector3D& v);

	bool operator < (const Vector3D& right) const
	{
		return x - right.x < -LHDOUBLE_EPS ||
			fabs(x - right.x) <= LHDOUBLE_EPS && y - right.y < -LHDOUBLE_EPS ||
			fabs(x - right.x) <= LHDOUBLE_EPS && fabs(y - right.y) <= LHDOUBLE_EPS && z - right.z < -LHDOUBLE_EPS;
	}

    friend Vector3D operator+(const Vector3D& lv, const Vector3D& rv);
	friend Vector3D operator-(const Vector3D& lv, const Vector3D& rv);
	friend Vector3D operator*(const double u, const Vector3D& rv);
	friend Vector3D operator*(const Vector3D& lv, const double u);
	friend Vector3D operator/(const Vector3D& lv, const double u);
	friend double   operator*(const Vector3D& lv, const Vector3D& rv);
	friend Vector3D operator^(const Vector3D& lv, const Vector3D& rv);

};

//4D point
class Vector4D 
{
public:
	double x, y, z, t;
	// constructions
	Vector4D() { x = 0;	y = 0;	z = 0; t = 0; }
	Vector4D(double xx, double yy, double zz, double tt) { x = xx;	y = yy;	z = zz; t = tt; }
	Vector4D(const Vector4D& v) { x = v.x;	y = v.y;	z = v.z; t = v.t; }

	// operator
	double length() { return sqrt(x*x + y*y + z*z + t*t); }
	double length2() { return x*x + y*y + z*z + t*t; }

	Vector4D& Vector4D::operator=(const Vector4D& v)
	{
		x = v.x;  y = v.y;  z = v.z;  t = v.t;
		return (*this);
	}

	Vector4D& Vector4D::operator+=(const Vector4D& v)
	{
		x += v.x;	y += v.y;	z += v.z;  t += v.t;
		return (*this);
	}

	Vector4D& Vector4D::operator-=(const Vector4D& v)
	{
		x -= v.x;	y -= v.y;	z -= v.z;  t -= v.t;
		return (*this);
	}

	Vector4D& Vector4D::operator*=(double u)
	{
		x *= u;  y *= u;  z *= u;  t *= u;
		return (*this);
	}

	Vector4D& Vector4D::operator/=(double u)
	{
		if (!EQUALZERO(u))
		{
			x /= u;  y /= u;  z /= u;  t /= u;
		}
		return(*this);
	}

	friend Vector4D operator+(const Vector4D& lv, const Vector4D& rv)
	{
		Vector4D rel = lv;
		rel += rv;
		return rel;
	}

	friend Vector4D operator-(const Vector4D& lv, const Vector4D& rv)
	{
		Vector4D rel = lv;
		rel -= rv;
		return rel;
	}

	friend Vector4D operator*(const double u, const Vector4D& rv)
	{
		Vector4D rel = rv;
		rel *= u;
		return rel;
	}

	friend Vector4D operator*(const Vector4D& lv, const double u)
	{
		Vector4D rel = lv;
		rel *= u;
		return rel;
	}

	friend Vector4D operator/(const Vector4D& lv, const double u)
	{
		Vector4D rel = lv;
		rel /= u;
		return rel;
	}

	friend double operator*(const Vector4D& lv, const Vector4D& rv)
	{
		return lv.x*rv.x + lv.y*rv.y + lv.z*rv.z + lv.t*rv.t;
	}
};

//10D point
class Vector10D 
{
public:
	double p[10];
	Vector10D() {}
};

class Vector20D
{
public:
	double x[30];
	Vector20D() {}
};

class Vector50D
{
public:
	double x[50];
	Vector50D() {}
};

//#endif