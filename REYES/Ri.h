#ifndef RI_H
#define RI_H


#include <iostream>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include "CImg.h"
#include "Eigen/Eigen"
#include "Transform.h"
#include "mesh.h"
#include "Image_Buffer.h"
#include "util.h"
#include "shape.h"
#include "reyes_render.h"
#include "Sampler.h"
#include "Patch.h"


#define CHECKBOARD 1
#define LAMBERT 0
#define BUMPY 2
#define RI_NULL 0

#define TextureMap0 &TextureShader


float BUMP_AMPLITUDE;
float BUMP_MIN_FREQ_EXP;
float BUMP_MAX_FREQ_EXP;

//global variabls

Vector3f RI_Model_trans;
Vector3f RI_Model_trans_old;
Matrix3f RI_Model_rotate;
Matrix3f RI_Model_rotate_old;
float    RI_IMAGE_Width;
float    RI_IMAGE_Height;
float    RI_Opacity;
float    RI_CHECK_SIZE_X;
float    RI_CHECK_SIZE_Y;
int      RI_SURFACE_KIND;

std::string RI_Output_name;

Vector3f RI_Model_Color;

std::vector<Geometry*> geoms;
Image_Buffer* imag_buff;
TransformV* trans;
sampler2D* sampler;
Texture* RI_Current_Texture = NULL;
std::vector<Texture*> RI_TEXTURES;

LightPoint* light = NULL;
std::vector<Texture*> RI_BUMP_TEXTURES;


class Color{
public:
	float col[3];
	inline float& operator[](int i) { 
		return col[i]; 
	};
};


#define RtColor Color
#define RtPoint Color
#define RtFloat float
#define RtInt int
#define RI_PERSPECTIVE "perspective"

void RiBegin(int){;}

void RiFormat(int _w, int _h, int z){
	RI_IMAGE_Width = _w;
	RI_IMAGE_Height = _h;
}

void RiPixelSamples(int xspp, int yspp){
	sampler =  new sampler2D(xspp,yspp);
	sampler->GenerateRndStratifiedSmps();
}
void RiShutter(float, float){;}


void RiFrameBegin(int i){
	std::stringstream ss;
	ss<<"output/frame";ss<<i;ss<<".bmp";
	RI_Output_name = ss.str();
	std::cout<<"frame: "<<i<<std::endl;
	imag_buff = new Image_Buffer(RI_IMAGE_Width,RI_IMAGE_Height);

	RI_Model_trans.setZero();
	RI_Model_trans_old.setZero();
	//indentity
	RI_Model_rotate.setZero();
	RI_Model_rotate_old.setZero();
	for(int i=0;i<3;i++)
	{
		RI_Model_rotate(i,i) = 1.f;
		RI_Model_rotate_old(i,i)= 1.f;
	}
}
//save the image buffer
void RiFrameEnd(){
	imag_buff->save_image_buffer(RI_Output_name);
	delete imag_buff;
	//delete gemetries
	geoms.clear();
	RI_TEXTURES.clear();
	RI_BUMP_TEXTURES.clear();

	delete trans;
	delete light;
	delete RI_Current_Texture;
	
	//reset transformations
	RI_Model_trans.setZero();
	RI_Model_trans_old.setZero();
	//indentity
	RI_Model_rotate.setZero();
	RI_Model_rotate_old.setZero();
	for(int i=0;i<3;i++)
	{
		RI_Model_rotate(i,i) = 1.f;
		RI_Model_rotate_old(i,i)= 1.f;
	}
}
void RiDisplay(std::string, std::string, std::string){;}

//TransformV trans;
//trans.SetViewProjectMatrix(60,0.1,100);
//trans.TransformCamera(Vector3f(0,0,2));	//take world coordinates and translate ,camera position should plus - this vector
//trans.RotateCamera(Vector3f(0,1,0), 0);
void RiProjection(std::string str1, std::string str2, float* fov){
	trans = new TransformV;
	trans->SetViewProjectMatrix(*fov, 0.1f,100.f);
}

void RiTranslate(float _x, float _y, float _z){
	RI_Model_trans += Vector3f(_x,_y,_z);
}
void RiScale(float x, float y, float z)
{
	Matrix3f scal;
	scal.setZero();
	scal(0,0) = x;
	scal(1,1) = y;
	scal(2,2) = z;
	RI_Model_rotate = scal * RI_Model_rotate;
}
void RiRotate(float _ang,float _x,float _y,float _z){
	_ang = _ang/180.f*M_PI;
	Matrix3f rot = Mat3FromQuaterion(Vector3f(_x, _y,_z),_ang);
	RI_Model_rotate = rot* RI_Model_rotate;

}
void RiWorldBegin(){
	//setup light
	light = new LightPoint();
	light->Pos = Vector3f(-18,-18,-18);
	light->Amblight = 0.1f;
	light->Color = Vector3f(6.0f,6.0f,6.0f);
}
void RiLight(float x, float y, float z, float r, float g, float b){
	light->Pos = Vector3f(x,y,z);
	light->Color = Vector3f(r,g,b);
}
void RiLigthAmb(float _amb)
{
	light->Amblight = _amb;
}
void RiColor(RtColor col){
	RI_Model_Color(0) = col[0];
	RI_Model_Color(1) = col[1];
	RI_Model_Color(2) = col[2];

	//std::cout<<RI_Model_Color<<std::endl;
}
//begin rendering
void RiWorldEnd(){
	std::cout<<geoms.size()<<std::endl;
	if(!trans||!imag_buff||!light) {std::cout<<"Null"<<std::endl; return;}
	//trans->printMatrix();
	Render_Geomereties_Tesellate(geoms,trans,imag_buff,sampler,light);
}
//save current state
void RiTransformBegin(){
	RI_Model_trans_old = RI_Model_trans;
	RI_Model_rotate_old = RI_Model_rotate;
}
//restore state
void RiTransformEnd(){

	RI_Model_rotate = RI_Model_rotate_old;
	RI_Model_trans = RI_Model_trans_old;
	RI_Current_Texture = NULL;
	RI_SURFACE_KIND = LAMBERT;
}

void RiCylinder(float _r,float _zmin,float _zmax,float _Thetamax){
	float Rad = _Thetamax/180.f*M_PI;
	Cylinder* cyl = new Cylinder(_r,_zmin,_zmax, Rad ,RI_Model_rotate,RI_Model_trans);
	cyl->Generate_pathches();
	cyl->Deform_init();
	cyl->Bind_color_uniform(RI_Model_Color);
	cyl->Bind_Texture(RI_Current_Texture);
	cyl->Set_transpancy(RI_Opacity);
	cyl->Surface_status = RI_SURFACE_KIND;
	if(RI_SURFACE_KIND == CHECKBOARD)
	{
		cyl->checkborad_Nw = RI_CHECK_SIZE_X;
		cyl->checkborad_Nh = RI_CHECK_SIZE_Y;
	}
	geoms.push_back(cyl);
}
void RiCone(float _r,float _hei, float _Thetamax){
	float Rad = _Thetamax/180.f*M_PI;
	Cone* con = new Cone(_r,_hei, Rad, RI_Model_rotate,RI_Model_trans);
	con->Generate_pathches();
	con->Deform_init();
	con->Bind_color_uniform(RI_Model_Color);
	con->Bind_Texture(RI_Current_Texture);
	con->Set_transpancy(RI_Opacity);
	con->Surface_status = RI_SURFACE_KIND;
	if(RI_SURFACE_KIND == CHECKBOARD)
	{
		con->checkborad_Nw = RI_CHECK_SIZE_X;
		con->checkborad_Nh = RI_CHECK_SIZE_Y;
	}
	geoms.push_back(con);
}

void RiSphere(float _r, float _zmin, float _zmax, float _themax){
	float Rad = _themax/180.f*M_PI;
	Sphere* sph = new Sphere(_r,_zmin,_zmax,Rad,RI_Model_rotate, RI_Model_trans);
	sph->Generate_pathches();
	sph->Deform_init();
	sph->Bind_color_uniform(RI_Model_Color);
	sph->Bind_Texture(RI_Current_Texture);
	sph->Set_transpancy(RI_Opacity);
	sph->Surface_status = RI_SURFACE_KIND;
	if(RI_SURFACE_KIND == CHECKBOARD)
	{
		sph->checkborad_Nw = RI_CHECK_SIZE_X;
		sph->checkborad_Nh = RI_CHECK_SIZE_Y;
	}
	geoms.push_back(sph);
}
//RiTorus(1, .25, 0, 360, 360);
void RiTorus(float _MajorR, float _MinorR,float _Phimin,float _Phimax ,float _Thetamax){
	float Rad_phi_Min = _Phimin/180.f*M_PI;
	float Rad_phi_Max = _Phimax/180.f*M_PI;
	float Rad_The_Max = _Thetamax/180.f*M_PI;
	Torus* tor = new Torus(_MajorR, _MinorR, Rad_phi_Min, Rad_phi_Max, Rad_The_Max, RI_Model_rotate,RI_Model_trans );
	tor->Generate_pathches();
	tor->Deform_init();
	tor->Bind_color_uniform(RI_Model_Color);
	tor->Bind_Texture(RI_Current_Texture);
	tor->Set_transpancy(RI_Opacity);
	tor->Surface_status = RI_SURFACE_KIND;
	if(RI_SURFACE_KIND == CHECKBOARD)
	{
		tor->checkborad_Nw = RI_CHECK_SIZE_X;
		tor->checkborad_Nh = RI_CHECK_SIZE_Y;
	}
	geoms.push_back(tor);
}
void RiEnd(){
	delete sampler;
}

void RiOpacity(Color col){
	RI_Opacity = col[0];
}
void RiEnableOpacity()
{
	if(!imag_buff) {std::cout<<"image buffer is null"<<std::endl; return;}
	imag_buff->Enable_Transpancy();
}
void RiCameraTranslate(float x, float y, float z)
{
	trans->TransformCamera(Vector3f(-x,-y,-z));
}
void RiCameraRotate(float _ang, float x, float y, float z)
{
	trans->RotateCamera(Vector3f(x,y,z), _ang/180.f*M_PI);
}
void RiMakeTexture(std::string _name, int i)
{
	Texture* texx = new Texture(_name.c_str(),false);
	RI_TEXTURES.push_back(texx);
}
void RIBindTexture(int i)
{
	//i = clamp(i,0,RI_TEXTURES.size() -1);
	RI_Current_Texture = RI_TEXTURES[i];
}
void RIBindBumpTexture(int i)
{
	RI_Current_Texture = RI_BUMP_TEXTURES[i];
}
void RiSurface(int _status){
	if(_status = CHECKBOARD)
	{
		RI_SURFACE_KIND = CHECKBOARD;
	}
	else RI_SURFACE_KIND = LAMBERT;
}
// void RiSurface(void (*shah)(void)){
//   (*shah)();
// }
void RiDisplacementTexture(std::string ss){
	Texture* texx= new Texture(ss.c_str(),true);
	texx->is_deformation = true;
	RI_BUMP_TEXTURES.push_back(texx);
}

// void RiDisplacement(int i)
// {
// 	;
// }
// void TextureShader(void)
// {
// 	;
// }
#endif