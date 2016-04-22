#ifndef UTIL_H
#define UTIL_H

#include <Eigen/Eigen>
#include <cassert>
#include <math.h>


using namespace Eigen;

//left hand rule to rotate

//bool is_nan(float x) { return isnan(x);}

inline Matrix4f Mat4FromQuaterion( Vector3f axis, float angle_in_radian)
{
	Quaternion<float> q;
	q = AngleAxis<float>(angle_in_radian, axis);
	Matrix3f r3 = q.toRotationMatrix ();
	Matrix4f rotataion;
	rotataion.setZero();
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
			rotataion(i,j) = r3(i,j);
	}
	rotataion(3,3) = 1.0;	//!! !!
	return rotataion;
}

inline Matrix3f Mat3FromQuaterion(Vector3f axis, float angle_in_radian)
{
	Quaternion<float> q;
	q = AngleAxis<float>(angle_in_radian, axis);
	return q.toRotationMatrix ();
}
inline bool component_smaller(Vector3f arg1, Vector3f arg2)
{
	return arg1(0)<=arg2(0)&&arg1(1)<=arg2(1)&&arg1(2)<=arg2(2);
}
inline bool component_larger(Vector3f arg1, Vector3f arg2)
{
	return arg1(0)>=arg2(0)&&arg1(1)>=arg2(1)&&arg1(2)>=arg2(2);
}
//return componentwise smaller one
inline Vector3f min_comp(Vector3f a, Vector3f b)
{
	return component_smaller(a,b)?a:b;
}
//return componentwise larger one
inline Vector3f max_comp(Vector3f a, Vector3f b)
{
	return component_smaller(a,b)?b:a;
}
template<typename T> T inline  max(T b, T a)
{
	return b>a ? b:a;
}
template<typename T> T inline  min(T b, T a)
{
	return b<a ? b:a;
}
template<typename T> T inline clamp(T in, T _min, T _max)
{
	if(in<_min)
		return _min;
	if(in>_max)
		return _max;
	return in;
}

inline float sign2D (Vector2f p1, Vector2f p2, Vector2f p3)
{
	return (p1(0) - p3(0)) * (p2(1) - p3(1)) - (p2(0) - p3(0)) * (p1(1) - p3(1));
}

inline bool PointInTriangle2D (Vector2f pt, Vector2f v1, Vector2f v2, Vector2f v3)
{
	bool b1, b2, b3;

	b1 = sign2D(pt, v1, v2) < 0.0f;
	b2 = sign2D(pt, v2, v3) < 0.0f;
	b3 = sign2D(pt, v3, v1) < 0.0f;

	return ((b1 == b2) && (b2 == b3));
}
inline float AreaTrangle(Vector2f A, Vector2f B, Vector2f C)
{
	return fabs(A(0)*(B(1)-C(1)) + B(0)*(C(1)-A(1)) + C(0)*(A(1)-B(1)))*0.5;
}
inline Vector3f BayercentricInterpolation (Vector2f pos, Vector2f A1, Vector2f A2, Vector2f A3)
{
	if(!PointInTriangle2D(pos,A1,A2,A3))
		return Vector3f(0,0,0);
	else{
		
		float Area = AreaTrangle(A1,A2,A3);
		if(Area == 0) return Vector3f(0,0,0);
		float w1 = AreaTrangle(pos,A2,A3)/Area;
		float w2 = AreaTrangle(pos,A1,A3)/Area;
		float w3 = 1.0 - w1 - w2;
		return Vector3f(w1,w2,w3);
	}
}
//pos : the position of interpolated point, val1 cord1... is value and coordinate associated with two points
template<typename T>
inline T LinearInterpolate( float pos, float cord1, float cord2, T val1, T val2){
	//assert( pos>=cord1&&pos<=cord2 );
	
	if(pos>=cord1&&pos<=cord2)
	{
		float dist = fabs(cord2 -cord1);
		float w1   = fabs(cord2 - pos)/dist;
		return val1*w1 + val2*(1.f-w1);
	}
	else if(pos<cord1)
		return val1;
	else return val2;
}

inline Vector3f GetTrangleNormal(Vector3f A, Vector3f B, Vector3f C)
{
	Vector3f _result;
	_result(0) = (B(1) - A(1))*(C(2) - A(2)) - (C(1) - A(1))*(B(2) - A(2));
	_result(1) = (B(2) - A(2))*(C(0) - A(0)) - (C(2) - A(2))*(B(0) - A(0));
	_result(2) = (B(0) - A(0))*(C(1) - A(1)) - (C(0) - A(0))*(B(1) - A(1));
	_result.normalize();
	return _result;
}
#endif