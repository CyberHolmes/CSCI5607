//UMN CSCI 5607 2D Geometry Library Homework [HW0]
//TODO: For the 18 functions below, replace their sub function with a working version that matches the desciption.

#ifndef GEOM_LIB_H
#define GEOM_LIB_H

#include "pga.h"

//Displace a point p on the direction d
//The result is a point
Point2D move(Point2D p, Dir2D d){
  //return Point2D(0,0); //Wrong, fix me...
  return p+d;
}

//Compute the displacement vector between points p1 and p2
//The result is a direction 
Dir2D displacement(Point2D p1, Point2D p2){
  //return Dir2D(0,0); //Wrong, fix me...
  return p2-p1;
}

//Compute the distance between points p1 and p2
//The result is a scalar 
float dist(Point2D p1, Point2D p2){
  //return 0; //Wrong, fix me...
  return vee(p1,p2).magnitude();
}

//Compute the perpendicular distance from the point p the the line l
//The result is a scalar 
float dist(Line2D l, Point2D p){
  //return 0; //Wrong, fix me...
  return vee(p,l.normalized());
}

//Compute the perpendicular distance from the point p the the line l
//The result is a scalar 
float dist(Point2D p, Line2D l){
  return vee(p,l.normalized());
}

//Compute the intersection point between lines l1 and l2
//You may assume the lines are not parallel
//The results is a a point that lies on both lines
Point2D intersect(Line2D l1, Line2D l2){
  return wedge(l1,l2);
}

//Compute the line that goes through the points p1 and p2
//The result is a line 
Line2D join(Point2D p1, Point2D p2){
  return vee(p1,p2);
}

//Compute the projection of the point p onto line l
//The result is the closest point to p that lies on line l
Point2D project(Point2D p, Line2D l){
  return dot(p,l)*l;
}

//Compute the projection of the line l onto point p
//The result is a line that lies on point p in the same direction of l
Line2D project(Line2D l, Point2D p){
  return dot(l,p)*p;
}

//Compute the angle point between lines l1 and l2
//You may assume the lines are not parallel
//The results is a scalar
float angle(Line2D l1, Line2D l2){
  return acos(dot(l1.normalized(),l2.normalized()));
}

//Compute if the line segment p1->p2 intersects the line segment a->b
//The result is a boolean
bool segmentSegmentIntersect(Point2D p1, Point2D p2, Point2D a, Point2D b){
  Line2D l1 = vee(p1,p2);
  Line2D l2 = vee(a,b);
  if (sign(vee(a,l1)) * sign(vee(b,l1))>0) {
    return false;
  } else if (sign(vee(p1,l2))*sign(vee(p2,l2))>0){
    return false;
  }
  return true;
}

//Compute if the point p lies inside the triangle t1,t2,t3
//Your code should work for both clockwise and counterclockwise windings
//The result is a bool
bool pointInTriangle(Point2D p, Point2D t1, Point2D t2, Point2D t3){
  Line2D l1 = vee(t1,t2), l2 = vee(t2,t3), l3 = vee(t3,t1);
  if (!(vee(p,l1)>0) == (vee(p,l2)>0)){
    return false;
  } else if (!(vee(p,l2)>0) == (vee(p,l3)>0)){
    return false;
  }
  return true;
}

//Compute the area of the triangle t1,t2,t3
//The result is a scalar
float areaTriangle(Point2D t1, Point2D t2, Point2D t3){
  return vee(t1,t2,t3)/2;
}

//Compute distance between point and a line segment defined by 2 points
float pointSegmentDistance(Point2D p, Point2D a, Point2D b){
  Line2D l = vee(a,b);
  float l_len = dist(a,b);
  Point2D pProj = project(p,l);
  if (dist(a,pProj)>l_len) {pProj=b;}
  else if (dist(b,pProj)>l_len) {pProj=a;}
  return dist(pProj,p);
}

//Compute the distance from the point p to the triangle t1,t2,t3 as defined 
//by it's distance from the edge closest to p.
//The result is a scalar
float pointTriangleEdgeDist(Point2D p, Point2D t1, Point2D t2, Point2D t3){
  float d1 = pointSegmentDistance(p,t1,t2);
  float d2 = pointSegmentDistance(p,t2,t3);
  float d3 = pointSegmentDistance(p,t3,t1);
  float dmin = d1;
  if (d2<dmin) dmin = d2;
  if (d3<dmin) dmin = d3;
  return dmin;
}

//Compute the distance from the point p to the closest of three corners of
// the triangle t1,t2,t3
//The result is a scalar
float pointTriangleCornerDist(Point2D p, Point2D t1, Point2D t2, Point2D t3){
  float d1 = dist(p,t1);
  float d2 = dist(p,t2);
  float d3 = dist(p,t3);
  float dmin = d1;
  if (d2<dmin) dmin=d2;
  if (d3<dmin) dmin=d3;
  return dmin;
}

//Compute if the quad (p1,p2,p3,p4) is convex.
//Your code should work for both clockwise and counterclockwise windings
//The result is a boolean
bool isConvex_Quad(Point2D p1, Point2D p2, Point2D p3, Point2D p4){
  return segmentSegmentIntersect(p1,p3,p2,p4);
}

//Compute the reflection of the point p about the line l
//The result is a point
Point2D reflect(Point2D p, Line2D l){
  return l.normalized()*p*l.normalized(); //Wrong, fix me...
}

//Compute the reflection of the line d about the line l
//The result is a line
Line2D reflect(Line2D d, Line2D l){
  return l.normalized()*d*l.normalized(); //Wrong, fix me...
}

#endif