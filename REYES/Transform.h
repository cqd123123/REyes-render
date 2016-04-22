#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Eigen/Eigen"
#include "util.h"
#include <math.h>

using namespace Eigen;

class TransformV {

public:
	 EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	TransformV(){
		ViewPointMatrix.setZero();
		World2CameraMatrix.setZero();
		RotationMatrix.setZero();
		TotalTransMatrix.setZero();
		for(int i=0;i<4;i++)	//init to indentity
		{
			ViewPointMatrix(i,i) = 1.0;
			World2CameraMatrix(i,i) = 1.0;
			RotationMatrix(i,i) = 1.0;
			TotalTransMatrix(i,i) = 1.0;
		}
		//init camera at zero, faceing z + axis
		CameraPos = Vector3f(0,0,0);
		TotalTransMatrix = ViewPointMatrix * World2CameraMatrix;
	}

	void SetViewProjectMatrix(float angleOfView, float nearClip,  float farClip)
	{
		Eigen::Matrix4f mat;
		mat.setZero();
		float scale = 1.f / tan(angleOfView * 0.5 * M_PI / 180);
		float temp = 1.f / 2.f;
		mat(0,0) = scale;
		mat(1,1) = scale;
		mat(2,2) = - farClip / (farClip - nearClip);
		mat(3,2) = - 1.0;
		mat(2,3) =  farClip*nearClip/(farClip - nearClip);

		ViewPointMatrix = mat;
		TotalTransMatrix = ViewPointMatrix * World2CameraMatrix;
	}

	//add transform vector directly to camera transform
	void TransformCamera(Vector3f dir)
	{
		World2CameraMatrix(0,3) += dir(0);
		World2CameraMatrix(1,3) += dir(1);
		World2CameraMatrix(2,3) += dir(2);
		CameraPos -= dir;
		TotalTransMatrix = ViewPointMatrix * RotationMatrix *World2CameraMatrix;
		//std::cout<<TotalTransMatrix<<std::endl;
	}
	//left hand rule to rotate world vertecies
	void RotateCamera(Vector3f axis, float angle_rad)
	{
		RotationMatrix = Mat4FromQuaterion(axis, angle_rad);
		TotalTransMatrix = ViewPointMatrix * RotationMatrix *World2CameraMatrix;
	}
	inline void TransVertex(Vector3f in, Vector3f& out)
	{
		Matrix4f& mat = this->TotalTransMatrix;	//homogenious coordinates
		out(0) = mat(0,0)*in(0) + mat(0,1)*in(1) + mat(0,2)*in(2) + mat(0,3);
		out(1) = mat(1,0)*in(0) + mat(1,1)*in(1) + mat(1,2)*in(2) + mat(1,3);
		out(2) = mat(2,0)*in(0) + mat(2,1)*in(1) + mat(2,2)*in(2) + mat(2,3);
		float w = mat(3,0)*in(0) + mat(3,1)*in(1) + mat(3,2)*in(2) + mat(3,3);
		if(w!=1.0 && w!=0.0)
		{
			out = out/w;
		}
	}
	inline void TransVertexNew(Vector3f& in, Vector3f& out)
	{
		Matrix4f& mat = this->TotalTransMatrix;	//homogenious coordinates
		out(0) = mat(0,0)*in(0) + mat(0,1)*in(1) + mat(0,2)*in(2) + mat(0,3);
		out(1) = mat(1,0)*in(0) + mat(1,1)*in(1) + mat(1,2)*in(2) + mat(1,3);
		out(2) = mat(2,0)*in(0) + mat(2,1)*in(1) + mat(2,2)*in(2) + mat(2,3);
		float w = mat(3,0)*in(0) + mat(3,1)*in(1) + mat(3,2)*in(2) + mat(3,3);
		if(w!=1.0 && w!=0.0)
		{
			out(0) = out(0)/w;
			out(1) = out(1)/w;
		}
	}
	inline bool OutofImagePlane(Vector3f in)
	{
		if(in(0)<-1 || in(0)>1 || in(1)<-1 || in(1)>1 || in(2) < 0 || in(2) > 1)
			return true;
		else return false;

	}
	//whether the depth of a quad lies in [nearclip, farclip]
	inline bool QuadOutofFrustum(Vector3f* _in)
	{
		//both of the four vertices out of range
		bool is_out = true;
		for(int i=0;i<4;i++)
		{
			if(!OutofImagePlane(_in[i]))	//either of the point is inside the frustum
				return false;
		}
		return true;	//four is out of frustum
	}
	inline Vector2f Translate_To_screen(Vector3f in, int _w, int _h)
	{
		;
	}
	Matrix4f getViewProjectMartix() {return ViewPointMatrix;}
	Matrix4f getWorld2CameraMatrix() {return World2CameraMatrix;}
	Vector3f getCameraPos(){return CameraPos;}
	void printMatrix()
	{
		std::cout<<TotalTransMatrix<<std::endl;
	}

private:
	Matrix4f ViewPointMatrix;
	Matrix4f World2CameraMatrix;
	Matrix4f RotationMatrix;
	Matrix4f TotalTransMatrix;
	Vector3f CameraPos;
};
#endif