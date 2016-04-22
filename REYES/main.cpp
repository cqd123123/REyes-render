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
#include "noise.h"
#include "TestScene.h"



//init camera points to +z direction in world coordinates


using namespace cimg_library;
using namespace Eigen;

#define image_width 512
#define image_heigh 512

const float RI_TESELLATION_LEVEL = 0.5;

int main(){
	ShaderTest();
	//Tunnel();
	//Tornado();
	//SampleScene1();
	//Earth();
}

/*int main() {

	Matrix3f Mode_Rot;
	Vector3f Mode_Tras;

	TransformV trans;
	trans.SetViewProjectMatrix(60,0.1,100);
	trans.TransformCamera(Vector3f(0,0,2));	//take world coordinates and translate ,camera position should plus - this vector
	trans.RotateCamera(Vector3f(0,1,0), 0);
	

 	Image_Buffer buffer(image_width,image_heigh);
//	Render_meshes_vertex(&buffer, &trans, meshes);
//	Render_meshes_quad_point(&buffer,&trans, meshes);
	
	sampler2D smp(1,1);
	
	smp.GenerateRndStratifiedSmps();
//	smp.Debug();
	std::cout<<"begin rendering"<<std::endl;

	Texture *tex = new Texture("lena.bmp", false);

	//generate height map
	//PerlinNoise* pl = new PerlinNoise(0.1,0.05,1.0,10,213);

	Texture *heimap =new  Texture("hei1.bmp",true);
	heimap->is_deformation = true;
	//heimap->is_height = true;
// 
// 	for(int i=0;i<512;i++)
// 	{
// 		for(int j=0;j<512;j++)
// 		{
// 			float indx1 = (float)i ;
// 			float indx2 = (float)j*0.3;
// 
// 			//		float noi =  pl->perlin(indx1,indx2);
// 			//std::cout<<i<<" "<<j<<std::endl;
// 			//		std::cout<<noi<<std::endl;
// 		//	std::cout<<pl->GetHeight(indx1,indx2)<<std::endl;
// 			(*heimap)(i, j)(0) = pl->GetHeight(indx1,indx2);
// 			(*testbuf)(i,j,0) = pl->GetHeight(indx1,indx2);
// 		}
// 	}
	buffer.Disable_transpancy();

	Mode_Rot = Mat3FromQuaterion(Vector3f(0.0,1.0,0.0),M_PI*0.5f);
	Mode_Rot = Mat3FromQuaterion(Vector3f(1.0,0.0,0.0),M_PI*0.7)*Mode_Rot;
	Mode_Tras += Vector3f(0,0.0,3.5);
 	Sphere* sph = new Sphere(2.0,-2.0,2.0,M_PI*2.0,Mode_Rot,Mode_Tras);
 	sph->Generate_pathches();
 	sph->Deform_init();
	sph->Bind_color_uniform(Vector3f(0.f,1.f,0.f));
	sph->Bind_Texture(heimap);
	sph->Set_transpancy(1.f);

    Mode_Tras += Vector3f(0,0,1);
	Sphere* ssph = new Sphere(0.3,-0.3,0.3,M_PI*2.0,Mode_Rot,Mode_Tras);
	ssph->Generate_pathches();
	ssph->Deform_init();
	ssph->Bind_color_uniform(Vector3f(0,0,1.f));
	ssph->Set_transpancy(0.2f);

	Mode_Tras += Vector3f(-1.5,0,1);
	Cylinder* cyl = new Cylinder(2.0,-2.0,2.0,M_PI*2.0,Mode_Rot,Mode_Tras);
	cyl->Generate_pathches();
	cyl->Deform_init();
	cyl->Bind_color_uniform(Vector3f(1.f,0,0));
	cyl->Set_transpancy(0.2f);
	//cyl->Bind_Texture(heimap);5
	Mode_Tras += Vector3f(0,0,1);
	Cone* con = new Cone(2,4.0,2.0*M_PI,Mode_Rot,Mode_Tras);
	con->Generate_pathches();
	con->Deform_init();
	con->Bind_color_uniform(Vector3f(1.0,1.0,0.0));
	con->Set_transpancy(0.2f);
	
	Torus* tor = new Torus(1.0,0.5,0,M_PI*2.0,M_PI*2.0,Mode_Rot,Mode_Tras);
	tor->Generate_pathches();
	tor->Deform_init();
	tor->Bind_color_uniform(Vector3f(1.f,0,1.f));
	tor->Set_transpancy(1.0f);

	LightPoint light;
	light.Pos = Vector3f(-18,-18,-18);
	light.Amblight = 0.1f;
	light.Color = Vector3f(6.0f,6.0f,6.0f);

 	std::vector<Geometry*> gemos;
 	gemos.push_back(sph);
	//gemos.push_back(ssph);
	//gemos.push_back(cyl);
	//gemos.push_back(con);
	//gemos.push_back(tor);
 	Render_Geomereties_Tesellate(gemos,&trans,&buffer,&smp,&light);
	

 	buffer.save_image_buffer("buffer.bmp");

//	Texture texx("lena.bmp");


	return 0;
}*/
