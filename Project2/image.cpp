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
// #define DEBUG

uint8_t mask(int n){
	return ~-(1<<n);
}
void bin(uint8_t n)
{
    unsigned i;
    for (i = 1 << 7; i > 0; i = i / 2)
        (n & i) ? printf("1") : printf("0");
}

void write_ppm(char* imgName, int width, int height, int bits, const uint8_t *data){
   //Open the texture image file
   ofstream ppmFile(imgName, std::ios::binary);
//    ppmFile.open(imgName);
   if (!ppmFile){
      printf("ERROR: Could not creat file '%s'\n",imgName);
      exit(1);
   }

   //Set this as an ASCII PPM (first line is P3)
   string PPM_style = "P6\n";
   ppmFile << PPM_style; //Read the first line of the header    

   //Write out the texture width and height
   ppmFile << width << " "  << height << "\n" ;
	int maximum = (2<<(bits-1))-1;
   ppmFile << maximum;// << "\n" ;

   int shift = 8-bits;
   int numBytes = ceil(bits*3.0/8.0);
   int numComponents=sizeof(Pixel)*width*height;
   
   int pbits=bits,dbits=8; 
   uint8_t dout=0, p;
   int dcnt=0;
   for (int i=0; i<numComponents; i++){
	   if ((i+1)%4==0) i++;
	   p=data[i]>>shift; pbits=bits;
	   while (pbits>0){
		  	if (dbits>pbits){
				if (pbits==bits){
					dout=(dout)|((p<<(dbits-pbits)));
				}else{
					dout=dout|(p<<(8-pbits));
				}
				dbits-=pbits;pbits-=pbits;
			} else {
				dout=dout|(p>>(pbits-dbits));
				pbits -= dbits;dbits-=dbits;
			}
			if (dbits<1){
				ppmFile.write(reinterpret_cast<char*>(&dout), sizeof dout);
				dbits=8;
				dout=0;
				dcnt++;
			}
	   }
   }
   ppmFile.close();
}

int log2_asm(int x){
	uint32_t y;
	asm ("\tbsr %1, %0\n"
		: "=r"(y)
		: "r" (x)
	);
	return y;
}

//DONE - HW2: The current implementation of read_ppm() assumes the PPM file has a maximum value of 255 (ie., an 8-bit PPM) ...
//DONE - HW2: ... you need to adjust the function to support PPM files with a max value of 1, 3, 7, 15, 31, 63, 127, and 255 (why these numbers?)
uint8_t* read_ppm(char* imgName, int& width, int& height){
   //Open the texture image file
   ifstream ppmFile;
   ppmFile.open(imgName, ios::binary);
   if (!ppmFile){
      printf("ERROR: Image file '%s' not found.\n",imgName);
      exit(1);
   }
	int dsize=sizeof(Pixel);
   //Check that this is an ASCII PPM (first line is P3)
   string PPM_style;
   ppmFile >> PPM_style; //Read the first line of the header    

   //Read in the texture width and height
   ppmFile >> width >> height;
   unsigned char* img_data = new unsigned char[dsize*width*height];

   //Check that the 3rd line is 255 (ie., this is an 8 bit/pixel PPM)
   int maximum;
   ppmFile >> maximum;   
//    int factor = 256/(maximum+1);
   int bits=log2_asm(maximum+1);
   int shift=8-bits;
   int numBytes = ceil(bits*3.0/8.0);

   if (PPM_style == "P3"){
	int r, g, b;
	for (int i = 0; i < height; i++){
		for (int j = 0; j < width; j++){
			ppmFile >> r >> g >> b;
			img_data[i*width*4 + j*4] = r<<shift;//*factor>>;  //Red
			img_data[i*width*4 + j*4 + 1] = g<<shift;//*factor;  //Green
			img_data[i*width*4 + j*4 + 2] = b<<shift;//*factor;  //Blue
			img_data[i*width*4 + j*4 + 3] = 255;  //Alpha
		}
	}
   } else if (PPM_style == "P6"){
	int pbits=bits,dbits=0; 
	uint8_t dout;
	int numComponents=dsize*width*height, i=0;
	int dcnt=0;
	while (i<numComponents){
		if (dbits<1){
			ppmFile.read(reinterpret_cast<char*>(&dout), sizeof dout);
			dbits=8; dcnt++;
		}
		if (dbits>pbits){
			if (pbits==bits){
				img_data[i]=(dout>>(dbits-pbits))&mask(pbits);
			}else{
				img_data[i]= img_data[i] | (dout>>(dbits-pbits));
			}
			dbits-=pbits;
			img_data[i]=img_data[i]<<shift; i++; pbits=bits;
			if ((i+1)%4==0) {
				img_data[i]=255;
				i++;
			}
		} else {
			img_data[i]=(dout<<(pbits-dbits))&mask(pbits);
			pbits -= dbits;dbits-=dbits;
		}	
	}
   }

	ppmFile.close();
   return img_data;
}

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
	
	data.raw = new uint8_t[num_pixels*sizeof(Pixel)];
	
	memcpy(data.raw, src.data.raw, num_pixels*sizeof(Pixel));
	// *data.raw = *src.data.raw;
}

Image::Image (char* fname){
	uint8_t* loadedPixels;
	int lastc = strlen(fname);
	if (string(fname+lastc-3) == "ppm"){
		loadedPixels = read_ppm(fname, width, height);
	}
	else{
	int numComponents; //(e.g., Y, YA, RGB, or RGBA)

	//Load the pixels with STB Image Lib
	loadedPixels = stbi_load(fname, &width, &height, &numComponents, 4);
	}
	if (loadedPixels == NULL){
	printf("Error loading image: %s", fname);
	exit(-1);
	}

	//Set image member variables
	num_pixels = width * height;
	sampling_method = IMAGE_SAMPLING_POINT;
	
	 //Copy the loaded pixels into the image data structure
	data.raw = new uint8_t[num_pixels*sizeof(Pixel)];
	memcpy(data.raw, loadedPixels, num_pixels*sizeof(Pixel));
	free(loadedPixels);
}

Image::~Image (){
    delete[] data.raw;
    data.raw = NULL;
}

void Image::Write(char* fname){
	
	int lastc = strlen(fname);

	switch (fname[lastc-1]){
		case 'm': //ppm
			printf("Writing ppm file with depth=%d\n",export_depth);
		 	 write_ppm(fname, width, height, export_depth, data.raw);
				break;
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
			Pixel q=PixelRandom();
			GetPixel(x,y)=PixelLerp(p,q,factor);
			#ifdef DEBUG
			rs+=GetPixel(x,y).r/255;gs+=GetPixel(x,y).g/255;bs+=GetPixel(x,y).b/255;	
			l+=GetPixel(x,y).Luminance()/255;	
			ns +=q.r/255;	
			#endif
		}
	}
	#ifdef DEBUG
	printf("factor=%f, noise avg=%f\n",factor,ns/NumPixels());
	printf("Original image: r=%f, g=%f, b=%f, l=%f\n",rs0/NumPixels(),gs0/NumPixels(),bs0/NumPixels(),l0/NumPixels());
	printf("New image: r=%f, g=%f, b=%f, l=%f\n",rs/NumPixels(),gs/NumPixels(),bs/NumPixels(),l/NumPixels());
	#endif
}

void Image::AddNoiseSaltPepper (double factor){
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
			if (rand()%100<factor*100){
				if (rand()%2==0){
					GetPixel(x,y)=Pixel(0,0,0,p.a);
					#ifdef DEBUG
					ns -=1;
					#endif
				} else {
					GetPixel(x,y)=Pixel(255,255,255,p.a);
					#ifdef DEBUG
					ns +=1;
					#endif
				}
			}
			#ifdef DEBUG
			rs+=GetPixel(x,y).r/255;gs+=GetPixel(x,y).g/255;bs+=GetPixel(x,y).b/255;	
			l+=GetPixel(x,y).Luminance()/255;	
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
	float r=1.0-(float)nbits/(float)8;
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
		Pixel new_p = Pixel(ComponentClamp(p.r+rand()%factor-factor/2),ComponentClamp(p.g+rand()%factor-factor/2),ComponentClamp(p.b+rand()%factor-factor/2),p.a);
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

//Generate a 1D Gaussian filter of size n
// void GaussianFilter1D(float* kernel, int n){
// 	float sigma=n/3; //sigma
// 	float p,q=2.0*sigma*sigma;
// 	float s=0;
// 	int m=(n-1)/2;
// 	int i;
// 	if (n>1){
// 		for (i=-m;i<=m;i++){
// 			p = i*i;
// 			kernel[i+m]=(exp(-p/q)/sqrt(PI*q));
// 			s += kernel[i+m];
// 		}
// 		for (i=-m;i<=m;i++){
// 			kernel[i+m] /= s;
// 		}
// 	} else {
// 		kernel[0]=1;
// 	}
// }

void GaussianFilter1D(float* kernel, int n, float dx){
	// float sigma=n/3; //sigma
	float p,q=2.0*0.2;
	float s=0;
	int m=(n-1)/2;
	int i;
	if (n>1){
		for (i=-m;i<=m;i++){
			p = (i-dx)*(i-dx)/(float)(m*m);
			kernel[i+m]=(exp(-p/q/2.0)/sqrt(2.0*PI*q));
			s += kernel[i+m];
		}
		for (i=-m;i<=m;i++){
			kernel[i+m] /= s;
		}
	} else {
		kernel[0]=1;
	}
}

void Image::ConvolveRow(const float* kernel, int n, int c){
	Image* img_copy=new Image(*this);
	int w=Width(), h=Height();
	int x,y,i,j,ix,iy;
	int m=(n-1)/2;
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
			r=(r<0)?-r*c:(r>255)?255:r;
			g=(g<0)?-g*c:(g>255)?255:g;
			b=(b<0)?-b*c:(b>255)?255:b;
			GetPixel(x,y)=Pixel(r,g,b,img_copy->GetPixel(x,y).a);
			// GetPixel(x,y)=Pixel(ComponentClamp(r),ComponentClamp(g),ComponentClamp(b),img_copy->GetPixel(x,y).a);
		}
	}
	delete img_copy;
	img_copy = NULL;
}

void Image::ConvolveCol(const float* kernel, int n, int c){
	Image* img_copy=new Image(*this);
	int w=Width(), h=Height();
	int x,y,i,j,ix,iy;
	int m=(n-1)/2;
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
			r=(r<0)?-r*c:(r>255)?255:r;
			g=(g<0)?-g*c:(g>255)?255:g;
			b=(b<0)?-b*c:(b>255)?255:b;
			GetPixel(x,y)=Pixel(r,g,b,img_copy->GetPixel(x,y).a);
			// GetPixel(x,y)=Pixel(ComponentClamp(r),ComponentClamp(g),ComponentClamp(b),img_copy->GetPixel(x,y).a);
		}
	}
	delete img_copy;
	img_copy = NULL;
}

void Image::Blur(int n){	
	int w=Width(), h=Height();	
	assert(n%2!=0);	
	n = (n<1)? 1 : (n>min(w,h))? min(w,h):n;
	
	float* kernel = new float[n];
	GaussianFilter1D(kernel,n,0);
	ConvolveRow(kernel,n,0);
	ConvolveCol(kernel,n,0);
	
	delete kernel;
	kernel = NULL;
}

// void GaussianFilter2D(float* kernel, int n, float sigma){	
// 	float p,q=2.0*sigma*sigma;
// 	float s=0;
// 	int i,j,m=(n-1)/2;
// 	if (n>1){
// 		for (i=-m;i<=m;i++){
// 			for(j=-m;j<=m;j++){
// 				p = i*i+j*j;
// 				int idx = (i+m)*(2*m+1)+j+m;
// 				kernel[idx]=(exp(-p/q)/sqrt(PI*q));
// 				s += kernel[idx];
// 			}
// 		}
// 		for (i=-m;i<=m;i++){
// 			for(j=-m;j<=m;j++){
// 				int idx = (i+m)*(2*m+1)+j+m;
// 				kernel[idx] /= s;
// 			}
// 		}
// 	} else {
// 		kernel[0]=1;
// 	}
// }

void GaussianFilter2D(float* kernel, int n, float dx, float dy){	
	float sigma=1.0;
	float p,q=2.0*0.2;
	float s=0;
	int i,j,m=(n-1)/2;
	if (n>1){
		for (i=-m;i<=m;i++){
			for(j=-m;j<=m;j++){
				p = ((i-dx)*(i-dx)+(j-dy)*(j-dy))/(float)(m*m);
				int idx = (i+m)*(2*m+1)+j+m;
				kernel[idx]=(exp(-p/q/2.0)/sqrt(2.0*PI*q));//(exp(-p/q)/(PI*q));
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

Pixel convolve2DPixel(const float* filter, const Image& img, int n, int x, int y, int c){
	int i,j,ik,ix,iy;
	int w = img.Width();
	int h = img.Height();
	float k,r=0,g=0,b=0;
	int m=(n-1)/2;
	for (i=-m;i<=m;i++){
		for (j=-m;j<=m;j++){
			ik = (i+m)*(2*m+1)+j+m;
			k=filter[ik];
			ix = (x+i<=0)?-x-i : (x+i>(w-1))? (2*w-2-x-i) : x+i;
			iy = (y+j<=0)?-y-j : (y+j>(h-1))? (2*h-2-y-j):y+j;
			Pixel p = img.GetPixel(ix,iy);
			r+=k*(float)p.r;
			g+=k*(float)p.g;
			b+=k*(float)p.b;
		}
	}
	r=(r<0)?-r*c:(r>255)?255:r;
	g=(g<0)?-g*c:(g>255)?255:g;
	b=(b<0)?-b*c:(b>255)?255:b;
	return Pixel(r,g,b,255);//img.GetPixel(x,y).a);
}

void Image::Blur2D(int n){	
	int w=Width(), h=Height();
	assert(n%2==1);
	// int m=(n-1)/2;
	n = (n<1)? 1 : (n>min(w,h))? min(w,h):n;
	Image* img_copy=new Image(*this);
	float* kernel = new float[n*n];
	float sigma=n/3; //sigma
	GaussianFilter2D(kernel,n,0,0);
	int x,y;
	for (y=0;y<Height();y++){
		for (x=0;x<Width();x++){
			GetPixel(x,y)=convolve2DPixel(kernel,*img_copy,n,x,y,0);
		}
	}
	delete[] kernel,img_copy; //, r, g, b;
	kernel = nullptr;
	img_copy = nullptr;
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
			// printf("x=%d,y=%d\n",x,y);
			Pixel p = convolve2DPixel(kernel,*img_copy,n,x,y,1);
			int r=(p.r>thr)?p.r:0;
			int g=(p.g>thr)?p.g:0;
			int b=(p.b>thr)?p.b:0;
			GetPixel(x,y)=Pixel(r,g,b,p.a);
		}
	}
	delete img_copy;
	img_copy=nullptr;
}

void Image::EdgeDetectSobel(){
	float kernel1[3] = {1,0,-1};
	float kernel2[3] = {1,2,1};
	int n=3;
	Image* Gx=new Image(*this);
	// Blur(11);
	
	Gx->ConvolveRow(kernel1,n,1);
	Gx->ConvolveCol(kernel2,n,1);
	Image* Gy=new Image(*this);
	Gy->ConvolveRow(kernel2,n,1);
	Gy->ConvolveCol(kernel1,n,1);
	int x,y;
	int thr = 50;
	for (y=0;y<Height();y++){
		for (x=0;x<Width();x++){
			Pixel px = Gx->GetPixel(x,y);
			Pixel py = Gy->GetPixel(x,y);
			float r=sqrt(px.r*px.r+py.r*py.r);
			float g=sqrt(px.g*px.g+py.g*py.g);
			float b=sqrt(px.b*px.b+py.b*py.b);
			r=(r<thr)?0:(r>255)?255:r;
			g=(g<thr)?0:(g>255)?255:g;
			b=(b<thr)?0:(b>255)?255:b;
			GetPixel(x,y)=Pixel(r,g,b,GetPixel(x,y).a);
			// GetPixel(x,y)=Pixel(ComponentClamp(sqrt(px.r*px.r+py.r*py.r)),ComponentClamp(sqrt(px.g*px.g+py.g*py.g)),ComponentClamp(sqrt(px.b*px.b+py.b*py.b)),GetPixel(x,y).a);
		}
	}
	ChangeSaturation(0);
	/*img->GetPixel(x,y) = Pixel(rx*rx+ry*ry>t?255:0,gx*gx+gy*gy>t?255:0,bx*bx+by*by>t?255:0);*/
	delete Gx,Gy;
	Gx=nullptr;
	Gy=nullptr;
}

void Image::EdgeDetectSobel2(){
	float kernel1[9] = {1,0,-1,2,0,-2,1,0,-1};
	float kernel2[9] = {1,2,1,0,0,0,-1,-2,-1};
	int n=3;
	Image* Gx=new Image(*this);
	Image* Gy=new Image(*this);
	
	int x,y;
	for (y=0;y<Height();y++){
		for (x=0;x<Width();x++){
			// printf("x=%d,y=%d\n",x,y);
			Gx->GetPixel(x,y)=convolve2DPixel(kernel1,*this,n,x,y,1);
			Gy->GetPixel(x,y)=convolve2DPixel(kernel2,*this,n,x,y,1);
		}
	}
	float thr=0;
	for (y=0;y<Height();y++){
		for (x=0;x<Width();x++){			
			Pixel px = Gx->GetPixel(x,y);
			Pixel py = Gy->GetPixel(x,y);
			float r=sqrt(px.r*px.r+py.r*py.r);
			float g=sqrt(px.g*px.g+py.g*py.g);
			float b=sqrt(px.b*px.b+py.b*py.b);
			r=(r<thr)?0:(r>255)?255:r;
			g=(g<thr)?0:(g>255)?255:g;
			b=(b<thr)?0:(b>255)?255:b;
			GetPixel(x,y)=Pixel(r,g,b,GetPixel(x,y).a);
		}
	}
	delete Gx,Gy;
	Gx=nullptr;
	Gy=nullptr;
}

Image* Image::Scale(double sx, double sy){
	int new_w = sx*Width();
	int new_h = sy*Height();
	Image* img_new = new Image(new_w,new_h);
	int x,y;
	float smin=min(sx,sy);
	float smax=max(sx,sy);
	float s=(smin>1)?smax:smin;
	// printf("sx=%f\n",sx);
	for (x=0;x<new_w;x++){
		for (y=0;y<new_h;y++){
			float u=x/sx,v=y/sy;			
			img_new->GetPixel(x,y)=Sample(u,v,s);
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
			img_new->GetPixel(x,y)=Sample(u,v,1);
			} else {
				img_new->GetPixel(x,y)=Pixel(200,200,200,0); //set extra space color to gray with full transparency support
			}
		}
	}
	return img_new;
}

void Image::Mosaic(int n){
	SetSamplingMethod(2);
	Image* s_img;
	// Image* img_copy=new Image(*this);
	// printf("created copy\n");
	float sx=1/(float)n;
	s_img=Scale(sx,sx);
	// printf("scaled\n");
	int x,y,i,j;	
	for (x=0;x<s_img->Width();x++){
		for (y=0;y<s_img->Height();y++){
			Pixel p=s_img->GetPixel(x,y);
			for (i=0;i<n;i++){
				for (j=0;j<n;j++){
					// printf("x=%d,y=%d,i=%d,j=%d,%d,%d\n",x,y,i,j,x*s+i,y*s+j);
					GetPixel(x*n+i,y*n+j)=p;
				}				
			}
		}
	}
	// int m=(n%2==0)?n-1:n;
	int m=(int)(n/4)*2+1;
	Blur(m);
	delete s_img; //,img_copy;
	s_img=nullptr;
	// img_copy=nullptr;
}

void Image::CharcoalPaint(){
	// float kernel1[9] = {1,1,-1,2,1,-2,2,-2,-2};//{1,1,-1,2,1,-2,1,-2,-1};
	// float kernel1[9]={1,1,1,1,1,1,1,1,1};
	float k=1;
	float kernel1[9]={0,k,0,k,k,k,0,k,0};//dilation filter
	// float kernel[3]={0.1,0.4,0.}
	int w=Width(), h=Height();
	int n = 3;
	int x,y;
	EdgeDetect();
	Image* img_copy=new Image(*this);
	// // erosion
	// int es=3; //size of erosion
	// int thr = 100;
	// for (y=es;y<Height()-es;y++){
	// 	for (x=es;x<Width()-es;x++){
	// 		Pixel p=img_copy->GetPixel(x,y);
	// 		int lp=p.Luminance();
	// 		int setp=1;
	// 		for (int i=-es;i<=es;i++){
	// 			Pixel q=img_copy->GetPixel(x+i,y);
	// 			int lq=q.Luminance();
	// 			if (abs(lp-lq)>thr) setp=0;
	// 			q=img_copy->GetPixel(x,y+i);
	// 			if (abs(lp-lq)>thr) setp=0;			
	// 		}
	// 		lp=lp*setp;
	// 		GetPixel(x,y)=Pixel(lp,lp,lp,p.a);
	// 	}
	// }
	// img_copy=new Image(*this);
	// int ds=es; //size of dilation
	// for (y=ds;y<Height()-ds;y+=ds){
	// 	for (x=ds;x<Width()-ds;x+=ds){
	// 		Pixel p=img_copy->GetPixel(x,y);
	// 		for (int i=-ds;i<=ds;i++){
	// 			for (int j=-ds;j<=ds;j++){
	// 				// int l=(p.Luminance())>50?255-p.Luminance():255;
	// 				GetPixel(x+i,y+j)=p;
	// 			}
	// 		}

	// 	}
	// }
	for (y=0;y<Height();y++){
		for (x=0;x<Width();x++){
			Pixel p=convolve2DPixel(kernel1,*img_copy,n,x,y,0);
			// Pixel p=GetPixel(x,y);
			int l=(p.Luminance())>50?255-p.Luminance():255;
			GetPixel(x,y)=Pixel(l,l,l,255);
			// GetPixel(x,y)=Pixel(255-p.r,255-p.g,255-p.b,p.a);
		}
	}
	// ChangeSaturation(0);
	// ChangeContrast(-0.5);
	delete img_copy; //, r, g, b;
	img_copy = nullptr;
}
void Image::CharcoalPaint2(){
	// float kernel1[9] = {1,1,-1,2,1,-2,2,-2,-2};//{1,1,-1,2,1,-2,1,-2,-1};
	// float kernel1[9]={1,1,1,1,1,1,1,1,1};
	float k=1;
	float kernel1[9]={0,k,0,k,k,k,0,k,0};//dilation filter
	// float kernel[3]={0.1,0.4,0.}
	int w=Width(), h=Height();
	int n = 3;
	int x,y;
	EdgeDetectSobel();
	Image* img_copy=new Image(*this);
	// erosion
	int es=3; //size of erosion
	// int thr = 100;
	// for (y=es;y<Height()-es;y++){
	// 	for (x=es;x<Width()-es;x++){
	// 		Pixel p=img_copy->GetPixel(x,y);
	// 		int lp=p.Luminance();
	// 		int setp=1;
	// 		for (int i=-es;i<=es;i++){
	// 			Pixel q=img_copy->GetPixel(x+i,y);
	// 			int lq=q.Luminance();
	// 			if (abs(lp-lq)>thr) setp=0;
	// 			q=img_copy->GetPixel(x,y+i);
	// 			if (abs(lp-lq)>thr) setp=0;			
	// 		}
	// 		lp=lp*setp;
	// 		GetPixel(x,y)=Pixel(lp,lp,lp,p.a);
	// 	}
	// }
	// img_copy=new Image(*this);
	int ds=es*2; //size of dilation
	for (y=ds;y<Height()-ds;y+=ds){
		for (x=ds;x<Width()-ds;x+=ds){
			Pixel p=img_copy->GetPixel(x,y);
			for (int i=-ds;i<=ds;i++){
				for (int j=-ds;j<=ds;j++){
					// int l=(p.Luminance())>50?255-p.Luminance():255;
					GetPixel(x+i,y+j)=p;
				}
			}

		}
	}
	for (y=0;y<Height();y++){
		for (x=0;x<Width();x++){
			Pixel p=convolve2DPixel(kernel1,*img_copy,n,x,y,0);
			// Pixel p=GetPixel(x,y);
			int l=(p.Luminance())>50?255-p.Luminance():255;
			GetPixel(x,y)=Pixel(l,l,l,255);
			// GetPixel(x,y)=Pixel(255-p.r,255-p.g,255-p.b,p.a);
		}
	}
	// ChangeSaturation(0);
	// ChangeContrast(-0.5);
	delete img_copy; //, r, g, b;
	img_copy = nullptr;
}

void Image::Filter1(){
	// float kernel1[9] = {1,1,-1,2,1,-2,2,-2,-2};//{1,1,-1,2,1,-2,1,-2,-1};
	// float kernel1[9]={1,1,1,1,1,1,1,1,1};
	float k=1;
	float kernel1[9]={0,k,0,k,k,k,0,k,0};//dilation filter
	// float kernel[3]={0.1,0.4,0.}
	int w=Width(), h=Height();
	int n = 3;
	int x,y;
	Image* img_copy=new Image(*this);
	//erosion
	int es=20; //size of erosion
	int thr = 100;
	for (y=es;y<Height()-es;y++){
		for (x=es;x<Width()-es;x++){
			Pixel p=img_copy->GetPixel(x,y);
			int lp=p.Luminance();
			int setp=1;
			for (int i=-es;i<=es;i++){
				Pixel q=img_copy->GetPixel(x+i,y);
				int lq=q.Luminance();
				if (abs(lp-lq)>thr) setp=0;
				q=img_copy->GetPixel(x,y+i);
				if (abs(lp-lq)>thr) setp=0;			
			}
			lp=lp*setp;
			GetPixel(x,y)=Pixel(p.r*setp,p.g*setp,p.b*setp,p.a);
		}
	}
	img_copy=new Image(*this);
	//dilation
	int ds=es; //size of dilation
	for (y=ds;y<Height()-ds;y+=ds){
		for (x=ds;x<Width()-ds;x+=ds){
			Pixel p=img_copy->GetPixel(x,y);
			for (int i=-ds;i<=ds;i++){
				for (int j=-ds;j<=ds;j++){
					// int l=(p.Luminance())>50?255-p.Luminance():255;
					GetPixel(x+i,y+j)=p;
				}
			}

		}
	}
	img_copy=new Image(*this);
	x=Width()-ds;
	for (y=Height()-ds;y<Height()-1;y++){
		Pixel p=img_copy->GetPixel(x-1,y);
		GetPixel(x,y)=p;
	}
	img_copy=new Image(*this);
	y=Height()-ds;
	for (x=Width()-ds;x<Width()-1;x++){
		Pixel p=img_copy->GetPixel(x,y-1);
		GetPixel(x,y)=p;
	}
	Blur(3);
	// ChangeSaturation(0);
	// ChangeContrast(-0.5);
	delete img_copy; //, r, g, b;
	img_copy = nullptr;
}

/**
 * Image Sample
 **/
void Image::SetSamplingMethod(int method){
   assert((method >= 0) && (method < IMAGE_N_SAMPLING_METHODS));
   sampling_method = method;
}


Pixel Image::Sample (double u, double v, double s){
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
			int n = (s<1)?(1/(2*s)*2+1):3; //Determines Gaussian filter size
			float* kernel = new float[n*n];
			// float sigma=n/3.0; //sigma
			GaussianFilter2D(kernel,n,(u-floor(u)),(v-floor(v)));
			int x=(u>Width()-1)?Width()-1:u;
			int y=(v>Height()-1)?Height()-1:v;
			return convolve2DPixel(kernel,*this,n,x,y, 0);
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