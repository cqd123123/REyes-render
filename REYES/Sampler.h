#ifndef SAMPLER_H
#define SAMPLER_H

#include <stdlib.h>
#include <cassert>
#include "Eigen/Eigen"
using namespace Eigen;


//a sampler for one pixel, sample offset range from [0-1]^2
class sampler2D{

public:

	//x sample in x axis, y sample in y axis
	inline Vector2f& operator()(int x, int y) { 
		assert(x<x_spp&&x>=0&&y<y_spp&&y>=0);
		 return samples[x + y*x_spp]; 
	};

	int get_xspp() {return x_spp;}
	int get_yspp() {return y_spp;}

	sampler2D(int numx_spp, int numy_spp):x_spp(numx_spp), y_spp(numy_spp)
	{
		samples = (Vector2f*)malloc(sizeof(Vector2f)*x_spp*y_spp);
		memset(samples,0x00, sizeof(Vector2f)*x_spp*y_spp);
	}

	void GenerateRndStratifiedSmps()
	{
		float _dx = 1.0f/x_spp;
		float _dy = 1.0f/y_spp;
		//place uniform in a grid
		for(int j=0;j<y_spp;j++)
		{
			for(int i=0;i<x_spp;i++)
			{
				float xspos = 0.5*_dx + i*_dx;
				float yspos = 0.5*_dy + j*_dy;
				//rnd jitter
				float randx = ((float) rand()) / (float) RAND_MAX;	// in the range [0 -1]
				float randy = ((float) rand()) / (float) RAND_MAX;
				xspos = xspos + (randx - 0.5f)*_dx;				//in the range [-0.5dx, 0.5dx]
				yspos = yspos + (randy - 0.5f)*_dy;
				Vector2f samp_pos;
				samp_pos(0) = xspos;
				samp_pos(1) = yspos;
				samples[i + j*x_spp] = samp_pos;
			}
		}
	}
	void Debug()
	{
		std::cout<<"sampler debug"<<std::endl;
		for(int i=0;i<x_spp*y_spp;i++)
			std::cout<<samples[i]<<std::endl;
	}
private:
	int x_spp;
	int y_spp;
	Vector2f* samples;
};
#endif