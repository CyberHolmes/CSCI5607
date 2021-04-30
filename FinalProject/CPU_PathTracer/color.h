#ifndef COLOR_H
#define COLOR_H

struct Color{
  float r,g,b;

  Color(float r, float g, float b) : r(r), g(g), b(b) {}
  Color() : r(0), g(0), b(0) {}

  Color operator+ (const Color& c){
    return Color(r+c.r, g+c.g, b+c.b);
  };
  Color operator* (const Color& c){
    return Color(r*c.r, g*c.g, b*c.b);
  };
  Color operator* (float f){
    return Color(r * f, g * f, b * f);
  };
  float luminance(){
    return 0.299*r + 0.587*g + 0.114*b;
  }
  float diff(Color& c){
    float t = luminance()-c.luminance();
    return (t>0)?t:-t;
  }
  float mag(){
    return r+g+b;
  }
};

#endif