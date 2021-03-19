#include "bvh.h"
// #include "vec3.h"
#include <iostream>

bool BoundingSphere::Hit(Ray ray, float& t){
    float a = dot(ray.d,ray.d);
    vec3 toStart = (ray.p - pos);
    float b = 2 * dot(ray.d,toStart);
    float c = dot(toStart,toStart) - r*r;
    float discr = b*b - 4*a*c;
    if (discr > 0){
        float t1 = (-b + sqrt(discr))/(2*a);
        float t2 = (-b - sqrt(discr))/(2*a);
        t1 = (t1 > MIN_T)? t1 : MAX_T;
        t2 = (t2 > MIN_T)? t2 : MAX_T;
        t = (t1 < t2)? t1 : t2;
        if (t < MAX_T){                
            return true;
        }         
    }
    return false;
}

bool BoundingBox::Hit(Ray ray, float& t){
    float t_left_x, t_right_x, t_left_y, t_right_y, t_left_z, t_right_z;
    t_left_x = (minPos.x - ray.p.x)/ray.d.x;
    t_right_x = (maxPos.x - ray.p.x)/ray.d.x;
    t_left_y = (minPos.y - ray.p.y)/ray.d.y;
    t_right_y = (maxPos.y - ray.p.y)/ray.d.y;
    t_left_z = (minPos.z - ray.p.z)/ray.d.z;
    t_right_z = (maxPos.z - ray.p.z)/ray.d.z;

    float t_max_x = (t_left_x>t_right_x)?t_left_x:t_right_x;
    float t_max_y = (t_left_y>t_right_y)?t_left_y:t_right_y;
    float t_max_z = (t_left_z>t_right_z)?t_left_z:t_right_z;
    float t_max = mymin(t_max_x,t_max_y,t_max_z);    

    float t_min_x = (t_left_x<t_right_x)?t_left_x:t_right_x;
    float t_min_y = (t_left_y<t_right_y)?t_left_y:t_right_y;
    float t_min_z = (t_left_z<t_right_z)?t_left_z:t_right_z;
    float t_min = mymax(t_min_x,t_min_y,t_min_z);
    


    if (t_max < MIN_T) return false;
    if (t_min > t_max) return false;
    if (t_min > MAX_T) return false;
    // if (t_min < 0) return false;
    // printf("t_left_x=%f,t_right_x=%f\n",t_left_x,t_right_x);
    // printf("t_max_x=%f, t_max_y=%f, t_max_z=%f",t_max_x,t_max_y,t_max_z);
    // printf("t_max=%f\n",t_max);
    // printf("t_min_x=%f, t_min_y=%f, t_min_z=%f",t_min_x,t_min_y,t_min_z);
    // printf("t_min=%f\n",t_min);
    // printf("minPos=%f,%f,%f\n", minPos.x,minPos.y,minPos.z);
    // printf("maxPos=%f,%f,%f\n", maxPos.x,maxPos.y,maxPos.z);
    // printf("t_max=%f, t_min=%f\n",t_max,t_min);
    // assert(false);

    t = (t_min<0)?t_max:t_min;
    return t_min<=t_max;
}

bool BoundingBox::HitWInfo(Ray ray, HitInfo& hi){
    HitInfo hi_temp = HitInfo();
    bool hit = false;
    for (auto& obj : objlist){
        bool curhit = obj->Hit(ray,hi_temp);
        if (curhit && hi_temp.t < hi.t){
            hi = hi_temp;
            hit = true;
        }
    }
    return hit;
}

bool comp_x(Obj* x, Obj* y){
    return (x->GetPos().x < y->GetPos().x);
    // return (x->GetBoundMin().x < y->GetBoundMin().x);
}

bool comp_y(Obj* x, Obj* y){
    return (x->GetPos().y < y->GetPos().y);
    // return (x->GetBoundMin().y < y->GetBoundMin().y);
}

bool comp_z(Obj* x, Obj* y){
    return (x->GetPos().z < y->GetPos().z);
    // return (x->GetBoundMin().z < y->GetBoundMin().z);
}

float EvalBBSplit(std::vector<Obj*> objList, int numObj, int axis, std::vector<Obj*> &objsublist1, std::vector<Obj*> &objsublist2,
    vec3 &maxVleft, vec3 &minVleft, vec3 &maxVright, vec3 &minVright){ //axis = 0 - x, 1-y,2-z
    switch (axis)
    {
    case 0:
        std::sort(objList.begin(),objList.end(),comp_x);
        // objList.sort(comp_x);
        break;
    case 1:
        std::sort(objList.begin(),objList.end(),comp_y);
        // objList.sort(comp_y);
        break;    
    default:
        std::sort(objList.begin(),objList.end(),comp_z);
        // objList.sort(comp_z);
        break;
    }
    // std::list<Obj*> objsublist1, objsublist2;
    int numObj1 = numObj/2, numObj2 = numObj - numObj1;
    // printf("inside split f: axis=%d, numObj=%d,numObj1=%d,numObj2=%d\n",axis,numObj,numObj1,numObj2);
    objsublist1.get_allocator().allocate(numObj1);
    objsublist2.get_allocator().allocate(numObj2);
    // vec3 minVleft = vec3(MAX_T,MAX_T,MAX_T), minVright = vec3(MAX_T,MAX_T,MAX_T);
    // vec3 maxVleft = vec3(-MAX_T,-MAX_T,-MAX_T), maxVright = vec3(-MAX_T,-MAX_T,-MAX_T);
    int i = 0;
    for ( auto const& obj : objList){
        if (i<numObj1) {
            objsublist1.emplace_back(obj);
            vec3 curMinVleft = obj->GetBoundMin();
            vec3 curMaxVleft = obj->GetBoundMax();
            minVleft.x = (minVleft.x>curMinVleft.x)?curMinVleft.x:minVleft.x;
            minVleft.y = (minVleft.y>curMinVleft.y)?curMinVleft.y:minVleft.y;
            minVleft.z = (minVleft.z>curMinVleft.z)?curMinVleft.z:minVleft.z;
            maxVleft.x = (maxVleft.x<curMaxVleft.x)?curMaxVleft.x:maxVleft.x;
            maxVleft.y = (maxVleft.y<curMaxVleft.y)?curMaxVleft.y:maxVleft.y;
            maxVleft.z = (maxVleft.z<curMaxVleft.z)?curMaxVleft.z:maxVleft.z;
        }
        else {
            objsublist2.emplace_back(obj);
            vec3 curMinVright = obj->GetBoundMin();
            vec3 curMaxVright = obj->GetBoundMax();
            minVright.x = (minVright.x>curMinVright.x)?curMinVright.x:minVright.x;
            minVright.y = (minVright.y>curMinVright.y)?curMinVright.y:minVright.y;
            minVright.z = (minVright.z>curMinVright.z)?curMinVright.z:minVright.z;
            maxVright.x = (maxVright.x<curMaxVright.x)?curMaxVright.x:maxVright.x;
            maxVright.y = (maxVright.y<curMaxVright.y)?curMaxVright.y:maxVright.y;
            maxVright.z = (maxVright.z<curMaxVright.z)?curMaxVright.z:maxVright.z;
        }
        i++;
    }
    return (minVright-minVleft).lengthsq()+(maxVright-maxVleft).lengthsq();
}

BoundingBox* BuildBVHTree(std::vector<Obj*> objList, vec3 minV, vec3 maxV, int depth){
    //Get number of objects
    int numObj = objList.size();    
    if (numObj==1 || depth<=0){
        BoundingBox* leaf = new BoundingBox(minV, maxV,0, objList);
        return leaf;
    }
    std::vector<Obj*> objsublist1, objsublist2;
    vec3 minVleft = vec3(MAX_T,MAX_T,MAX_T), minVright = vec3(MAX_T,MAX_T,MAX_T);
    vec3 maxVleft = vec3(-MAX_T,-MAX_T,-MAX_T), maxVright = vec3(-MAX_T,-MAX_T,-MAX_T);
    float dx = EvalBBSplit(objList,numObj,0, objsublist1, objsublist2,maxVleft, minVleft, maxVright, minVright);
    objsublist1.clear();objsublist2.clear();
    float dy = EvalBBSplit(objList,numObj,1, objsublist1, objsublist2,maxVleft, minVleft, maxVright, minVright);
    objsublist1.clear();objsublist2.clear();
    float dz = EvalBBSplit(objList,numObj,2, objsublist1, objsublist2,maxVleft, minVleft, maxVright, minVright);
    objsublist1.clear();objsublist2.clear();
    if (dx<0.000000001 && dy<0.000000001 && dz<0.000000001){
        // printf("no good split.\n");
        BoundingBox* leaf = new BoundingBox(minV, maxV,0, objList);
        return leaf;
    }
    if (dx>=dy && dx>=dz) {dx = EvalBBSplit(objList,numObj,0, objsublist1, objsublist2,maxVleft, minVleft, maxVright, minVright);}
    else if (dy>=dx && dy>=dz) {dy = EvalBBSplit(objList,numObj,1, objsublist1, objsublist2,maxVleft, minVleft, maxVright, minVright);}
    else {dz = EvalBBSplit(objList,numObj,2, objsublist1, objsublist2,maxVleft, minVleft, maxVright, minVright);}
    BoundingBox* curRoot = new BoundingBox(minV,maxV, depth);
    BoundingBox* LeftNode = BuildBVHTree(objsublist1,minVleft, maxVleft, depth-1);
    BoundingBox* RightNode = BuildBVHTree(objsublist2,minVright, maxVright, depth-1);
    // printf("depth=%d\n",depth);
    // if (depth<10)assert(false);
    curRoot->SetLeftChild(LeftNode);
    curRoot->SetRightChild(RightNode);
    return curRoot;
};

bool SearchBVHTree(Ray ray, BoundingBox* BB, HitInfo& hi){
    bool hit = false;
    float t = MAX_T;
    bool curhit = BB->Hit(ray,t);   
    if (curhit){
        bool curhitleft=false, curhitright=false;
        HitInfo hi_left = HitInfo(), hi_right = HitInfo();
        // printf("in root. hi.t=%f\n",hi.t);
        if (BB->GetLeftChild() != NULL){
            
            curhitleft = SearchBVHTree(ray,BB->GetLeftChild(),hi_left);
            // if (curhitleft) printf("hit left child, hi_left.t=%f\n",hi_left.t);
        }
        if (BB->GetRightChild() != NULL){
            // printf("check right child\n");
            curhitright = SearchBVHTree(ray,BB->GetRightChild(),hi_right);
            // if (curhitleft) printf("hit right child, hi_right.t=%f\n",hi_right.t);
        }
        if (curhitleft || curhitright) {
            hi = (hi_left.t < hi_right.t)? hi_left : hi_right;
            hit = hi.t<(MAX_T-1);
            // if (hit) printf("hi.t=%f\n",hi.t);
        }        
        if (BB->GetLeftChild() == NULL && BB->GetRightChild() == NULL){
            // printf("in leaf\n");
            hit = BB->HitWInfo(ray,hi);
            // if (hit) printf("in leaf. hi.t=%f\n",hi.t);
        }
        // printf("hi_left.t=%f, hi_right.t=%f, hi.t=%f\n",hi_left.t,hi_right.t, hi.t);
    }
    // std::cout<<(hi.t<MAX_T)<<std::endl;assert(false);
    return hit;
};

void DeallocateBVHTree(BoundingBox* BB){
    if (BB==NULL) return;
    DeallocateBVHTree(BB->GetLeftChild());
    DeallocateBVHTree(BB->GetRightChild());
    delete BB;
};