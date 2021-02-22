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
#define DEBUG

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
	x = (x<0)? 0 : (x>Width()-1)? (Width()-1):x;
   y = (y<0)? 0 : (y>Height()-1)? (Height()-1):y;
   w = (w<0)? 0 : (w>Width()-x)? Width()-x:w;
   h = (h<0)? 0 : (h>Height()-y)? Height()-y:h;
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
	#ifdef DEBUG
	float rs0=0,gs0=0,bs0=0,rs=0,bs=0,gs=0; //sum of r,g,b of original and new image
	float l=0,l0=0,ns=0;
	#endif
	srand(time(NULL));
	for (x=0;x<Width();x++){
		for (y=0;y<Height();y++){
			Pixel p=GetPixel(x,y);
			#ifdef DEBUG
			rs0+=p.r/255;gs0+=p.g/255;bs0+=p.b/255;
			l+=p.Luminance()/255;
			#endif
			// Pixel new_p=PixelRandom();
			// GetPixel(x,y)=p+(new_p*factor);
			int amp = 511*factor;
			int amp2 = amp/2;
			int noise = rand()%amp-amp2;			
			int new_r=p.r+noise;
			int new_g=p.g+noise;
			int new_b=p.b+noise;
			new_r=(new_r<0)?0:(new_r>255)?255:new_r;
			new_g=(new_g<0)?0:(new_g>255)?255:new_g;
			new_b=(new_b<0)?0:(new_b>255)?255:new_b;
			GetPixel(x,y)=Pixel(new_r,new_g,new_b,p.a);
			#ifdef DEBUG
			rs+=new_r/255;gs+=new_g/255;bs+=new_b/255;	
			l+=GetPixel(x,y).Luminance()/255;	
			ns +=noise;	
			#endif
		}
	}
	#ifdef DEBUG
	printf("factor=%f, noise avg=%f\n",factor,ns/NumPixels());
	printf("Original image: r=%f, g=%f, b=%f, l=%f\n",rs0/NumPixels(),gs0/NumPixels(),bs0/NumPixels(),l0/NumPixels());
	printf("New image: r=%f, g=%f, b=%f, l=%f\n",rs/NumPixels(),gs/NumPixels(),bs/NumPixels(),l/NumPixels());
	#endif
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
			if (factor>0){
				GetPixel(x,y)=Pixel(ComponentLerp(l_mean,p.r,factor),ComponentLerp(l_mean,p.g,factor),ComponentLerp(l_mean,p.b,factor),p.a);
			} else {
				GetPixel(x,y)=Pixel(ComponentLerp(l_mean,255-p.r,-factor),ComponentLerp(l_mean,255-p.g,-factor),ComponentLerp(l_mean,255-p.b,-factor),p.a);
			}
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
	#ifdef DEBUG
	float rs0=0,gs0=0,bs0=0,rs=0,bs=0,gs=0; //sum of r,g,b of original and new image
	float l=0,l0=0;
	#endif
   for (x=0;x<Width();x++){
      for (y=0;y<Height();y++){
         Pixel p = GetPixel(x,y);
		 #ifdef DEBUG
			rs0+=p.r/255;gs0+=p.g/255;bs0+=p.b/255;
			l+=p.Luminance()/255;
		#endif
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
		int noise = rand()%factor-factor/2;
		Pixel new_p = Pixel(ComponentClamp(p.r+noise),ComponentClamp(p.g+noise),ComponentClamp(p.b+noise),p.a);
		// Pixel new_p = Pixel(ComponentClamp(p.r+rand()%factor-factor/2),ComponentClamp(p.g+rand()%factor-factor/2),ComponentClamp(p.b+rand()%factor-factor/2),p.a);
		GetPixel(x,y)=PixelQuant(new_p,nbits);
		#ifdef DEBUG
			rs+=GetPixel(x,y).r/255;gs+=GetPixel(x,y).g/255;bs+=GetPixel(x,y).b/255;	
			l+=GetPixel(x,y).Luminance()/255;	
		#endif
      }
   }
   #ifdef DEBUG
	printf("nbits=%d\n",nbits);
	printf("Original image: r=%f, g=%f, b=%f, l=%f\n",rs0/NumPixels(),gs0/NumPixels(),bs0/NumPixels(),l0/NumPixels());
	printf("New image: r=%f, g=%f, b=%f, l=%f\n",rs/NumPixels(),gs/NumPixels(),bs/NumPixels(),l/NumPixels());
	#endif
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

void GaussianFilter1D(float* kernel, int n){
	float sigma=n/3; //sigma
	float p,q=2.0*sigma*sigma;
	float s=0;
	int m=(n-1)/2;
	int i;
	if (n>1){
		for (i=-m;i<=m;i++){
			p = i*i;
			kernel[i+m]=(exp(-p/q)/(PI*q));
			s += kernel[i+m];
		}
		for (i=-m;i<=m;i++){
			kernel[i+m] /= s;
		}
	} else {
		kernel[0]=1;
	}
}

float convolve1D(const float* filter, const float* data, int n, int x, int w){
	int i,ix;
	float s=0;
	float a,b;
	int m=(n-1)/2;
	for (i=-m;i<=m;i++){
		a=filter[i+m];
		// ix = (x<n)?n-x : (x>(xl-n-1))? (2*xl-x-n-1) : x+i;
		// ix = (x<n)?n : (x>(xl-n-1))? (xl-n-1) : x+i;
		ix = (x+i<=0)?-x-i : (x+i>(w-1))? (2*w-2-x-i) : x+i;
		b = data[ix];
		s += a*b;
	}
	return s;
}

void Image::Blur(int n){	
	int w=Width(), h=Height();
	assert(n%2!=0);
	int m=(n-1)/2;
	n = (n<1)? 1 : (n>min(w,h))? min(w,h):n;
	Image* img_copy=new Image(*this);
	float* kernel = new float[n];
	GaussianFilter1D(kernel,n);
	int x,y,i,j,ix,iy;
	for (y=0;y<Height();y++){		
		for (x=0;x<Width();x++){ 
			float k,r=0,g=0,b=0;
			for (i=-m;i<=m;i++){
				k=kernel[i+m];
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
			for (i=-m;i<=m;i++){
				k=kernel[i+m];
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

void GaussianFilter2D(float* kernel, int n, float sigma){	
	float p,q=2.0*sigma*sigma;
	float s=0;
	int i,j,m=(n-1)/2;
	if (n>1){
		for (i=-m;i<=m;i++){
			for(j=-m;j<=m;j++){
				p = i*i+j*j;
				int idx = (i+m)*(2*m+1)+j+m;
				kernel[idx]=(exp(-p/q)/(PI*q));
				s += kernel[idx];
			}
		}
		for (i=-m;i<=m;i++){
			for(j=-m;j<=m;j++){
				int idx = (i+m)*(2*m+1)+j+m;
				kernel[idx] /= s;
			}
		}
	} else {
		kernel[0]=1;
	}
}

float convolve2D(const float* filter, const float* data, int n, int x, int y, int w, int h){
	int i,j,idxa,idxb,ix,iy;
	float s=0;
	float a,b;
	int m=(n-1)/2;
	for (i=-m;i<m+1;i++){
		for (j=-m;j<m+1;j++){
			idxa = (i+m)*(2*m+1)+j+m;
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
	int i,j,ik,ix,iy;
	int w = img.Width();
	int h = img.Height();
	// Pixel ps=Pixel(0,0,0,img.GetPixel(x,y).a);
	float k,r=0,g=0,b=0;
	int m=(n-1)/2;
	for (i=-m;i<=m;i++){
		for (j=-m;j<=m;j++){
			ik = (i+m)*(2*m+1)+j+m;
			k=filter[ik];
			ix = (x+i<=0)?-x-i : (x+i>(w-1))? (2*w-2-x-i) : x+i;
			iy = (y+j<=0)?-y-j : (y+j>(h-1))? (2*h-2-y-j):y+j;
			// ix = (x<=m)?m : (x>=(xl-m-1))? (xl-m-1) : x+i;
			// iy = (y<=m)?m:(y>=(yl-m-1))?(yl-m-1):y+j;
			Pixel p = img.GetPixel(ix,iy);
			r+=k*(float)p.r;
			g+=k*(float)p.g;
			b+=k*(float)p.b;
		}
	}
	r=(r<0)?0:(r>255)?255:r;
	g=(g<0)?0:(g>255)?255:g;
	b=(b<0)?0:(b>255)?255:b;
	return Pixel(r,g,b,img.GetPixel(x,y).a);
}

void Image::Blur2D(int n){	
	int w=Width(), h=Height();
	assert(n%2==1);
	int m=(n-1)/2;
	n = (n<1)? 1 : (n>min(w,h))? min(w,h):n;
	// Image* img_copy=new Image(*this);
	float* kernel = new float[n*n];
	float sigma=n/3; //sigma
	GaussianFilter2D(kernel,n,sigma);
	int x,y;
	for (y=0;y<Height();y++){
		for (x=0;x<Width();x++){
			GetPixel(x,y)=convolve2DPixel(kernel,*this,n,x,y);
		}
	}
	delete kernel; //, r, g, b;
	kernel = nullptr;
}

void Image::Sharpen(int n){
	int w=Width(), h=Height();
	n = (n<1)? 1 : (n>min(w,h))? min(w,h):n;
	Image* img_blur=new Image(*this);
	img_blur->Blur(n);
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
	int n=3;
	int x,y;
	int thr=0; //threshold to control how much edge to show
	Image* img_copy = new Image(*this);
	for (y=0;y<Height();y++){
		for (x=0;x<Width();x++){
			Pixel p = convolve2DPixel(kernel,*img_copy,n,x,y);
			int r=(p.r>thr)?p.r:0;
			int g=(p.g>thr)?p.g:0;
			int b=(p.b>thr)?p.b:0;
			GetPixel(x,y)=Pixel(r,g,b,p.a);
		}
	}
	delete img_copy;
	img_copy=nullptr;
}

Image* Image::Scale(double sx, double sy){
	int new_w = sx*Width();
	int new_h = sy*Height();
	Image* img_new = new Image(new_w,new_h);
	int x,y;
	for (x=0;x<new_w;x++){
		for (y=0;y<new_h;y++){
			float u=x/sx,v=y/sy;
			img_new->GetPixel(x,y)=Sample(u,v);
		}
	}
	return img_new;
}

Image* Image::Rotate(double angle){
	float a = angle*PI/180;
	float cos_th = cos(a);
	float sin_th = sin(a);
	int new_w = Width()*abs(cos_th)+Height()*abs(sin_th);
	int new_h = Width()*abs(sin_th)+Height()*abs(cos_th);
	float cx0 = Width()/2, cy0 = Height()/2; // old center of image
	float cx = new_w/2, cy = new_h/2; // new center of image
	Image* img_new = new Image(new_w,new_h);
	int x,y;
	for (x=0;x<new_w;x++){
		for (y=0;y<new_h;y++){
			float u,v;
			u = (x-cx)*cos_th+(y-cy)*sin_th+cx0;
			v = -(x-cx)*sin_th+(y-cy)*cos_th+cy0;
			if (u>=0 && v>=0 && u<Width() && v<Height()){
			img_new->GetPixel(x,y)=Sample(u,v);
			} else {
				img_new->GetPixel(x,y)=Pixel(200,200,200,0); //set extra space color to gray with full transparency support
			}
		}
	}
	return img_new;
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
			int x1,x2,y1,y2;
			float a1,a2,b1,b2;
			if (u>=Width()-1){
				x1=Width()-2;
				x2=Width()-1;
			} else {
				x1=u;
				x2=u+1;				
			}
			if (v>=Height()-1){
				y1=Height()-2;
				y2=Height()-1;
			} else {
				y1=v;
				y2=v+1;				
			}
			a1 = (x2-u)/(x2-x1);
			a2 = (u-x1)/(x2-x1);
			b1 = (y2-v)/(y2-y1);
			b2 = (v-y1)/(y2-y1);
			Pixel p11 = GetPixel(x1,y1);
			Pixel p12 = GetPixel(x1,y2);
			Pixel p21 = GetPixel(x2,y1);
			Pixel p22 = GetPixel(x2,y2);
			float c1=a1*b1, c2=a2*b1, c3=a1*b2, c4=a2*b2;
			return p11*c1+p21*c2+p12*c3+p22*c4;			
			break;
		}
		case IMAGE_SAMPLING_GAUSSIAN:{
			int n = 5; //Determines Gaussian filter size
			float* kernel = new float[n*n];
			float sigma=n/3; //sigma
			GaussianFilter2D(kernel,n,sigma);
			return convolve2DPixel(kernel,*this,n,(int)u,(int)v);
			delete kernel;
			kernel = nullptr;
			break;
		}
		case IMAGE_N_SAMPLING_METHODS:
		case IMAGE_SAMPLING_POINT:
		default:
			int x=(round(u)>Width()-1)?Width()-1:round(u);
			int y=(round(v)>Height()-1)?Height()-1:round(v);
			return (GetPixel(x,y));
	}

   return Pixel();
}