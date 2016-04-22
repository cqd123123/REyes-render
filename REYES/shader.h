#ifndef SHADER_H
#define SHADER_H

#include <math.h>
#include "Eigen/Eigen"
#include "util.h"

#define SPEC_M 32


using namespace Eigen;

struct LightPoint{
	float Amblight;
	Vector3f Pos;
	Vector3f Color;
};
//light, normal, sample position, eyeposition, and surface color
 Vector3f Labmert_Shader(LightPoint* lig, Vector3f Normal, Vector3f HitPos, Vector3f Eyepos, Vector3f Surface_col, bool Trans)
{
	//cal surface to light dir
	Vector3f result = Surface_col*lig->Amblight;	//ambinent light
	Vector3f suf2L = lig->Pos - HitPos;
	Vector3f suf2E = Eyepos - HitPos;
	suf2E.normalize();
	suf2L.normalize();
	if(Normal.dot(Normal) != 1.0) Normal.normalize();
//	if(suf2L.dot(suf2E) < 0) return result;	//
	float Val1 = Normal.dot(suf2E);
	float Val2 = Normal.dot(suf2L);

	//if((Val1*Val2 < 0)&&!Trans) return result;	//light eye not in same plane
	//if((Val1<0 || Val2<0)&&!Trans) return result;

	//if(Trans)
	{
		if(Val2*Val2 < 0) return result;
	}
	//else if((Val1<0 || Val2<0)) return result;

	Vector3f comp_col;
	Vector3f Half_vec;
	Half_vec = 0.5*(suf2E + suf2L);
	Half_vec.normalize();
	comp_col(0)  = lig->Color(0)*Surface_col(0);
	comp_col(1)  = lig->Color(1)*Surface_col(1);
	comp_col(2)  = lig->Color(2)*Surface_col(2);
	 Val2 = fabs(Val2);
	float Val3 = Normal.dot(Half_vec);
	 Val3 = fabs(Val3);

	float cosi   = max(Val2,0.f);
	float coshalf= max(Val3,0.f);
	float coshalf2 = coshalf*coshalf;
	float coshalf4 = coshalf2*coshalf2;
	float coshalf8 = coshalf4*coshalf4;
	float coshalf16 = coshalf8*coshalf8;
	float coshalf32 = coshalf16*coshalf16;

	float Lamb_com = cosi / M_PI;

	float spec_com = (SPEC_M + 8)/8/M_PI;
	spec_com *= cosi*coshalf32;

	if(Trans) spec_com = 0.f;
	//float Shadernum = ;

	return result+Lamb_com*comp_col + spec_com*lig->Color;
}

 Vector3f Texture_Shader( Vector3f Normal, Vector3f HitPos, Vector3f parameter, Texture* tex)
{
	if(tex == NULL) return Vector3f(0.f,0.f,0.f);
	Vector2f para;
	para(0) = parameter(0);
	para(1) = parameter(1);
	return tex->Linear_interpolate_val(para);
}
 Vector3f CheckBoard_Shader(Vector3f parameter, int U_size, int V_Size)
 {
	 float pa1 = parameter[0];	//u
	 float pa2 = parameter[1];	//v
	 int ipa1 = pa1*U_size;
	 int ipa2 = pa2*V_Size;
	 if(ipa1%2 == ipa2%2)
		 return Vector3f(1.f,1.f,1.f);
	 else return Vector3f(0.f,0.f,0.f);
 }
 Vector3f get_height_Trangle(Vector3f pa, Vector3f pb, Vector3f pc, Texture* tex){
	if(tex == NULL) return Vector3f(0.f,0.f,0.f);
	Vector2f Ap,Bp,Cp;
	Ap(0) = pa(0);Ap(1) = pa(1);
	Bp(0) = pb(0);Bp(1) = pb(1);
	Cp(0) = pc(0);Cp(1) = pc(1);
	float h1 = tex->Linear_interpolate_val(Ap)(0);
	float h2 = tex->Linear_interpolate_val(Bp)(0);
	float h3 = tex->Linear_interpolate_val(Cp)(0);
	return Vector3f(h1,h2,h3);
}
 Vector3f Displacement_shader(Vector3f Normal, Vector3f SN_A,Vector3f SN_B, Vector3f SN_C){
	Vector3f _result = GetTrangleNormal(SN_A,SN_B,SN_C);
	if(_result.dot(Normal)<0)
		_result = -_result;
	return _result;
}

 void Displacement_shader_new(Vector3f finpos[], Texture* tex, patch* pa, float def_const){
	
	for(int i=0;i<4;i++)
	{
		finpos[i] = pa->p_vs[i]; 
	}
	if(tex == NULL) {return;}
	//get normal of the patch
	Vector3f _normal[4];
	for(int i=0;i<4;i++)
	{
		_normal[i] = pa->Normal[i];
	}
	//access height
	Vector2f _para[4];
	float _heights[4];
	for(int i=0;i<4;i++)
	{
		_para[i](0) = pa->parameter[i](0);
		_para[i](1) = pa->parameter[i](1);
	}
	
	for(int i=0;i<4;i++)
	{
		//if(isnan(0.1));
		_heights[i] = tex->Linear_interpolate_val(_para[i])(0);
	}
	//deform the pos along the normal
	for(int i=0;i<4;i++)
	{
		finpos[i] += _heights[i] * _normal[i]*def_const;
	}
}

#endif