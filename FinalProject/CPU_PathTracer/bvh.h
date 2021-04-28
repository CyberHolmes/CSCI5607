#ifndef BVH_H
#define BVH_H 

#include "vec3.h"
#include "objects.h"
#include <vector>
#include <algorithm>

class BoundingSphere {
    vec3 pos;
    float r;
    int depth;
    BoundingSphere* leftChild;
    BoundingSphere* rightChild;    
public:
    //Constructor
    BoundingSphere() : pos(vec3()), r(0), depth(0), leftChild(NULL), rightChild(NULL) {}
    BoundingSphere(vec3 p_, float r_, int d_) : pos(p_), r(r_), depth(d_), leftChild(NULL), rightChild(NULL) {}
    ~BoundingSphere() {}
    //Check whether a ray intersect the sphere
    bool Hit(Ray, float&);
};

class BoundingBox {
    vec3 minPos; //minimum axis value defining the left lower corner of the bounding box
    vec3 maxPos;
    int depth;
    BoundingBox* leftChild;
    BoundingBox* rightChild;
    std::vector<Obj*> objlist;   
public:
    //Constructor
    BoundingBox() : minPos(vec3()), maxPos(vec3()), depth(0), leftChild(NULL), rightChild(NULL) {}
    BoundingBox(vec3 pmin_, vec3 pmax_, int d_) : minPos(pmin_), maxPos(pmax_), depth(d_), leftChild(NULL), rightChild(NULL) {}
    BoundingBox(vec3 pmin_, vec3 pmax_, int d_, std::vector<Obj*> objlist_) : minPos(pmin_), maxPos(pmax_), depth(d_), leftChild(NULL), rightChild(NULL), objlist (objlist_) {}
    ~BoundingBox() {}
    //Check whether a ray intersect the sphere
    bool Hit(Ray, float&);
    void SetLeftChild(BoundingBox* leftChild_) {leftChild = leftChild_;};
    void SetRightChild(BoundingBox* rightChild_) {rightChild = rightChild_;};
    BoundingBox* GetLeftChild() {return leftChild;}
    BoundingBox* GetRightChild() {return rightChild;}
    bool HitWInfo(Ray, HitInfo&);
};

// class leafnode : public BoundingBox{
//     std::list<Obj*> objlist;
// public:
//     leafnode() : BoundingBox() {objlist.get_allocator().allocate(10);}
//     leafnode(vec3 pmin_, vec3 pmax_, int d_, std::list<Obj*> objlist_) : BoundingBox(pmin_, pmax_, d_), objlist (objlist_) {}
//     ~leafnode() {}
//     bool HitWInfo(Ray, HitInfo&) override;
// };

//return root node
BoundingBox* BuildBVHTree(std::vector<Obj*>, vec3, vec3, int);

bool SearchBVHTree(Ray, BoundingBox*, HitInfo&);

void DeallocateBVHTree(BoundingBox*);

#endif