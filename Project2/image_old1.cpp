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

#define PI 3.1415 

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
	
	memcpy(data.raw, src.data.raw, num_pixels*sizeof(Pixel));
	// *data.raw = *src.data.raw;
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
    delete[] data.raw;
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
	x = (x<0)? 0 : (x>Width())? (Width()-1):x;
   y = (y<0)? 0 : (y>Height())? (Height()-1):y;
   w = (w<0)? 0 : (w>Width())? Width():w;
   h = (h<0)? 0 : (h>Height())? Height():h;
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
			GetPixel(x,y)=Pixel(ComponentLerp(l_mean,p.r,factor),ComponentLerp(l_mean,p.g,factor),ComponentLerp(l_mean,p.b,factor),p.a);
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
			GetPixel(x,y)=PixelLerp(q,p,factor);
		}
	}
}


//For full credit, check that your dithers aren't making the pictures systematically brighter or darker
void Image::RandomDither (int nbits){
	int x,y;
	int factor=2<<(8-nbits); //resolution of the quantizer
	srand(time(NULL));
   for (x=0;x<Width();x++){
      for (y=0;y<Height();y++){
         Pixel p = GetPixel(x,y);
		//  Pixel noise=Pixel(rand()%factor,rand()%factor,rand()%factor,255); //factor;
		//  noise = noise*0.5;
		// // int new_r=(p.r+rand()%factor-factor/2)>0?p.r+rand()%factor-factor/2:0;
		// // int new_g=(p.g+rand()%factor-factor/2)>0?p.g+rand()%factor-factor/2:0;
		// // int new_b=(p.b+rand()%factor-factor/2)>0?p.b+rand()%factor-factor/2:0;
		// //  Pixel new_p=Pixel(new_r,new_g,new_b,p.a);
		// //   if (x==100 && y==100){
		// // 	  printf("p=%d,%d,%d\n",p.r,p.g,p.b);
		// // 	printf("new_p=%d,%d,%d\n",new_p.r,new_p.g,new_p.b);
		// //  }
        // //  GetPixel(x,y) = PixelQuant(new_p,nbits);
		// GetPixel(x,y) = PixelQuant(p+noise,nbits);
		Pixel new_p = Pixel(ComponentClamp(p.r+rand()%factor-factor/2),ComponentClamp(p.g+rand()%factor-factor/2),ComponentClamp(p.b+rand()%factor-factor/2),p.a);
		GetPixel(x,y)=PixelQuant(new_p,nbits);
      }
   }
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
	int x,y;
	for (x=0;x<Width();x++){
      	for (y=0;y<Height();y++){
			Pixel p=GetPixel(x,y);
			//Corresponding Bayer Matrix entry
			float t=((float)Bayer4[x%4][y%4]/16-0.5)*255.0/nbits;
			Pixel new_p=Pixel(ComponentClamp(p.r+(int)t),ComponentClamp(p.g+(int)t),ComponentClamp(p.b+(int)t),p.a);
			GetPixel(x,y)=PixelQuant(new_p,nbits);
		}
	}
}

/* Error-diffusion parameters */
const double
    ALPHA = 7.0 / 16.0,
    BETA  = 3.0 / 16.0,
    GAMMA = 5.0 / 16.0,
    DELTA = 1.0 / 16.0;

void Image::FloydSteinbergDither(int nbits){
	int x,y;
	for (x=1;x<Width()-1;x++){
      	for (y=0;y<Height()-1;y++){
			Pixel p=GetPixel(x,y);
			Pixel neighbor1=GetPixel(x+1,y);
			Pixel neighbor2=GetPixel(x-1,y+1);
			Pixel neighbor3=GetPixel(x,y+1);
			Pixel neighbor4=GetPixel(x+1,y+1);
			Pixel new_p=PixelQuant(p,nbits);
			int err_r=p.r-new_p.r;
			int err_g=p.g-new_p.g;
			int err_b=p.b-new_p.b;	
			GetPixel(x,y)=new_p;
			GetPixel(x+1,y  )=Pixel(ComponentClamp(neighbor1.r+err_r*ALPHA),ComponentClamp(neighbor1.g+err_g*ALPHA),ComponentClamp(neighbor1.b+err_b*ALPHA),neighbor1.a);
			GetPixel(x-1,y+1)=Pixel(ComponentClamp(neighbor2.r+err_r*BETA),ComponentClamp(neighbor2.g+err_g*BETA),ComponentClamp(neighbor2.b+err_b*BETA),neighbor2.a);
			GetPixel(x  ,y+1)=Pixel(ComponentClamp(neighbor3.r+err_r*GAMMA),ComponentClamp(neighbor3.g+err_g*GAMMA),ComponentClamp(neighbor3.b+err_b*GAMMA),neighbor3.a);
			GetPixel(x+1,y+1)=Pixel(ComponentClamp(neighbor4.r+err_r*DELTA),ComponentClamp(neighbor4.g+err_g*DELTA),ComponentClamp(neighbor4.b+err_b*DELTA),neighbor4.a);
		}
	}
	//Border
	for (y=0;y<Height();y++){
		Pixel p=GetPixel(0,y);
		GetPixel(0,y)=PixelQuant(p,nbits);
		p=GetPixel(Width()-1,y);
		GetPixel(Width()-1,y)=PixelQuant(p,nbits);
	}
	for (x=0;x<Width()-1;x++){
      	Pixel p=GetPixel(x,Height()-1);
		GetPixel(x,Height()-1)=PixelQuant(p,nbits);
	}
}

// static float GaussianKernel[7] = {
// 	0.006, 0.061, 0.242, 0.383, 0.242, 0.061, 0.006
// };

void GaussianFilter(float* kernel, int n){
	float sigma=(2*n+1)/3; //sigma
	float p,q=2.0*sigma*sigma;
	float s=0;
	int i;
	for (i=-n;i<=n;i++){
		p = i*i;
		kernel[i+n]=(exp(-p/q)/(PI*q));
		s += kernel[i+n];
	}
	for (i=-n;i<=n;i++){
		kernel[i+n] /= s;
	}
}

float convolve1D(const float* filter, const float* data, int n, int x, int w){
	int i,ix;
	float s=0;
	float a,b;
	for (i=-n;i<=n;i++){
		a=filter[i+n];
		// ix = (x<n)?n-x : (x>(xl-n-1))? (2*xl-x-n-1) : x+i;
		// ix = (x<n)?n : (x>(xl-n-1))? (xl-n-1) : x+i;
		ix = (x+i<=0)?-x-i : (x+i>(w-1))? (2*w-2-x-i) : x+i;
		b = data[ix];
		s += a*b;
	}
	return s;
}

void Image::Blur1D(int n){	
	int w=Width(), h=Height();
	n = (n<1)? 1 : (n>min(w,h)/2)? min(w,h)/2:n;
	Image* img_copy=new Image(*this);
	int l=2*n+1;
	float* kernel = new float[l];
	GaussianFilter(kernel,n);
	int x,y,i,j,ix,iy;
	for (y=0;y<Height();y++){		
		for (x=0;x<Width();x++){ 
			float k,r=0,g=0,b=0;
			for (i=-n;i<=n;i++){
				k=kernel[i+n];
				ix = (x+i<=0)?-x-i : (x+i>(w-1))? (2*w-2-x-i) : x+i;
				Pixel p = img_copy->GetPixel(ix,y);
				r+=k*(float)p.r;
				g+=k*(float)p.g;
				b+=k*(float)p.b;
			}
			GetPixel(x,y)=Pixel(r,g,b,img_copy->GetPixel(x,y).a);
		}
	}
	//printf("row convolution completed.\n");
	img_copy=new Image(*this);
	//printf("start column.\n");
	for (x=0;x<Width();x++){		
		for (y=0;y<Height();y++){ 
			float k,r=0,g=0,b=0;
			for (i=-n;i<=n;i++){
				k=kernel[i+n];
				iy = (y+i<=0)?-y-i : (y+i>(h-1))? (2*h-2-y-i) : y+i;
				Pixel p = img_copy->GetPixel(x,iy);
				r+=k*(float)p.r;
				g+=k*(float)p.g;
				b+=k*(float)p.b;
			}
			GetPixel(x,y)=Pixel(r,g,b,img_copy->GetPixel(x,y).a);
		}
	}
	delete img_copy, kernel;
	img_copy = NULL;
	kernel = NULL;
}

// void Image::Blur1D(int n){	
// 	int w=Width(), h=Height();
// 	n = (n<1)? 1 : (n>min(w,h)/2)? min(w,h)/2:n;
// 	Image* img_copy=new Image(*this);
// 	int l=2*n+1;
// 	float* kernel = new float[l];
// 	GaussianFilter(kernel,n);
// 	int x,y;
// 	for (y=0;y<Height();y++){
// 		float r[w],g[w],b[w];
// 		float r_new,g_new,b_new;
// 		for (x=0;x<Width();x++){      	
// 			Pixel p = img_copy->GetPixel(x,y);
// 			r[x]=(float)p.r;
// 			g[x]=(float)p.g;
// 			b[x]=(float)p.b;			
// 		}
// 		for (x=0;x<Width();x++){ 
// 			Pixel p = img_copy->GetPixel(x,y);
// 			r_new=convolve1D(kernel,r,n,x,w);
// 			g_new=convolve1D(kernel,g,n,x,w);
// 			b_new=convolve1D(kernel,b,n,x,w);
// 			GetPixel(x,y)=Pixel(r_new,g_new,b_new,p.a);
// 		}
// 	}
// 	//printf("row convolution completed.\n");
// 	img_copy=new Image(*this);
// 	//printf("start column.\n");
// 	for (x=0;x<Width();x++){
// 		float r[h],g[h],b[h];
// 		float r_new,g_new,b_new;
// 		for (y=0;y<Height();y++){      	
// 			Pixel p = img_copy->GetPixel(x,y);
// 			r[y]=(float)p.r;
// 			g[y]=(float)p.g;
// 			b[y]=(float)p.b;
// 		}
// 		for (y=0;y<Height();y++){ 
// 			Pixel p = img_copy->GetPixel(x,y);
// 			r_new=convolve1D(kernel,r,n,y,h);
// 			g_new=convolve1D(kernel,g,n,y,h);
// 			b_new=convolve1D(kernel,b,n,y,h);
// 			GetPixel(x,y)=Pixel(r_new,g_new,b_new,p.a);
// 		}
// 	}
// 	delete img_copy, kernel;
// 	img_copy = NULL;
// 	kernel = NULL;
// }

void GaussianFilter2D(float* kernel, int n){
	float sigma=(2*n+1)/3; //sigma
	float p,q=2.0*sigma*sigma;
	float s=0;
	int i,j;
	for (i=-n;i<=n;i++){
        for(j=-n;j<=n;j++){
            p = i*i+j*j;
            int idx = (i+n)*(2*n+1)+j+n;
            kernel[idx]=(exp(-p/q)/(PI*q));
            s += kernel[idx];
        }
	}
	for (i=-n;i<=n;i++){
        for(j=-n;j<=n;j++){
            int idx = (i+n)*(2*n+1)+j+n;
            kernel[idx] /= s;
        }
	}
}

float convolve2D(const float* filter, const float* data, int n, int x, int y, int w, int h){
	int i,j,idxa,idxb,ix,iy;
	float s=0;
	float a,b;
	for (i=-n;i<n+1;i++){
		for (j=-n;j<n+1;j++){
			idxa = (i+n)*(2*n+1)+j+n;
			a=filter[idxa];
			ix = (x+i<=0)?-x-i : (x+i>(w-1))? (2*w-2-x-i) : x+i;
			iy = (y+j<=0)?-y-j : (y+j>(h-1))? (2*h-2-y-j):y+j;
			idxb = ix+iy*w;
			b=data[idxb];
			s += a*b;
		}
	}
	return s;
}

Pixel convolve2DPixel(const float* filter, const Image& img, int n, int x, int y){
	int i,j,idxa,idxb,ix,iy;
	int w = img.Width();
	int h = img.Height();
	// Pixel ps=Pixel(0,0,0,img.GetPixel(x,y).a);
	float a,r=0,g=0,b=0;
	for (i=-n;i<=n;i++){
		for (j=-n;j<=n;j++){
			idxa = (i+n)*(2*n+1)+j+n;
			a=filter[idxa];
			ix = (x+i<=0)?-x-i : (x+i>(w-1))? (2*w-2-x-i) : x+i;
			iy = (y+j<=0)?-y-j : (y+j>(h-1))? (2*h-2-y-j):y+j;
			// ix = (x<=n)?n : (x>=(xl-n-1))? (xl-n-1) : x+i;
			// iy = (y<=n)?n:(y>=(yl-n-1))?(yl-n-1):y+j;
			Pixel p = img.GetPixel(ix,iy);
			r+=a*(float)p.r;
			g+=a*(float)p.g;
			b+=a*(float)p.b;
		}
	}
	r=(r>255)?255:r;g=(g>255)?255:g;b=(b>255)?255:b;
	return Pixel(r,g,b,img.GetPixel(x,y).a);
}

void Image::Blur2D(int n){	
	int w=Width(), h=Height();
	n = (n<1)? 1 : (n>min(w,h)/2)? min(w,h)/2:n;
	// Image* img_copy=new Image(*this);
	int l=2*n+1;
	float* kernel = new float[l*l];
	GaussianFilter2D(kernel,n);
	int x,y;
	// float* r = new float[w*h];
	// float* g = new float[w*h];
	// float* b = new float[w*h];
	// float r_new, g_new, b_new;
	// for (y=0;y<Height();y++){
	// 	for (x=0;x<Width();x++){	
	// 		Pixel p = GetPixel(x,y);
	// 		idx = y*w+x;
	// 		r[idx] = p.r;
	// 		g[idx] = p.g;
	// 		b[idx] = p.b;
	// 	}
	// }
	for (y=0;y<Height();y++){
		for (x=0;x<Width();x++){
			// Pixel p = GetPixel(x,y);
			// r_new=convolve2D(kernel,r,n,x,y,w,h);
			// g_new=convolve2D(kernel,g,n,x,y,w,h);
			// b_new=convolve2D(kernel,b,n,x,y,w,h);
			GetPixel(x,y)=convolve2DPixel(kernel,*this,n,x,y);
		}
	}
	//printf("row convolution completed.\n");
	delete kernel; //, r, g, b;
	kernel = nullptr;
	// r = nullptr;
	// g = nullptr;
	// b = nullptr;
}

// void Image::Blur2D(int n){	
// 	int w=Width(), h=Height();
// 	n = (n<1)? 1 : (n>min(w,h)/2)? min(w,h)/2:n;
// 	// Image* img_copy=new Image(*this);
// 	int l=2*n+1;
// 	float* kernel = new float[l*l];
// 	GaussianFilter2D(kernel,n);
// 	int x,y,idx;
// 	float* r = new float[w*h];
// 	float* g = new float[w*h];
// 	float* b = new float[w*h];
// 	float r_new, g_new, b_new;
// 	for (y=0;y<Height();y++){
// 		for (x=0;x<Width();x++){	
// 			Pixel p = GetPixel(x,y);
// 			idx = y*w+x;
// 			r[idx] = p.r;
// 			g[idx] = p.g;
// 			b[idx] = p.b;
// 		}
// 	}
// 	for (y=0;y<Height();y++){
// 		for (x=0;x<Width();x++){
// 			Pixel p = GetPixel(x,y);
// 			r_new=convolve2D(kernel,r,n,x,y,w,h);
// 			g_new=convolve2D(kernel,g,n,x,y,w,h);
// 			b_new=convolve2D(kernel,b,n,x,y,w,h);
// 			GetPixel(x,y)=Pixel(r_new,g_new,b_new,p.a);
// 		}
// 	}
// 	//printf("row convolution completed.\n");
// 	delete kernel, r, g, b;
// 	kernel = nullptr;
// 	r = nullptr;
// 	g = nullptr;
// 	b = nullptr;
// }

void Image::Sharpen(int n){
	int w=Width(), h=Height();
	n = (n<1)? 1 : (n>min(w,h)/2)? min(w,h)/2:n;
	Image* img_blur=new Image(*this);
	img_blur->Blur1D(n);
	int x,y;
	for (x=0;x<Width();x++){
		for (y=0;y<Height();y++){
			Pixel p=GetPixel(x,y);
			Pixel q=img_blur->GetPixel(x,y);
			GetPixel(x,y)=PixelLerp(q,p,3);
		}
	}
}

void Image::EdgeDetect(){
	float kernel[9] = {-1,-1,-1,-1,8,-1,-1,-1,-1};
	int n=1;
	int w=Width(), h=Height();
	int x,y,idx;
	// float* r = new float[w*h];
	// float* g = new float[w*h];
	// float* b = new float[w*h];
	// float r_new, g_new, b_new;
	float* l = new float[w*h];
	float l_new;
	//Blur2D(50);
	for (y=0;y<Height();y++){
		for (x=0;x<Width();x++){	
			Pixel p = GetPixel(x,y);
			idx = y*w+x;
			// r[idx] = p.r;
			// g[idx] = p.g;
			// b[idx] = p.b;
			l[idx] = p.Luminance();
		}
	}
	for (y=0;y<Height();y++){
		for (x=0;x<Width();x++){
			Pixel p = GetPixel(x,y);
			// r_new=convolve2D(kernel,r,n,x,y,w,h);
			// g_new=convolve2D(kernel,g,n,x,y,w,h);
			// b_new=convolve2D(kernel,b,n,x,y,w,h);
			// GetPixel(x,y)=Pixel(r_new,g_new,b_new,p.a);
			l_new=convolve2D(kernel,l,n,x,y,w,h);
			l_new=(l_new>10)?l_new:0;
			GetPixel(x,y)=Pixel(l_new,l_new,l_new,p.a);
		}
	}
	//printf("row convolution completed.\n");
	// delete r, g, b;
	// r = nullptr;
	// g = nullptr;
	// b = nullptr;
	delete l;
	l= nullptr;
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
   /* IMAGE_SAMPLING_POINT,
    IMAGE_SAMPLING_BILINEAR,
    IMAGE_SAMPLING_GAUSSIAN,
    IMAGE_N_SAMPLING_METHODS */
	switch (sampling_method){
		case IMAGE_SAMPLING_BILINEAR:{
			int x1 = (int)u;
			int x2 = (int)u+1;
			int y1 = (int)v;
			int y2 = (int)v+1;
			float a1 = (x2-u)/(x2-x1);
			float a2 = (u-x1)/(x2-x1);
			float b1 = (y2-v)/(y2-y1);
			float b2 = (v-y1)/(y2-y1);
			Pixel p11 = GetPixel(x1,y1);
			Pixel p12 = GetPixel(x1,y2);
			Pixel p21 = GetPixel(x2,y1);
			Pixel p22 = GetPixel(x2,y2);
			return (b1*(a1*p11+a2*p21)+b2*(a1*p12+a2*p22));			
			break;
		}
		case IMAGE_SAMPLING_GAUSSIAN:{
			int n = 3; //Determines Gaussian filter size
			int l = 2*n+1; //Gaussian filter size
			float* kernel = new float[l*l];
			GaussianFilter2D(kernel,n);

			delete kernel;
			kernel = nullptr;
			break;
		}
		case IMAGE_N_SAMPLING_METHODS:
		//
			break;
		case IMAGE_SAMPLING_POINT:
		default:
			return (GetPixel(round(u),round(v)));
	}

   return Pixel();
}