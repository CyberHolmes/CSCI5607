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
};

#endif