#pragma once

#include <cmath>
#include <vector>
#pragma region Vector


namespace ____MATH__Vector
{
	template <class T> struct Vector2
	{
		union 
		{
			struct{ T x,y;};
			struct{ T u,v;};
			T raw[2];
		};

		Vector2():x(0),y(0){ }
		Vector2(T xx,T yy):x(xx),y(yy){ }

		inline Vector2<T> operator+ (const Vector2<T>& a) const{ return Vector2<T>(a.x+x,a.y+y); }
		inline Vector2<T> operator- (const Vector2<T>& a) const{ return Vector2<T>(x-a.x,y-a.y); }
		inline Vector2<T> operator* (T f) const {return Vector2<T>(x*f,y*f); }
	};

	template <class T> struct Vector3
	{
		union {
			struct {T x, y, z;};
			struct { T ivert, iuv, inorm; };
			T raw[3];
		};


		Vector3() : x(0), y(0), z(0) {}
		Vector3(T xx, T yy, T zz) : x(xx),y(yy),z(zz) {}

		//���
		inline Vector3<T> operator^ (const Vector3<T> &v) const { return Vector3<T>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }


		inline Vector3<T> operator+ (const Vector3<T> &v) const { return Vector3<T>(x+v.x, y+v.y, z+v.z); }


		inline Vector3<T> operator- (const Vector3<T> &v) const { return Vector3<T>(x-v.x, y-v.y, z-v.z); }


		inline Vector3<T> operator* (float f) const { return Vector3<T>(x*f, y*f, z*f); }

		inline T operator* (const Vector3<T> &v) const { return x*v.x + y*v.y + z*v.z; }

		float len() const { return std::sqrt(x*x+y*y+z*z); }

		Vector3<T>& normalize() { *this = (*this)*(1/len()); return *this; }
	};
}

struct vec4f
{
	float x,y,z,w;
	vec4f(float xx,float yy,float zz,float ww):x(xx),y(yy),z(zz),w(ww){ }
};

typedef ____MATH__Vector::Vector2<float> vec2f;
typedef ____MATH__Vector::Vector2<int>   vec2i;
typedef ____MATH__Vector::Vector3<float> vec3f;
typedef ____MATH__Vector::Vector3<int>   vec3i;
typedef std::vector<vec3f> vvec3f;
typedef std::vector<vec3i> vvec3i;
typedef std::vector<vec2f> vvec2f;
#pragma endregion

#pragma region Matrix

namespace ____MATH__Matrix
{
	class Matrix
	{
	private:
		int ncols,nrows;
		std::vector<std::vector<float>> m;
	public:
		Matrix(int row,int col):m(std::vector<std::vector<float>>(row,std::vector<float>(col,0.f))),ncols(col),nrows(row){ }

		//�о���4*1
		Matrix(vec3f v):m(std::vector<std::vector<float> >(4, std::vector<float>(1, 1.f))),nrows(4), ncols(1)
		{
			m[0][0] = v.x;
			m[1][0] = v.y;
			m[2][0] = v.z;
			m[3][0] = 0.f;//w������Ϊ0
		}

		int Nrows(){return nrows;}
		int Ncols(){return ncols;}

		//���嵥λ����
		static Matrix identity(int row);
		std::vector<float>& operator[](const int i);
		Matrix operator*(const Matrix& a);
		Matrix transpose();//����ת��
		Matrix inverse();
	};
}

//mat(r,c)
typedef ____MATH__Matrix::Matrix mat;

#pragma endregion
