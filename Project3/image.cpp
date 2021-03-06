
#include "image.h"

Image::Image(int w, int h) : width(w), height(h) {
    pixels = new Color[width*height];
}

    //Copy constructor - Called on: Image img1 = img2; //Making new image
Image::Image(const Image& cpy){
    width = cpy.width;
    height = cpy.height;
    memcpy(pixels, cpy.pixels, width*height*sizeof(Color));
}

    //Assignment operator - Called on:  img1 = img2; //Overwriting existing data
Image&  Image::operator=(const Image& rhs){
    width = rhs.width;
    height = rhs.height;
    memcpy(pixels, rhs.pixels, width*height*sizeof(Color));
    return *this;
}

Image::Image(const char* fname) {
    int numComponents; //(e.g., Y, YA, RGB, or RGBA)
    unsigned char *data = stbi_load(fname, &width, &height, &numComponents, 4);

    if (data == NULL){
        printf("Error loading image: '%s'\n", fname);
        exit(-1);
    }

    pixels = new Color[width*height];

    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
        Color col = GetPixel(i,j);
        pixels[(i+j*width)] = Color(data[4*(i+j*width)+0]/255.0,data[4*(i+j*width)+1]/255.0,data[4*(i+j*width)+2]/255.0);
        }
    }

    stbi_image_free(data);
}

void Image::SetPixel (int i, int j, Color c){
    pixels[i+j*width] = c;
}

Color& Image::GetPixel(int i, int j){
    return pixels[i+j*width];
}

    uint8_t* Image::ToBytes(){
    uint8_t* rawPixels = new uint8_t[width*height*4];
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
        Color col = GetPixel(i,j);
        rawPixels[4*(i+j*width)+0] = uint8_t(fmin(col.r,1)*255);
        rawPixels[4*(i+j*width)+1] = uint8_t(fmin(col.g,1)*255);
        rawPixels[4*(i+j*width)+2] = uint8_t(fmin(col.b,1)*255);
        rawPixels[4*(i+j*width)+3] = 255; //alpha
        }
    }
    return rawPixels;
}

void Image::Write(const char* fname){

    uint8_t* rawBytes = ToBytes();

    int lastc = strlen(fname);

    switch (fname[lastc-1]){
        case 'g': //jpeg (or jpg) or png
        if (fname[lastc-2] == 'p' || fname[lastc-2] == 'e') //jpeg or jpg
            stbi_write_jpg(fname, width, height, 4, rawBytes, 95);  //95% jpeg quality
        else //png
            stbi_write_png(fname, width, height, 4, rawBytes, width*4);
        break;
        case 'a': //tga (targa)
        stbi_write_tga(fname, width, height, 4, rawBytes);
        break;
        case 'p': //bmp
        default:
        stbi_write_bmp(fname, width, height, 4, rawBytes);
    }

    delete[] rawBytes;
}

// void Image::Render(Scene& scene, Camera& camera, int max_d){
    // float halfW = width/2.0;
    // float halfH = height/2.0;
    // float d = halfH / tanf(camera.fov_ha * (M_PI / 180.0f));
    // int numSpheres = scene.GetNumSpheres();
    // for (int i = 0; i < width; i++){
    //     for (int j = 0; j < height; j++){
    //     // float t[numSpheres]; //interception time
    //     float tmin=999999;
    //     int iClosest=-1;
    //     float u = (halfW - (width)*(i/width));
    //     float v = (halfH - (height)*(j/height));
    //     vec3 p = camera.pos - d*camera.fwd + u*camera.right + v*camera.up;
    //     vec3 rayDir = (p - camera.pos).normalized();  //Normalizing here is optional
    //     Ray ray(camera.pos, rayDir, max_d);
    //     HitInfo hitInfo;
    //     for (int k=0;k<numSpheres;k++){
    //         hitInfo = raySphereIntersect(eye,rayDir,spheres[k].pos,spheres[k].r);
    //         if (hitInfo.hit && hitInfo.t<tmin){
    //         tmin=hitInfo.t;
    //         iClosest=k;
    //         }
    //     }
    //     Color color;
    //     if (iClosest>=0){
    //         Material m=spheres[iClosest].m;
    //         color = GetColor(m,hitInfo);
    //     } else {
    //         color = bgColor;
    //     }
    //     // Color color=s
    //     // else color = Color(0,0,0);
    //     outputImg.setPixel(i,j, color);
    //     //outputImg.setPixel(i,j, Color(fabs(i/imgW),fabs(j/imgH),fabs(0))); //TODO: Try this, what is it visualizing?
    //     }
    // }
// };

Image::~Image(){delete[] pixels;}