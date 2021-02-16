//CSCI 5607 HW 2 - Image Conversion Instructor: S. J. Guy <sjguy@umn.edu>
//In this assignment you will load and convert between various image formats.
//Additionally, you will manipulate the stored image data by quantizing, cropping, and supressing channels

#include "image.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#include <fstream>
using namespace std;

/**
 * Image
 **/
Image::Image (int width_, int height_){

    assert(width_ > 0);
    assert(height_ > 0);

    width           = width_;
    height          = height_;
    num_pixels      = width * height;
    sampling_method = IMAGE_SAMPLING_POINT;
    
    data.raw = new uint8_t[num_pixels*4];
		int b = 0; //which byte to write to
		for (int j = 0; j < height; j++){
			for (int i = 0; i < width; i++){
				data.raw[b++] = 0;
				data.raw[b++] = 0;
				data.raw[b++] = 0;
				data.raw[b++] = 0;
			}
		}

    assert(data.raw != NULL);
}

Image::Image (const Image& src){
	width           = src.width;
	height          = src.height;
	num_pixels      = width * height;
	sampling_method = IMAGE_SAMPLING_POINT;
	
	data.raw = new uint8_t[num_pixels*4];
	
	//memcpy(data.raw, src.data.raw, num_pixels);
	*data.raw = *src.data.raw;
}

Image::Image (char* fname){

	int lastc = strlen(fname);
   int numComponents; //(e.g., Y, YA, RGB, or RGBA)
   data.raw = stbi_load(fname, &width, &height, &numComponents, 4);
	
	if (data.raw == NULL){
		printf("Error loading image: %s", fname);
		exit(-1);
	}
	
	num_pixels = width * height;
	sampling_method = IMAGE_SAMPLING_POINT;
	
}

Image::~Image (){
    delete data.raw;
    data.raw = NULL;
}

void Image::Write(char* fname){
	
	int lastc = strlen(fname);

	switch (fname[lastc-1]){
	   case 'g': //jpeg (or jpg) or png
	     if (fname[lastc-2] == 'p' || fname[lastc-2] == 'e') //jpeg or jpg
	        stbi_write_jpg(fname, width, height, 4, data.raw, 95);  //95% jpeg quality
	     else //png
	        stbi_write_png(fname, width, height, 4, data.raw, width*4);
	     break;
	   case 'a': //tga (targa)
	     stbi_write_tga(fname, width, height, 4, data.raw);
	     break;
	   case 'p': //bmp
	   default:
	     stbi_write_bmp(fname, width, height, 4, data.raw);
	}
}


void Image::Brighten (double factor){
	int x,y;
	for (x = 0 ; x < Width() ; x++){
		for (y = 0 ; y < Height() ; y++){
			Pixel p = GetPixel(x, y);
			Pixel scaled_p = p*factor;
			GetPixel(x,y) = scaled_p;
		}
	}
}

void Image::ExtractChannel(int channel){
	uint8_t data[] = {0,0,0,1};
   data[channel] = 1;
   Pixel q = Pixel(data);

   int x,y;
	for (x = 0 ; x < Width() ; x++){
		for (y = 0 ; y < Height() ; y++){
			Pixel p = GetPixel(x, y);
			Pixel new_p = p*q;
			GetPixel(x,y) = new_p;
		}
	}
}


void Image::Quantize (int nbits){
	int x,y;
   for (x=0;x<Width();x++){
      for (y=0;y<Height();y++){
         Pixel p = GetPixel(x,y);
         Pixel new_p = PixelQuant(p,nbits);
         GetPixel(x,y) = new_p;
      }
   }
}

Image* Image::Crop(int x, int y, int w, int h){
	Image *nimg = new Image(w,h);
	int i,j;
	for (i=0;i<w;i++){
		for (j=0;j<h;j++){
			Pixel p=GetPixel(x+i,y+j);
			nimg->GetPixel(i,j)=p;
		}
	}
	return nimg;
}


void Image::AddNoise (double factor){
	// srand(time(NULL));
	// int noise[4] = {rand()%256*factor,rand()%256*factor,rand()%256*factor,0}; //generate a noise vector, assuming 8 bit
	int x,y;
	for (x=0;x<Width();x++){
		for (y=0;y<Height();y++){
			Pixel p=GetPixel(x,y);
			Pixel new_p=PixelRandom();
			GetPixel(x,y)=p+(new_p*factor);
		}
	}
}

void Image::ChangeContrast (double factor){
	int x,y;
	float sum=0.0;
	for (x=0;x<Width();x++){
		for (y=0;y<Height();y++){
			Pixel p=GetPixel(x,y);
			sum += (float)p.Luminance()/255.0;
		}
	}
	int l_mean=(int)(sum/num_pixels);
	for (x=0;x<Width();x++){
		for (y=0;y<Height();y++){
			Pixel p=GetPixel(x,y);
			GetPixel(x,y)=Pixel(ComponentLerp(p.r,l_mean,factor),ComponentLerp(p.g,l_mean,factor),ComponentLerp(p.b,l_mean,factor),p.a);
		}
	}
}

void Image::ChangeSaturation(double factor){
	int x,y;
	for (x=0;x<Width();x++){
		for (y=0;y<Height();y++){
			Pixel p=GetPixel(x,y);
			Component l=p.Luminance();
			Pixel q= Pixel(l,l,l,255);
			GetPixel(x,y)=PixelLerp(p,q,factor);
		}
	}
}


//For full credit, check that your dithers aren't making the pictures systematically brighter or darker
void Image::RandomDither (int nbits){
	/* WORK HERE */
}

//This bayer method gives the quantization thresholds for an ordered dither.
//This is a 4x4 dither pattern, assumes the values are quantized to 16 levels.
//You can either expand this to a larger bayer pattern. Or (more likely), scale
//the threshold based on the target quantization levels.
static int Bayer4[4][4] ={
    {15,  7, 13,  5},
    { 3, 11,  1,  9},
    {12,  4, 14,  6},
    { 0,  8,  2, 10}
};


void Image::OrderedDither(int nbits){
	/* WORK HERE */
}

/* Error-diffusion parameters */
const double
    ALPHA = 7.0 / 16.0,
    BETA  = 3.0 / 16.0,
    GAMMA = 5.0 / 16.0,
    DELTA = 1.0 / 16.0;

void Image::FloydSteinbergDither(int nbits){
	/* WORK HERE */
}

void Image::Blur(int n){
   // float r, g, b; //I got better results converting everything to floats, then converting back to bytes
	// Image* img_copy = new Image(*this); //This is will copying the image, so you can read the orginal values for filtering (
                                          //  ... don't forget to delete the copy!
	/* WORK HERE */
}

void Image::Sharpen(int n){
	/* WORK HERE */
}

void Image::EdgeDetect(){
	/* WORK HERE */
}

Image* Image::Scale(double sx, double sy){
	/* WORK HERE */
	return NULL;
}

Image* Image::Rotate(double angle){
	/* WORK HERE */
	return NULL;
}

void Image::Fun(){
	/* WORK HERE */
}

/**
 * Image Sample
 **/
void Image::SetSamplingMethod(int method){
   assert((method >= 0) && (method < IMAGE_N_SAMPLING_METHODS));
   sampling_method = method;
}


Pixel Image::Sample (double u, double v){
   /* WORK HERE */
   return Pixel();
}