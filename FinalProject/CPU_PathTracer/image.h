#ifndef IMAGE_H
#define IMAGE_H


#include <cstdint>
#include <cstring>
#include <math.h>
#include <cstdlib>
#include "color.h"

#include "stb_image.h"
#include "stb_image_write.h"
// #include "hitinfo.h"

class Image{
private:
    int width;
    int height;
    Color* pixels;    
public:
    uint8_t* rawPixels;
    //Construct an empty image with width and height
    Image(int, int);
    //copy constructor
    Image(const Image& cpy);
    //Assignment constructor
    Image& operator=(const Image&);
    //construct an image from a file
    Image(const char*);
    //Set pixel color
    void SetPixel(int i, int j, Color c);
    //Get pixel color
    Color& GetPixel(int, int);
    //Sample around the pixel
    Color Sample(float x, float y);
    //Convert pixel data to bytes
    void UpdateRawPixels();
    //Output image to a file
    void Write(const char*);
    //Render image
    // void Render(Scene&, Camera&, int);
    //destructor
    ~Image();
};

#endif