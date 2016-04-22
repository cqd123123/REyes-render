#ifndef TESTSCENE_H
#define TESTSCENE_H

#include "Ri.h"



void SampleScene1(void) {
	int i;
	int nf;
	float slopex,slopey,slopez;
	char name[50];

	RtColor red={1,0,0};
	RtColor green={0,1,0};
	RtColor blue={0,0,1};
	RtColor white={1,1,1};


	RtPoint p1={30,0,10}; /* ball's initial position */
	RtPoint p2={0,20,10}; /* ball's final position  */


	RtFloat fov = 45;
	RtFloat intensity1=0.1;
	RtFloat intensity2=1.5;
	RtInt init=0,end=1;


	nf = 100; /* number of frames to output */
	slopex = (p2[0]-p1[0])/nf;
	slopey = (p2[1]-p1[1])/nf;
	slopez = (p2[2]-p1[2])/nf;

	RiBegin(RI_NULL);
	RiFormat(320,240,1);
	RiPixelSamples(2,2);
	RiShutter(0,1);

	/* loop through all the frames */
	for (i = 1; i <= nf; i++) {
		RiFrameBegin(i);
		sprintf(name,"image_%02d.tif", i-1);
		RiDisplay(name, "file", "rgb");

		RiProjection("perspective", "fov", &fov);
		RiTranslate(0,-5,60);
		RiRotate(-120,1,0,0);
		RiRotate(25,0,0,1);

		RiCameraRotate(180,0,0,1);
		RiCameraTranslate(0,0,3);

		RiWorldBegin();
		RiColor(blue);

		RiTransformBegin();
		RiCylinder(1, 0, 20, 360);
		RiTranslate(0, 0, 20);
		RiCone(2, 2, 360);
		RiTransformEnd();

		RiColor(green);
		RiTransformBegin();
		RiRotate(-90, 1, 0, 0);
		RiCylinder(1, 0, 20, 360);
		RiTranslate(0, 0, 20);
		RiCone(2, 2, 360);
		RiTransformEnd();

		RiColor(red);
		RiTransformBegin();
		RiRotate(90, 0, 1, 0);
		RiCylinder(1, 0, 20, 360);
		RiTranslate(0, 0, 20);
		RiCone(2, 2, 360);
		RiTransformEnd();


		RiColor(white);
		RiTransformBegin();
		RiTranslate(p1[0] + slopex * (i-1), p1[1] + slopey * (i-1), p1[2] + slopez * (i-1));
		RiSphere(5, -5, 5, 360);
		RiTransformEnd();
		RiWorldEnd();

		/* when you hit this command you should output the final image for this frame */
		RiFrameEnd();
	}
	
	RiEnd();
};


void TransparencyTest1()
{
	RiBegin(RI_NULL);
	//RiFormat(960, 720, 1.0);
	RiFormat(400, 400, 1.0);
	//RiFormat(200, 150, 1.0);
	RiDisplay("TransparencyTest.png", "", "");
	RiPixelSamples(2,2);
for (int i = 1; i <= 1; i++) {
	RiFrameBegin(i);
	RiEnableOpacity();
	/* set the perspective transformation */
	float fov = 45.0;
	RiProjection(RI_PERSPECTIVE, "fov", &fov);
	//RiProjection(RI_ORTHOGRAPHIC);
	float campos = -1 + i*0.2;
	RiCameraRotate(180,0,0,1);
	RiCameraTranslate(0,0,3);

	RiWorldBegin();
	RiLight(-18,-18,-18,6,6,6);
	RiTransformBegin();
	RtColor color = {1,0,0};
	RtColor opacity = {0.2,0.2,0.2};
	RiOpacity(opacity);
	RiColor(color);
	RiTranslate(0, 0.5, 7.0);
	RiRotate(60, 1, 0, 0);
	RiTorus(1, .25, 0, 250, 270);
	RiTransformEnd();
	RiTransformBegin();
	color[0] = 0; color[1] = 1;
	opacity[0] = 0.1; opacity[1] = 0.1; opacity[2] = 0.1;
	RiOpacity(opacity);
	RiColor(color);
	RiTranslate(0, 0, 8.0);
	RiRotate(60, 1, 0, 0);
	RiRotate(30, 0, 1, 0);
	RiCylinder(1, -1, 1, 300);
	RiTransformEnd();
	RiTransformBegin();
	color[1] = 0; color[2] = 1;
	opacity[0] = 0.35; opacity[1] = .35; opacity[2] = .35;
	RiOpacity(opacity);
	RiColor(color);
	RiTranslate(0, 1, 9.0);
	RiRotate(60, 1, 0, 0);
	RiSphere(1.8, -1.0, 1.0, 250);
	RiTransformEnd();
	RiTransformBegin();
	color[0] = 1; color[1] = .6; color[2] = .6;
	RiColor(color);
	opacity[0] = .1; opacity[1] = .1; opacity[2] = .1;
	RiOpacity(opacity);
	RiTranslate(0, 0, 8.5);
	RiRotate(-160, 1, 0, 0);
	RiRotate(30, 0, 1, 0);
	RiRotate(140, 0, 0, 1);
	RiCone(1, 2.5, 180);
	RiTransformEnd();

	RiWorldEnd();

	RiFrameEnd();
}
	RiEnd();
}


void ShaderTest()
{
	RiBegin(RI_NULL);
	//RiFormat(960, 720, 1.0);
	RiFormat(1600, 1600, 1.0);
	//RiFormat(200, 150, 1.0);
	RiDisplay("ShaderTest.png", "", "");
	RiPixelSamples(2,2);

	RiFrameBegin(0);
	/* set the perspective transformation */
	float fov = 45.0;
	RiProjection(RI_PERSPECTIVE, "fov", &fov);
	//RiProjection(RI_ORTHOGRAPHIC);
	RiCameraRotate(180,0,0,1);
	RiCameraTranslate(0,0,0);

	//RiEnableOpacity();
	RiWorldBegin();
	RiLight(18,18,18,8,8,8);
	RiLigthAmb(0.2);
	RiTransformBegin();
	RtColor color = {1,0,0};
	RtColor opacity = {.4,.4,.4};
	RiOpacity(opacity);
	RiColor(color);
	RI_CHECK_SIZE_X = 20;
	RI_CHECK_SIZE_Y = 10;
	RiSurface(CHECKBOARD);
	RiTranslate(0, 0.5, 11);
	RiRotate(70, 0, 1, 0);
	RiTorus(2.5, .5, 0, 360, 360);
	RiTransformEnd();

	RiTransformBegin();
	color[0] = 0; color[1] = 1; color[2] = 0;
	opacity[0] = 1; opacity[1] = 1; opacity[2] = 1;
	RiOpacity(opacity);
//	RiSurface(NULL);
	RiColor(color);
	RiTranslate(3, -1.5, 11);
	RiRotate(90, 1, 0, 0);
	RiCylinder(1, -1, 1, 360);
	RiTransformEnd();

	RiTransformBegin();
	opacity[0] = 1; opacity[1] = 1; opacity[2] = 1;
	RiOpacity(opacity);
	

    RiDisplacementTexture("hei1.bmp");
	RIBindBumpTexture(0);
	RiTranslate(3, 1, 11.0);
	RiSphere(1, -1.5, 1.5, 360);
	RiTransformEnd();

	RiTransformBegin();
	opacity[0] = .8; opacity[1] = .8; opacity[2] = .8;
	RiOpacity(opacity);
//	RiSurface(earthShader);
	RiTranslate(-2.5, 1, 11.0);
	RiRotate(-175, 0, 1, 0);
	RiRotate(110, 1, 0, 0);
	RiMakeTexture("lena.bmp", 0);
	RIBindTexture(0);
	RiSphere(1.5, -1.5, 1.5, 360);
	RiTransformEnd();

	RiTransformBegin();
	color[0] = 0.4; color[1] = 0.2; color[2] = 0;
	RiColor(color);
	opacity[0] = 1; opacity[1] = 1; opacity[2] = 1;
	RiOpacity(opacity);
//	RiSurface(NULL);
	RiDisplacementTexture("hei.bmp");
	RIBindBumpTexture(1);
	RiTranslate(-2.5, -2.5, 11);
	RiRotate(-90, 1, 0, 0);
	RiRotate(-20,0,1,0);
	RiCone(1, 2.5, 360);
	RiTransformEnd();
	RiWorldEnd();

	RiFrameEnd();

	RiEnd();
}


void Tunnel()
{
	RiBegin(RI_NULL);
	//RiFormat(960, 720, 1.0);
	RiFormat(1600, 1600, 1.0);
	RiDisplay("Tunnel.png", "", "");
	RiPixelSamples(2,2);

	RiFrameBegin(0);
	/* set the perspective transformation */
	float fov = 45.0;
	RiProjection(RI_PERSPECTIVE, "fov", &fov);
	//RiProjection(RI_ORTHOGRAPHIC);

	RiCameraRotate(180,0,0,1);
	RiCameraTranslate(0,0,0);

	RiWorldBegin();
	RiTransformBegin();
	RtColor color = {1,0,0};
	RiColor(color);
	RiTranslate(0, 0.5, 7.0);
	RiRotate(60, 1, 0, 0);
	RiTorus(1, .25, 0, 360, 360);
	RiTransformEnd();
	RiTransformBegin();
	color[0] = 0; color[1] = 1;
	RiColor(color);
	RiTranslate(0, 0, 8.0);
	RiRotate(60, 1, 0, 0);
	RiRotate(30, 0, 1, 0);
	RiCylinder(1, -1, 1, 360);
	RiTransformEnd();
	RiTransformBegin();
	color[1] = 0; color[2] = 1;
	RiColor(color);
	RiTranslate(0, 1, 9.0);
	RiRotate(60, 1, 0, 0);
	RiSphere(1.0, -1.0, 1.0, 360);
	RiTransformEnd();
	RiTransformBegin();
	color[0] = 1; color[1] = .4; color[2] = .4;
	RiColor(color);
	RI_CHECK_SIZE_X = 40;
	RI_CHECK_SIZE_Y = 40;
	RiSurface(CHECKBOARD);

	RiTranslate(0, -1, 8.5);
	RiRotate(-160, 1, 0, 0);
	RiRotate(30, 0, 1, 0);
	RiCone(2, 1, 360);
	RiTransformEnd();
	RiTransformBegin();
	RI_CHECK_SIZE_X = 40;
	RI_CHECK_SIZE_Y = 40;
	RiSurface(CHECKBOARD);
	RiTranslate(0, 0, 7.0);
	RiCylinder(3, 0, 10, 360);
	RiTransformEnd();
	RiWorldEnd();

	RiFrameEnd();

	RiEnd();
}


void Tornado()
{
	RiBegin(RI_NULL);
	RiFormat(960, 720, 1.0);
	//RiFormat(400, 300, 1.0);
	//RiFormat(200, 150, 1.0);
	RiDisplay("Tornado.png", "", "");
	RiPixelSamples(2,2);

	RiFrameBegin(0);
	/* set the perspective transformation */
	float fov = 45.0;
	RiProjection(RI_PERSPECTIVE, "fov", &fov);
	//RiProjection(RI_ORTHOGRAPHIC);
	RiCameraRotate(180,0,0,1);
	RiCameraTranslate(0,0,0);

	RiWorldBegin();
	RiTransformBegin();
	RtColor gray = { .4, .4, .4 };
	RtColor opacity = { 0.5, 0.5, 0.5 };
	RiColor(gray);
	RiOpacity(opacity);
	RiDisplacementTexture("hei1.bmp");
	RIBindBumpTexture(0);
	RiTranslate(0, 0, 5.0);
	RiRotate(-270, 1, 0, 0);
	RiRotate(-20,0,1,0);
	RiTranslate(0, 2, -1.5);
	RiCone(0.5, 4, 360);					
	RiTransformEnd();
	RiWorldEnd();

	RiFrameEnd();

	RiEnd();
}
void Earth()
{
	RiBegin(RI_NULL);
	//RiFormat(960, 720, 1.0);
	RiFormat(800, 800, 1.0);
	//RiFormat(200, 150, 1.0);
	RiDisplay("Earth.png", "", "");
	RiPixelSamples(2,2);

	RiFrameBegin(0);
	/* set the perspective transformation */
	float fov = 45.0;
	RiProjection(RI_PERSPECTIVE, "fov", &fov);
	//RiProjection(RI_ORTHOGRAPHIC);

	RiWorldBegin();
	RiTransformBegin();
	RiMakeTexture("earth.bmp", 0);
	RIBindTexture(0);
	RtColor blue = { 0, 0, 1 };
	RtColor opacity = { .9, .9, .9 };
	RiColor(blue);
	RiOpacity(opacity);
//	BUMP_AMPLITUDE = .02;
//	BUMP_MIN_FREQ_EXP = 14;
//	BUMP_MAX_FREQ_EXP = 16;
//	RiDisplacement(BUMPY);
	RiTranslate(0, 0, 5.0);
	RiRotate(-175, 0, 1, 0);
	RiRotate(110, 1, 0, 0);
	RiSphere(1, -1, 1, 360);
	RiTransformEnd();
	RiWorldEnd();

	RiFrameEnd();

	RiEnd();
}

#endif