#ifndef IMAGE_BUFFER
#define IMAGE_BUFFER

#include <stdlib.h>
#include "CImg.h"
#include "util.h"
#include <cassert>
#include "Eigen/Eigen"
#include <vector>

using namespace Eigen;


using namespace cimg_library;

struct Pixel{
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	Vector3f color;
	float Z_value;
	float Tpancy;
};

struct PixelArray{
	std::vector<Pixel> pixls;
};


class Image_Buffer
{
public:
// 	inline float& operator()(int x, int y, int z) { 
// 		assert(x<_Width&&x>=0&&y<_Height&&y>=0&&z<3&&z>=0);
// 		return data_buffer[(x + y*_Width)*3 + z]; 
// 	};
	inline float& operator()(int x, int y, int z){
		assert(x<_Width&&x>=0&&y<_Height&&y>=0&&z<3&&z>=0);
		return pixel2D[x + y*_Width].pixls[0].color(z);
	}
	inline PixelArray& operator()(int x, int y){
		assert(x<_Width&&x>=0&&y<_Height&&y>=0);
		return pixel2D[x + y*_Width];
	}

	Image_Buffer(int Width, int Height)
	{
		int total_size = Width*Height*3;
		z_buffer = (float *) malloc(sizeof(float)*Width*Height);
		//memset(z_buffer, FLT_MAX, sizeof(float)*Width*Height);
		for(int i=0; i<Width*Height;i++)
			z_buffer[i] = FLT_MAX;

		_Width =Width;
		_Height = Height;

		//pixels = (Pixel*) malloc(sizeof(Pixel)*_Width*_Height);
		for(int i=0; i<_Width*_Height;i++)
		{
			Pixel pix;
			pix.color = Vector3f(0,0,0);
			pix.Tpancy = 0.0f;
			pix.Z_value = 0.0;

			PixelArray pa;
			pa.pixls.push_back(pix);

			pixel2D.push_back(pa);
		}
		_transparent_enabled = false;
	}
	int get_width() {return _Width;}
	int get_height() {return _Height;}

	//z buffer !! if near z value, replace the color
	inline void take_frag_color(int x, int y, Vector3f color, float _z, float _tranpcy)
	{
		//compare z value with z buffer
		if(!_transparent_enabled)
		{
			if(_z < z_buffer[x + y*_Width])
			{
				z_buffer[x + y*_Width] = _z;
				(*this)(x,y).pixls[0].color = color;
			}
		}
		else	//save every thing
		{		
			if(_tranpcy>1.0 || _tranpcy<0.0) std::cout<<"warning invalid transparency value"<<std::endl;
			
				Pixel pix;
				pix.color = color;
				pix.Z_value = _z;
				pix.Tpancy = _tranpcy;
				(*this)(x,y).pixls.push_back(pix);
		}
	}
	void save_image_buffer(std::string name){
		CImg<float> image;
		image.resize(_Width,_Height,1,3);
		
		
		if(!_transparent_enabled)
		{
			for(int j=0;j<_Height;j++)
			{
				for(int i=0;i<_Width;i++)
				{
					for(int k=0;k<3;k++)
						image(i,j,k) = clamp((*this)(i,j,k)*255.0,0.0,255.0);
				}
			}
		}
		else{

			//sort pixels from far to near pixelarray [0] far [1] near
			for(int j=0;j<_Height;j++)
			{
				for(int i=0;i<_Width;i++)
				{
					PixelArray& pa = (*this)(i,j);
					for(int k=0;k<pa.pixls.size();k++)
					{
						for(int z=k;z<pa.pixls.size();z++)
						{
							Pixel tmp;
							if(pa.pixls[k].Z_value < pa.pixls[z].Z_value)
							{
								tmp = pa.pixls[k];
								pa.pixls[k] = pa.pixls[z];
								pa.pixls[z] = tmp;
							}
						}
					}
				}
			}

			//composite color
			for(int j=0;j<_Height;j++)
			{
				for(int i=0;i<_Width;i++)
				{
					PixelArray pa = (*this)(i,j);
					Vector3f fcolor = Vector3f(0,0,0);	//black background
					
					for(int k=0;k<pa.pixls.size();k++)
					{
						Pixel px = pa.pixls[k];
						fcolor = px.color*px.Tpancy + (1.f - px.Tpancy)*fcolor;

					}

					for(int k=0;k<3;k++)
						image(i,j,k) = clamp(fcolor(k)*255.0,0.0,255.0);
				}
			}
		}
		
		
		image.save(name.c_str());
	}
	void Enable_Transpancy(){
		_transparent_enabled = true;
	}
	void Disable_transpancy(){
		_transparent_enabled = false;
	}
	bool Get_tranpancy(){return _transparent_enabled;}
private:
	int _Width;
	int _Height;
	float* z_buffer;
	std::vector<PixelArray> pixel2D;
	bool _transparent_enabled;

};

class Texture{
public:
	
	inline Vector3f& operator()(int u, int v){
		if(!(u<_w&&u>=0&&v<_h&&v>=0)){//std::cout<<u<<" "<<v<<std::endl;
		u = clamp(u,0,_w-1);
		v = clamp(v,0,_h-1);}
		return _data[u + v*_w];
	}

	inline Vector3f Linear_interpolate_val(Vector2f pos){
		//assert(pos(0)<=1.f&&pos(0)>=0.f&&pos(1)<=1.f&&pos(1)>=0.f);
		if(!(pos(0)<=1.f&&pos(0)>=0.f&&pos(1)<=1.f&&pos(1)>=0.f))
		{
			//std::cout<<pos(0)<<"  "<<pos(1)<<std::endl;
			//pos(0) = clamp(pos(0),0.f,1.f);
			//pos(1) = clamp(pos(1),0.f,1.f);
			//return Vector3f(0,0,0);
			pos(0) = rand()/RAND_MAX;
			pos(1) = rand()/RAND_MAX;
		}
		float u = pos(0)*_w;
		float v = pos(1)*_h;

		int u0 = (int)(pos(0)*_w);
		int v0 = (int)(pos(1)*_h);
		int u1 = min(u0+1, _w -1);
		int v1 = min(v0+1, _h -1);
		
		//interpolate
		
		Vector3f cx0 = LinearInterpolate(u, u0, u1, (*this)(u0,v0), (*this)(u1,v0));
		Vector3f cx1 = LinearInterpolate(u, u0, u1, (*this)(u0,v1), (*this)(u1,v1));
		Vector3f fcl = LinearInterpolate(v, v0, v1, cx0, cx1);
		return fcl;
	}
	//is_h whether is height map
	Texture(std::string filename, bool is_h):is_height(is_h){
		CImg<float> _tex(filename.c_str());
		_w = _tex.width();
		_h = _tex.height();

		_data =  (Vector3f*)malloc(sizeof(Vector3f)*_w*_h);

		for(int j=0;j<_h;j++)
		{
			for(int i=0;i<_w;i++)
			{
				
				Vector3f texcol;
				texcol(0) = _tex(i,j,0);
				texcol(1) = _tex(i,j,1);
				texcol(2) = _tex(i,j,2);
				(*this)(i,j) = texcol / 255.0f; //scale to [0-1]
			}
		}
		is_deformation = false;
	}
	//is_h whether is height map
	Texture(int ww, int hh, bool is_h):_w(ww),_h(hh), is_height(is_h)
	{
		_data =  (Vector3f*)malloc(sizeof(Vector3f)*_w*_h);
	}
	bool is_height;
	bool is_deformation;
private:
	Vector3f* _data;
	
	int _w;
	int _h;
};



#endif