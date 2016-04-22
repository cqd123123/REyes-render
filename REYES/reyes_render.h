#ifndef REYES_RENDER
#define REYES_RENDER

#include "Image_Buffer.h"
#include "Transform.h"
#include "mesh.h"
#include "Sampler.h"
#include "Patch.h"
#include "shape.h"
#include "shader.h"


#include <vector>


extern const float RI_TESELLATION_LEVEL;

//render meshes per quad basis
void Render_meshes_quad_point(Image_Buffer* buf, TransformV* trans, std::vector<Mesh*>& meshes){
	//ierate over all meshes
	int _w = buf->get_width();
	int _h = buf->get_height();

	for(int i=0;i<meshes.size();i++)
	{
		Mesh* _mesh = meshes[i];
		int total_quad = _mesh->get_numof_quad();
		
		//if mesh bounding box lie in image
		Vector3f transBoxMin, transBoxMax;
		trans->TransVertex(_mesh->BoundMin, transBoxMin);
		trans->TransVertex(_mesh->BoundMax, transBoxMax);
		if(trans->OutofImagePlane(transBoxMin) && trans->OutofImagePlane(transBoxMax)){std::cout<<"mesh culled"<<std::endl; continue;}
		

		for(int j=0 ;j< total_quad;j++)
		{
			Quad& _q = _mesh->get_quads(j);
			
			//process each vertexs
			for(int k=0;k<_q.vertexs.size();k++)
			{
				
				Vertex* _v = _q.vertexs[k];
				Vector3f pos = _v->position;
				//pos = _q.BoxMax;
				Vector3f TransPos;
				trans->TransVertex(pos,TransPos);
				//std::cout<<pos<<std::endl;
				if(trans->OutofImagePlane(TransPos)) continue;
				
				int x = std::min(_w - 1, (int)((TransPos(0) + 1) * 0.5 * _w)); 
				int y = std::min(_h - 1, (int)((1 - (TransPos(1) + 1) * 0.5) * _h));

				//assign color to vertex color
				Vector3f _color = _v->Color;
				(*buf)(x,y,0) = _color(0);
				(*buf)(x,y,1) = _color(1);
				(*buf)(x,y,2) = _color(2);
			}
		}
	}
}

//major render cycle:
int Path_visited = 0;
void Vist_Pathes(Geometry* geom,patch* patches, TransformV* trans, Image_Buffer* buf, sampler2D* sampler, LightPoint* lig){

	Vector3f geom_color;
	if(geom->is_uniform_color())
		geom_color = geom->get_geom_color();
	Texture* _tex = geom->get_texture();
	bool height_comp = false;
	bool deformation_comp = false;
	if(_tex) height_comp =_tex->is_height;
	if(_tex) deformation_comp = _tex->is_deformation;
	int Sufrace_kind = geom->Surface_status;
	float Tpancy = geom->Get_transpancy();
	bool is_tpancy = buf->Get_tranpancy();
	int _w, _h;
	Path_visited ++;
	_w = buf->get_width();
	_h = buf->get_height();

	Vector3f vetpos[4];
	Vector3f vettra[4];
	for(int i=0;i<4;i++)
		vetpos[i] = patches->p_vs[i];
	
	//deformation
	if(deformation_comp)
	{
		Displacement_shader_new(vetpos,_tex, patches,0.3);
	}

	for(int i=0;i<4;i++)
	{
		trans->TransVertex(vetpos[i],vettra[i]);
	}
	//cull quad out of frustum
	//pathch out of frustum, its child must outside frustum
	if(trans->QuadOutofFrustum(vettra)){ delete patches; return;}	//goto visitChild;

	Vector2f img_vert[4];
	for(int i=0;i<4;i++)
	{
		img_vert[i](0) = (vettra[i](0) + 1) * 0.5 * _w;
		img_vert[i](1) = (1 - (vettra[i](1) + 1) * 0.5) * _h;
	}
	//get image space bounding box
	Vector2f boxMin(FLT_MAX,FLT_MAX);
	Vector2f boxMax(-FLT_MAX,-FLT_MAX);
	for(int i=0;i<4;i++)
	{
		boxMin(0) = min(boxMin(0), img_vert[i](0));
		boxMin(1) = min(boxMin(1), img_vert[i](1));
		boxMax(0) = max(boxMax(0), img_vert[i](0));
		boxMax(1) = max(boxMax(1), img_vert[i](1));
	}
	float large_side = (max(boxMax(0) - boxMin(0), boxMax(1) - boxMin(1)));
	float Area = (boxMax(0) - boxMin(0)) * (boxMax(1) - boxMin(1));
	if(large_side > RI_TESELLATION_LEVEL)	//diceable
	{
		//dice into 4 childs
		geom->Tesellate_patch(patches);
		//std::cout<<"tesellated "<<std::endl;
		for(int i=0;i<4;i++)
 			geom->Deform_Pathes(patches->childs[i]);
	}
	else	//render the patch
	{
			//iterate over nearby pixel samples
			int x_start = (int)boxMin(0) ;
			int y_start = (int)boxMin(1) ;
			int x_end   = (int)boxMax(0)+1;
			int y_end   = (int)boxMax(1)+1;
			x_start = clamp(x_start,0,_w-1);
			x_end   = clamp(x_end,0,_w-1);
			y_start = clamp(y_start,0,_h-1);
			y_end   = clamp(y_end,0,_h-1);

			for(int j=y_start; j<=y_end;j++)
			{
				for(int i=x_start;i<=x_end;i++)
				{
					Vector3f pixelColor;
					pixelColor.setZero();
					float frag_depth=0;
						//std::cout<<"render1" <<std::endl;
					//iterate over all samples
					int xspp = sampler->get_xspp();
					int yspp = sampler->get_yspp();
					//std::cout<<"render2" <<std::endl;
					bool _update = false;
					for(int z=0;z<yspp;z++)
					{
						for(int k=0;k<xspp;k++)
						{
							//sample position + pixel base position
							Vector2f samplePos = (*sampler)(k,z);
							Vector3f sampleColor;
							sampleColor.setZero();
							samplePos(0) += (float)i;
							samplePos(1) += (float)j;
							float sample_depth;
							//if sample lie in quad
							if(PointInTriangle2D(samplePos,img_vert[0],img_vert[1],img_vert[2]))
							{
								Vector3f weig = BayercentricInterpolation(samplePos,img_vert[0],img_vert[1],img_vert[2]);
								Vector3f smp_N = (weig(0)*patches->Normal[0] + weig(1)*patches->Normal[1] + weig(2)*patches->Normal[2]);
								Vector3f smp_pos_3D  = (weig(0)*vettra[0] + weig(1)*vettra[1] + weig(2)*vettra[2]);
								Vector3f smp_para = (weig(0)*patches->parameter[0] + weig(1)*patches->parameter[1] + weig(2)*patches->parameter[2]);
								
								
							    //if(!height_comp)
								Vector3f _texcol  = Texture_Shader(smp_N,smp_pos_3D,smp_para,_tex);
								if(_tex&&!deformation_comp&&!height_comp) geom_color = _texcol;

								if(height_comp)
								{
									Vector3f heis = get_height_Trangle(patches->parameter[0],patches->parameter[1],patches->parameter[2],_tex);
									Vector3f PA1  = patches->p_vs[0] + patches->Normal[0]*heis(0);
									Vector3f PA2  = patches->p_vs[1] + patches->Normal[1]*heis(1);
									Vector3f PA3  = patches->p_vs[2] + patches->Normal[2]*heis(2);
									Vector3f New_norm = Displacement_shader(smp_N, PA1,PA2,PA3);
									smp_N = New_norm;
								}
								if(Sufrace_kind == 1)
								{
									geom_color = CheckBoard_Shader(smp_para,geom->checkborad_Nw,geom->checkborad_Nh);
								}
								Vector3f _shadcol = Labmert_Shader(lig,smp_N,smp_pos_3D,trans->getCameraPos(),geom_color, is_tpancy);

								sampleColor = _shadcol;
								sample_depth = smp_pos_3D(2);
								
								_update = true;
							}
							else
								if(PointInTriangle2D(samplePos,img_vert[1],img_vert[2],img_vert[3]))
								{
									Vector3f weig = BayercentricInterpolation(samplePos,img_vert[1],img_vert[2],img_vert[3]);
									Vector3f smp_N = (weig(0)*patches->Normal[1] + weig(1)*patches->Normal[2] + weig(2)*patches->Normal[3]);
									Vector3f smp_pos_3D  = (weig(0)*vettra[1] + weig(1)*vettra[2] + weig(2)*vettra[3]);
									Vector3f smp_para = (weig(0)*patches->parameter[1] + weig(1)*patches->parameter[2] + weig(2)*patches->parameter[3]);
									if(height_comp)
									{
										Vector3f heis = get_height_Trangle(patches->parameter[1],patches->parameter[2],patches->parameter[3],_tex);
										Vector3f PA1  = patches->p_vs[1] + patches->Normal[1]*heis(0);
										Vector3f PA2  = patches->p_vs[2] + patches->Normal[2]*heis(1);
										Vector3f PA3  = patches->p_vs[3] + patches->Normal[3]*heis(2);
										Vector3f New_norm = Displacement_shader(smp_N, PA1,PA2,PA3);
										smp_N = New_norm;
									}
									if(Sufrace_kind == 1)
									{
										geom_color = CheckBoard_Shader(smp_para,geom->checkborad_Nw,geom->checkborad_Nh);
									}
									
									Vector3f _texcol  = Texture_Shader(smp_N,smp_pos_3D,smp_para,_tex);
									if(_tex&&!deformation_comp&&!height_comp) geom_color = _texcol;

									Vector3f _shadcol = Labmert_Shader(lig,smp_N,smp_pos_3D,trans->getCameraPos(),geom_color,is_tpancy);

									sampleColor = _shadcol;
									sample_depth = smp_pos_3D(2);
									_update = true;
								}
								else{continue;}
								//add sampleColor to pixelcolor
								pixelColor += sampleColor;
								frag_depth += sample_depth;
						}

					}
					//average over samples
					if(_update)
					{
						pixelColor /= xspp*yspp;
						frag_depth /= xspp*yspp;
						buf->take_frag_color(i,j,pixelColor,frag_depth,Tpancy);
					}
				}
			}
			

		}
if(patches->childs.size()!=0)
		{
			int total = patches->childs.size();
			for(int i=0;i<total;i++)
				Vist_Pathes(geom,patches->childs[i], trans,buf,sampler, lig);
		}

//discard patch after rendered
	delete patches;
}

void Render_Geomereties_Tesellate(std::vector<Geometry*> Geoms,  TransformV* trans, Image_Buffer* buf, sampler2D* samp, LightPoint* lig)
{
	for(int i=0;i<Geoms.size();i++)
	{
		Geometry* _geom = Geoms[i];
		for(int j=0;j<_geom->get_num_of_patch();j++)
		{
			std::cout<<"patch "<<j<<std::endl;
			Vist_Pathes(_geom, _geom->get_patch(j),trans,buf,samp, lig);
		}
	}
}
#endif