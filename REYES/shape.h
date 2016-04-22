#ifndef SHAPE_H
#define SHAPE_H

#include "mesh.h"
#include "Eigen/Eigen"
#include <math.h>
#include "Patch.h"

using namespace Eigen;

//generate uniform rectangular mesh, can be transformed to other shapes
//vertex x range [0,1],xres, y in range [0,1],yres ,z is inited to zero

void Generate_Base_Rec_Mesh(Mesh2D& _result,int xRes, int yRes)
{
	float _dx = 1.0f/xRes;
	float _dy = 1.0f/yRes;
	//Mesh2D _result;
	_result.xRes = xRes;
	_result.yRes = yRes;
	//generate all vertices
	for(int j=0;j<=yRes;j++)
	{
		for(int i=0;i<=xRes;i++)
		{
			Vector3f vertpos;
			vertpos(0) = i*_dx;
			vertpos(1) = j*_dy;
			vertpos(2) = 0.;
			Vector3f normal;
			normal.setZero();
			Vector3f color;
			color.setZero();
			Vector3f textcoord;
			textcoord.setZero();

			Vertex vet;
			vet.position = vertpos;
			vet.Color    = color;
			vet.TextCord = textcoord;
			vet.Normal   = normal;

			_result.Add_Vertexs(vet);
		}
	}
	//associate vertex to quads
	for(int j=0;j<yRes;j++)
	{
		for(int i=0;i<xRes;i++)
		{
			Quad quad;
			Vertex* vet1 = &(_result.get_vertexs(i,j));
			Vertex* vet2 = &(_result.get_vertexs(i+1,j));
			Vertex* vet3 = &(_result.get_vertexs(i,j+1));
			Vertex* vet4 = &(_result.get_vertexs(i+1,j+1));
			
			quad.vertexs.push_back(vet1);
			quad.vertexs.push_back(vet2);
			quad.vertexs.push_back(vet3);
			quad.vertexs.push_back(vet4);
			
			_result.Add_Quads(quad);
		}
	}
	_result.BoundMin = Vector3f(0,0,0);
	_result.BoundMax = Vector3f(1,1,0);
	//return _result;
}

class Geometry{
public:
	Geometry(Matrix3f _rot, Vector3f _tra):Model_Rotate(_rot), Model_trans(_tra){
		Model_Color_uniform.setZero();
		uniform_color = true;
		Model_Rotate_invT = (Model_Rotate.transpose()).inverse();
		_tex = NULL;
		Tpancy = 1.0;
		
	}
	~Geometry(){delete mesh;}

	void BindColor_uniform(Vector3f color){
		mesh->BindColor_Uniform(color);
	}
	Mesh2D* get_mesh(){return mesh;}
	Vector3f get_BoxMax(){return BoxMax;}
	Vector3f get_BoxMin(){return BoxMin;}
	virtual void Tesellate_once(int xRes, int yRes){};
	virtual inline void Deform_Pathes(patch*){};
	virtual void Deform_init(){};
	int get_num_of_patch(){return patches.size();}
	patch* get_patch(int i){return patches[i];}
	inline Vector3f get_geom_color(){return Model_Color_uniform;}
	void Bind_color_uniform(Vector3f _col){Model_Color_uniform = _col; uniform_color = true;}
	bool is_uniform_color(){return uniform_color;}
	void Generate_pathches(){
		Mesh2D* _base = new Mesh2D();
		Generate_Base_Rec_Mesh(*_base,4,4);	//initial 16 patches
		//set up patch parameter
		for(int j=0;j<4;j++)
			for(int i=0;i<4;i++)
			{
				patch* _pa = new patch;
				Vector3f v0 = _base->get_vector_pos(i,j);
				Vector3f v1 = _base->get_vector_pos(i+1,j);
				Vector3f v2 = _base->get_vector_pos(i,j+1);
				Vector3f v3 = _base->get_vector_pos(i+1,j+1);
				_pa->parameter[0] = v0;
				_pa->parameter[1] = v1;
				_pa->parameter[2] = v2;
				_pa->parameter[3] = v3;
				_pa->mesh = NULL;
				_pa->childs.clear();
				patches.push_back(_pa);

			}
			delete _base;
	}
	void Debug_pathc(patch* pa)
	{
		patches.push_back(pa);
	}
	inline void Tesellate_patch(patch* pat)
	{
		
		Vector3f para[4];
		for(int i=0;i<4;i++)
			para[i] = pat->parameter[i];

		patch* pat1 = new patch;
		pat1->childs.clear();
		pat1->mesh=NULL;
		pat1->parameter[0] = para[0];
		pat1->parameter[1] = (para[0] + para[1])*0.5;
		pat1->parameter[2] = (para[0] + para[2])*0.5;
		pat1->parameter[3] = (para[0] + para[3])*0.5;
		pat->childs.push_back(pat1);
		patch* pat2 = new patch;
		pat2->childs.clear();
		pat2->mesh = NULL;
		pat2->parameter[0] = (para[0] + para[1])*0.5;
		pat2->parameter[1] = para[1];
		pat2->parameter[2] = (para[0] + para[3])*0.5;
		pat2->parameter[3] = (para[1] + para[3])*0.5;
		pat->childs.push_back(pat2);
		patch* pat3 = new patch;
		pat3->childs.clear();
		pat3->mesh = NULL;
		pat3->parameter[0] = (para[0] + para[2])*0.5;
		pat3->parameter[1] = (para[0] + para[3])*0.5;
		pat3->parameter[2] = para[2];
		pat3->parameter[3] = (para[2] + para[3])*0.5;
		pat->childs.push_back(pat3);
		patch* pat4 = new patch;
		pat4->childs.clear();
		pat4->mesh = NULL;
		pat4->parameter[0] = (para[0] + para[3])*0.5;
		pat4->parameter[1] = (para[1] + para[3])*0.5;
		pat4->parameter[2] = (para[2] + para[3])*0.5;
		pat4->parameter[3] = para[3];
		pat->childs.push_back(pat4);
	}

	Texture* get_texture(){return _tex;}
	void Bind_Texture(Texture* tex_in){
		_tex = tex_in;
	}
	void Set_transpancy(float _t){Tpancy = _t;}
	float Get_transpancy(){return Tpancy;}
	int Surface_status;
	int checkborad_Nw;
	int checkborad_Nh;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
protected:
	Mesh2D* mesh;
	Matrix3f Model_Rotate;
	Vector3f Model_trans;
	Matrix3f Model_Rotate_invT;
	Vector3f BoxMin;
	Vector3f BoxMax;
	std::vector<patch*> patches;
	Vector3f Model_Color_uniform;
	bool uniform_color;
	Texture* _tex;
	float Tpancy;
};

class Sphere :public Geometry{

public:
	Sphere(float _r, float _zmin, float _zmax, float _themax, Matrix3f Model_rot, Vector3f _trans): 
	  Geometry(Model_rot,_trans) , Radius(_r),Zmin(_zmin),Zmax(_zmax),ThetaMax(_themax)
	{
		BoxMin = Model_Rotate*Vector3f(-_r, -_r,-_r) + _trans;
		BoxMax = Model_Rotate*Vector3f(_r,_r,_r) + _trans;
		if(Zmin > -Radius)
		{
			PhiMin = asin(Zmin/Radius);
		}
		else PhiMin = - M_PI*0.5;
		if(Zmax < Radius)
		{
			PhiMax = asin(Zmax/Radius);
		}
		else PhiMax = M_PI*0.5;

	};
	~Sphere(){delete mesh;}

	void Tesellate_once(int xRes, int yRes)
	{
		//
		std::cout<<"!!!!!! tesellate onece for sphere not implemented !!!"<<std::endl;
		exit(0);
	}

	inline void Deform_Pathes(patch* pa){
		//deform patch accordig to its params
		Vector3f para[4];
		for(int i=0;i<4;i++)
			para[i] = pa->parameter[i];
		for(int i=0;i<4;i++)
		{
			Vector3f _pos_new;
			Vector3f _nor_new;
			float u = para[i](0);
			float v = para[i](1);
			
			float Theta = u*ThetaMax;
			float Phi   = PhiMin + v*(PhiMax - PhiMin);
			float Val1  = cos(Theta)*cos(Phi);
			float Val2  = sin(Theta)*cos(Phi);
			float Val3  = sin(Phi);
			_pos_new(0) = Radius*Val1;
			_pos_new(1) = Radius*Val2;
			_pos_new(2) = Radius*Val3;
			_nor_new(0) = Val1;
			_nor_new(1) = Val2;
			_nor_new(2) = Val3;
			pa->p_vs[i] = Model_Rotate*_pos_new + Model_trans;
			pa->Normal[i] = Model_Rotate_invT*_nor_new;
			//		pa->p_vs[i] = para[i];
		}
	};

	void Deform_init(){
		for(int i=0;i<patches.size();i++)
		{
			Deform_Pathes(patches[i]);
		}
	}

private:
	float Radius;
	float Zmin;
	float Zmax;
	float ThetaMax;
	float PhiMin;
	float PhiMax;
};
//Vector3f Model_Color_uniform; zim, zmax, thetamax, radius
class Cylinder : public Geometry{
public:
	Cylinder(float _r,float _zmin,float _zmax ,float _Thetamax, Matrix3f Model_rot, Vector3f _trans): 
	  Geometry(Model_rot,_trans) , Radius(_r) , Zmin(_zmin), Zmax(_zmax),ThetaMax(_Thetamax)
	{
		BoxMin = Model_Rotate*Vector3f(-_r, -_r,0) + _trans;
		BoxMax = Model_Rotate*Vector3f(_r,_r,_zmax-_zmin) + _trans;
	};
	~Cylinder(){delete mesh;}

	void Tesellate_once(int xRes, int yRes)
	{
		//mesh = new Mesh2D();
		//Generate_Base_Rec_Mesh(*mesh,xRes,yRes);
		//Deform_Mesh_sphere(*mesh,Radius,Model_Rotate,Model_trans);
		std::cout<<"!!!!!! tesellate onece for cylinder not implemented !!!"<<std::endl;
		exit(0);
	}

	inline void Deform_Pathes(patch* pa){
		//deform patch accordig to its params
		Vector3f para[4];
		for(int i=0;i<4;i++)
			para[i] = pa->parameter[i];
		for(int i=0;i<4;i++)
		{
			Vector3f _pos_new;
			Vector3f _nor_new;
			float theta = para[i](0)*ThetaMax;
			float _Z    = para[i](1)*(Zmax - Zmin);
			float Val1  = cos(theta);
			float Val2  = sin(theta);
			_pos_new(0) = Radius*Val1;
			_pos_new(1) = Radius*Val2;
			_pos_new(2) = _Z;
			_nor_new(0) = Val1;
			_nor_new(1) = Val2;
			_nor_new(2) = 0.0;
			pa->p_vs[i] = Model_Rotate*_pos_new + Model_trans;
			pa->Normal[i] = Model_Rotate_invT*_nor_new;
		}
	};

	void Deform_init(){
		for(int i=0;i<patches.size();i++)
		{
			Deform_Pathes(patches[i]);
		}
	}

private:
	float Radius;
	float Zmin;
	float Zmax;
	float ThetaMax;
};

class Cone : public Geometry{

public:
	Cone(float _r,float _hei, float _Thetamax, Matrix3f Model_rot, Vector3f _trans): 
	  Geometry(Model_rot,_trans) , Radius(_r) , Heig(_hei),ThetaMax(_Thetamax)
	  {
		  BoxMin = Model_Rotate*Vector3f(-_r, -_r,0) + _trans;
		  BoxMax = Model_Rotate*Vector3f(_r,_r,_hei) + _trans;
	  };
	  ~Cone(){delete mesh;}

	  void Tesellate_once(int xRes, int yRes)
	  {
		  std::cout<<"!!!!!! tesellate onece for cone not implemented !!!"<<std::endl;
		  exit(0);
	  }

	  inline void Deform_Pathes(patch* pa){
		  //deform patch accordig to its params
		  Vector3f para[4];
		  for(int i=0;i<4;i++)
			  para[i] = pa->parameter[i];
		  for(int i=0;i<4;i++)
		  {
			  Vector3f _pos_new;
			  Vector3f _nor_new;
			  float u = para[i](0);
			  float v = para[i](1);
			  float theta = u*ThetaMax;
			  float leng = sqrtf(Radius*Radius + Heig*Heig);
			  float V1 = cos(theta);
			  float V2 = sin(theta);
			  _pos_new(0) = Radius*(1-v)*V1;
			  _pos_new(1) = Radius*(1-v)*V2;
			  _pos_new(2) = v*Heig;
			  _nor_new(0) = V1*Heig/leng;
			  _nor_new(1) = V2*Heig/leng;
			  _nor_new(2) = Radius/leng;

			  pa->p_vs[i] = Model_Rotate*_pos_new + Model_trans;
			  pa->Normal[i] = Model_Rotate_invT*_nor_new;
		  }
	  };

	  void Deform_init(){
		  for(int i=0;i<patches.size();i++)
		  {
			  Deform_Pathes(patches[i]);
		  }
	  }

private:
	float Radius;
	float Heig;
	float ThetaMax;
};

class Torus : public Geometry{

public:
	Torus(float _MajorR, float _MinorR,float _Phimin,float _Phimax ,float _Thetamax, Matrix3f Model_rot, Vector3f _trans): 
	  Geometry(Model_rot,_trans) , Major_R(_MajorR) , Minor_R(_MinorR), PhiMin(_Phimin),PhiMax(_Phimax),ThetaMax(_Thetamax)
	  {
		  BoxMin = Model_Rotate*Vector3f(- (_MajorR + _MinorR ), -(_MajorR + _MinorR), -_MinorR) + _trans;
		  BoxMax = Model_Rotate*Vector3f((_MajorR+_MinorR),(_MajorR+_MinorR),_MinorR) + _trans;
	  };
	  ~Torus(){delete mesh;}

	  void Tesellate_once(int xRes, int yRes)
	  {
		  std::cout<<"!!!!!! tesellate onece for torus not implemented !!!"<<std::endl;
		  exit(0);
	  }

	  inline void Deform_Pathes(patch* pa){
		  //deform patch accordig to its params
		  Vector3f para[4];
		  for(int i=0;i<4;i++)
			  para[i] = pa->parameter[i];
		  for(int i=0;i<4;i++)
		  {
			  Vector3f _pos_new;
			  Vector3f _nor_new;
			  float theta = para[i](0) * ThetaMax;
			  float Phi   = PhiMin + (PhiMax - PhiMin) * para[i](1);
			  float V1    = cos(Phi);
			  float V2    = cos(theta);
			  float V3    = sin(theta);
			  float V4    = sin(Phi);
			  float r     = Minor_R*V1;
			  _pos_new(0) = (Major_R + r)*V2;
			  _pos_new(1) = (Major_R + r)*V3;
			  _pos_new(2) = Minor_R*V4;
			  _nor_new(0) = V1*V2;
			  _nor_new(1) = V3*V1;
			  _nor_new(2) = V4;
			  _nor_new.normalize();
			  pa->p_vs[i] = Model_Rotate*_pos_new + Model_trans;
			  pa->Normal[i] =Model_Rotate_invT* _nor_new;
		  }
	  };

	  void Deform_init(){
		  for(int i=0;i<patches.size();i++)
		  {
			  Deform_Pathes(patches[i]);
		  }
	  }

private:
	float Major_R;
	float Minor_R;
	float PhiMin;
	float PhiMax;
	float ThetaMax;
};

#endif