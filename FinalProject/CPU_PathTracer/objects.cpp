#include "objects.h"
#include "glm/glm.hpp"

vec3 Obj::GetPos(){
    return pos;
}
//helper function to calculate normal based on hit position
vec3 CalBoxNormal(vec3 p, vec3 c1, vec3 c8){
    vec3 n = vec3();
    vec3 c2 = vec3(c8.x,c1.y,c1.z);
    vec3 c3 = vec3(c1.x,c8.y,c1.z);
    vec3 c4 = vec3(c1.x,c1.y,c8.z);
    vec3 c5 = vec3(c8.x,c8.y,c1.z);
    vec3 c6 = vec3(c8.x,c1.y,c8.z);
    vec3 c7 = vec3(c1.x,c8.y,c8.z);
    vec3 v1 = p - c1;
    vec3 v2 = p - c2;
    vec3 v3 = p - c3;
    vec3 v4 = p - c4;
    vec3 v5 = p - c5;
    vec3 v6 = p - c6;
    vec3 v7 = p - c7;
    vec3 v8 = p - c8;
    if (v1.lengthsq()<MIN_T){// hit corner 1
        n = vec3(-1,-1,-1).normalized();
    } else if (v2.lengthsq()<MIN_T){// hit corner 1
        n = vec3(1,-1,-1).normalized();
    } else if (v3.lengthsq()<MIN_T){// hit corner 1
        n = vec3(-1,1,-1).normalized();
    } else if (v4.lengthsq()<MIN_T){// hit corner 1
        n =vec3(-1,-1,1).normalized();
    } else if (v5.lengthsq()<MIN_T){// hit corner 1
        n = vec3(1,1,-1).normalized();
    } else if (v6.lengthsq()<MIN_T){// hit corner 1
        n = vec3(1,-1,1).normalized();
    } else if (v7.lengthsq()<MIN_T){// hit corner 1
        n = vec3(-1,1,1).normalized();
    } else if (v8.lengthsq()<MIN_T){// hit corner 1
        n = vec3(1,1,1).normalized();
    } else if (v1.x<MIN_T) { //hit side 1
        //check if hitting edge 1
        if (v1.y<MIN_T) { //Hit edge 1
            n =vec3(-1,-1,0).normalized();
        } else if (v1.z<MIN_T) {
            n =vec3(-1,0,-1).normalized();
        } else if (v3.y<MIN_T) {
            n =vec3(-1,1,0).normalized();
        } else if (v7.z<MIN_T) {
            n =vec3(-1,0,1).normalized();
        }
        n = vec3(-1,0,0);//(t_min>MIN_T)?vec3(-1,0,0):vec3(1,0,0);
    } else if (v1.y<MIN_T){
        if (v1.z<MIN_T){
            n = vec3(0,-1,-1).normalized();
        } else if (v4.z<MIN_T){
            n = vec3(0,-1,1).normalized();
        } else if (v2.x<MIN_T){
            n = vec3(1,-1,0).normalized();
        }
         n = vec3(0,-1,0);
    } else if (v1.z<MIN_T){
        if (v3.y<MIN_T){
            n = vec3(0,1,-1).normalized();
        } else if (v5.x<MIN_T){
            n = vec3(1,0,-1).normalized();
        }
         n = vec3(0,0,-1);
    } else if (v8.x<MIN_T) {
        if (v5.y<MIN_T){
            n = vec3(1,1,0).normalized();
        } else if (v6.z<MIN_T){
            n = vec3(1,0,1).normalized();
        } 
        n = vec3(1,0,0);
    } else if (v8.y<MIN_T){
        if (v7.z<MIN_T){
            n = vec3(0,1,1).normalized();
        } 
         n = vec3(0,1,0);
    } else if (v2.z<MIN_T){
         n = vec3(0,0,1);
    }
    return n;
}
bool Box::Hit(Ray ray, HitInfo& hi) {
    float t_left_x, t_right_x, t_left_y, t_right_y, t_left_z, t_right_z;
    vec3 c1 = GetBoundMin(), c8 = GetBoundMax();
    float fx = (ray.d.x==0)?MAX_T:1/ray.d.x;
    float fy = (ray.d.y==0)?MAX_T:1/ray.d.y;
    float fz = (ray.d.z==0)?MAX_T:1/ray.d.z;
    t_left_x = (c1.x - ray.p.x)*fx;
    t_right_x = (c8.x - ray.p.x)*fx;
    t_left_y = (c1.y - ray.p.y)*fy;
    t_right_y = (c8.y - ray.p.y)*fy;
    t_left_z = (c1.z - ray.p.z)*fz;
    t_right_z = (c8.z - ray.p.z)*fz;

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
    float t = (t_min<MIN_T)?t_max:t_min;
    if (!(t<MAX_T)) return false;
    hi.t = t;
    hi.hitPos = ray.p + hi.t * ray.d;
    hi.hitNorm = (hi.hitPos, c1, c8);
    if (t_min>MIN_T && t_max<MAX_T){
        hi.t2 = t_max;
        hi.hitPos2 = ray.p + hi.t2 * ray.d;
        hi.hitNorm2= (hi.hitPos2, c1, c8);
    }
    // vec3 c2 = vec3(c8.x,c1.y,c1.z);
    // vec3 c3 = vec3(c1.x,c8.y,c1.z);
    // vec3 c4 = vec3(c1.x,c1.y,c8.z);
    // vec3 c5 = vec3(c8.x,c8.y,c1.z);
    // vec3 c6 = vec3(c8.x,c1.y,c8.z);
    // vec3 c7 = vec3(c1.x,c8.y,c8.z);
    // float ctheta = dot(ray.p-c1, ray.p-c8); //>0-inside box, <0-outside box
    // vec3 v1 = hi.hitPos - c1;
    // vec3 v2 = hi.hitPos - c2;
    // vec3 v3 = hi.hitPos - c3;
    // vec3 v4 = hi.hitPos - c4;
    // vec3 v5 = hi.hitPos - c5;
    // vec3 v6 = hi.hitPos - c6;
    // vec3 v7 = hi.hitPos - c7;
    // vec3 v8 = hi.hitPos - c8;
    // if (v1.lengthsq()<MIN_T){// hit corner 1
    //     hi.hitNorm = vec3(-1,-1,-1).normalized();
    // } else if (v2.lengthsq()<MIN_T){// hit corner 1
    //     hi.hitNorm = vec3(1,-1,-1).normalized();
    // } else if (v3.lengthsq()<MIN_T){// hit corner 1
    //     hi.hitNorm = vec3(-1,1,-1).normalized();
    // } else if (v4.lengthsq()<MIN_T){// hit corner 1
    //     hi.hitNorm =vec3(-1,-1,1).normalized();
    // } else if (v5.lengthsq()<MIN_T){// hit corner 1
    //     hi.hitNorm = vec3(1,1,-1).normalized();
    // } else if (v6.lengthsq()<MIN_T){// hit corner 1
    //     hi.hitNorm = vec3(1,-1,1).normalized();
    // } else if (v7.lengthsq()<MIN_T){// hit corner 1
    //     hi.hitNorm = vec3(-1,1,1).normalized();
    // } else if (v8.lengthsq()<MIN_T){// hit corner 1
    //     hi.hitNorm = vec3(1,1,1).normalized();
    // } else if (v1.x<MIN_T) { //hit side 1
    //     //check if hitting edge 1
    //     if (v1.y<MIN_T) { //Hit edge 1
    //         hi.hitNorm =vec3(-1,-1,0).normalized();
    //     } else if (v1.z<MIN_T) {
    //         hi.hitNorm =vec3(-1,0,-1).normalized();
    //     } else if (v3.y<MIN_T) {
    //         hi.hitNorm =vec3(-1,1,0).normalized();
    //     } else if (v7.z<MIN_T) {
    //         hi.hitNorm =vec3(-1,0,1).normalized();
    //     }
    //     hi.hitNorm = vec3(-1,0,0);//(t_min>MIN_T)?vec3(-1,0,0):vec3(1,0,0);
    // } else if (v1.y<MIN_T){
    //     if (v1.z<MIN_T){
    //         hi.hitNorm = vec3(0,-1,-1).normalized();
    //     } else if (v4.z<MIN_T){
    //         hi.hitNorm = vec3(0,-1,1).normalized();
    //     } else if (v2.x<MIN_T){
    //         hi.hitNorm = vec3(1,-1,0).normalized();
    //     }
    //      hi.hitNorm = vec3(0,-1,0);
    // } else if (v1.z<MIN_T){
    //     if (v3.y<MIN_T){
    //         hi.hitNorm = vec3(0,1,-1).normalized();
    //     } else if (v5.x<MIN_T){
    //         hi.hitNorm = vec3(1,0,-1).normalized();
    //     }
    //      hi.hitNorm = vec3(0,0,-1);
    // } else if (v8.x<MIN_T) {
    //     if (v5.y<MIN_T){
    //         hi.hitNorm = vec3(1,1,0).normalized();
    //     } else if (v6.z<MIN_T){
    //         hi.hitNorm = vec3(1,0,1).normalized();
    //     } 
    //     hi.hitNorm = vec3(1,0,0);
    // } else if (v8.y<MIN_T){
    //     if (v7.z<MIN_T){
    //         hi.hitNorm = vec3(0,1,1).normalized();
    //     } 
    //      hi.hitNorm = vec3(0,1,0);
    // } else if (v2.z<MIN_T){
    //      hi.hitNorm = vec3(0,0,1);
    // }
    
    hi.m = materialList[midx];
    hi.rayDepth = ray.depth;  
    hi.v = -ray.d;
    return true;
};
//Helper function to find positive max value in a array, also returns the index
float FindMax(float* arr, int size, int& idx){
    float maxv = 0;
    idx = -1;
    for (int i=0;i<size;i++){
        if (arr[i]>maxv && arr[i]<MAX_T){
            maxv = arr[i];
            idx = i;
        }
    }
    return maxv;
}
bool CSG::Hit(Ray ray, HitInfo& hi) {
    bool hit11, hit21, hit12, hit22;
    HitInfo hi11, hi21, hi12, hi22; //2 hit information per object
    
    hit11 = obj1->Hit(ray,hi11);    
    hit21 = obj2->Hit(ray,hi21);
    if (!hit11 && !hit21) return false;
    float ctheta11 = dot(hi11.hitNorm,ray.d), ctheta21 = dot(hi21.hitNorm,ray.d);
    switch (ut){
        case 0: //union
            if (ctheta11>0 || ctheta21>0){ //inside
                float a[] = {hi11.t, hi11.t2, hi21.t, hi21.t2};
                int idx;
                hi.t = FindMax(a,4,idx);
                switch (idx){
                    case 0:
                    hi = hi11;
                    break;
                    case 1:
                    hi = hi11;
                    hi.t = hi11.t2;
                    hi.hitNorm = hi11.hitNorm2;
                    hi.hitPos = hi11.hitPos2;
                    return true;
                    case 2:
                    hi = hi21;
                    break;
                    case 3:
                    hi = hi21;
                    hi.t = hi21.t2;
                    hi.hitNorm = hi21.hitNorm2;
                    hi.hitPos = hi21.hitPos2;
                    return true;
                    default:
                    assert(false);
                    return false;
                }
            }
            hi = (hi11.t<hi21.t)? hi11:hi21;
            return true;
            break;
        case 1: //intersect
            if (!(hit11 && hit21)) return false; //didn't hit both
            if (ctheta11>0 || ctheta21>0){ //inside
                hi = (hi11.t<hi21.t)?hi11:hi21;
                return true;
            }
            hi = (hi11.t<hi21.t)?hi21:hi11;
            return true;
            break;
        default: //difference obj1 - obj2
            if (!hit11) return false; //must hit the first object
            if (hi11.t<hi21.t){
                // if (ctheta21>0){ //inside object 2
                //     if (ctheta11>0) return false;
                //     hi = hi21;
                //     hi.hitNorm = -hi.hitNorm;
                //     return true;
                // }
                hi = hi11;
                return true;                
            }
            // if (ctheta21>0){
            //     hi = hi21;
            //     hi.hitNorm = -hi.hitNorm;
            //     return true;
            // }
            if (hi11.t2 < hi21.t2) return false;
            hi = hi21;
            hi.t = hi21.t2;
            hi.hitNorm = -hi21.hitNorm2;
            hi.hitPos = hi21.hitPos2;
            // if (hit21 && (hi21.t<=hi11.t)){
            // hit the not obj first
            // Ray ray2 = ray;
            // vec3 projd1 = Projection(ray.p,ray.d);
            // vec3 projd2 = Projection(obj2->GetPos(),ray.d);
            // // vec3 c = ray.p - projd1;            
            // ray2.p = 2*projd2 - 2*projd1+ray.p;
            // ray2.d = -ray.d;
            // hit12 = obj1->Hit(ray2,hi12);
            // // hi12.hitNorm = -hi12.hitNorm;
            // // hi12.v = -hi12.v;
            // hit22 = obj2->Hit(ray2,hi22);
            // if (hi22.t<hi12.t) return false; // the ray misses
            // hi22.hitNorm = -hi22.hitNorm;
            // hi22.v = ray2.d; 
            // hi = hi22;
            return true;
            // }
            // hi = hi11;
            return true; 
    }    
};
vec3 CSG::GetBoundMin() {
    float minx, miny, minz;
    vec3 minV1=obj1->GetBoundMin();
    vec3 minV2=obj2->GetBoundMin();
    minx = (minV1.x<minV2.x)?minV1.x:minV2.x;
    miny = (minV1.y<minV2.y)?minV1.y:minV2.y;
    minz = (minV1.z<minV2.z)?minV1.z:minV2.z;
    return vec3(minx,miny,minz);
};
vec3 CSG::GetBoundMax() {
    float maxx, maxy, maxz;
    vec3 maxV1=obj1->GetBoundMax();
    vec3 maxV2=obj2->GetBoundMax();
    maxx = (maxV1.x>maxV2.x)?maxV1.x:maxV2.x;
    maxy = (maxV1.y>maxV2.y)?maxV1.y:maxV2.y;
    maxz = (maxV1.z>maxV2.z)?maxV1.z:maxV2.z;
    return vec3(maxx,maxy,maxz);
};

vec3 Box::GetBoundMin() {
    return pos - vec3(l_h,l_h,l_h)-vec3(MIN_T,MIN_T,MIN_T);
};
vec3 Box::GetBoundMax() {
    return pos + vec3(l_h,l_h,l_h)+vec3(MIN_T,MIN_T,MIN_T);
};

bool Plane::Hit(Ray ray, HitInfo& hi) {
    float ctheta = dot(ray.d,n);
    float t = (ctheta>0)?(-(dot(-n,pos) - dot(-n,ray.p))/ctheta) : ((dot(n,pos) - dot(n,ray.p))/ctheta);
    if (t<MIN_T) return false;

    hi.hitPos = ray.p + t * ray.d; // intercept point
    hi.hitNorm = (ctheta<0)?n:-n;
    hi.m = materialList[midx];
    hi.rayDepth = ray.depth;  
    hi.v = -ray.d;
    hi.t = t;
    return true;
};
vec3 Plane::GetBoundMin() {
    return vec3(-MAX_T, -MAX_T, -MAX_T);
};
vec3 Plane::GetBoundMax() {
    return vec3(MAX_T, MAX_T, MAX_T);
};

bool Sphere::Hit(Ray ray, HitInfo& hi) {
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
        if (t1<t2){
            hi.t = t1; hi.t2 = t2;
        } else {
            hi.t = t2; hi.t2 = t1;
        }
        if (hi.t < MAX_T){                
            hi.hitPos = ray.p + hi.t*ray.d;
            hi.hitNorm = (hi.hitPos-pos).normalized();
            hi.v = -ray.d; //p - hi.hitPos;
            hi.m = materialList[midx];
            hi.rayDepth = ray.depth;
            if (hi.t2 < MAX_T){
                hi.hitPos2 = ray.p + hi.t2*ray.d;
                hi.hitNorm2 = (hi.hitPos2-pos).normalized();
            }
            return true;
        }         
    }
    return false;
};

vec3 Sphere::GetBoundMin(){
    return pos - vec3(r,r,r)-vec3(MIN_T,MIN_T,MIN_T);
}
vec3 Sphere::GetBoundMax(){
    return pos + vec3(r,r,r)+vec3(MIN_T,MIN_T,MIN_T);
}

void Triangle::addTexture(int uv1, int uv2, int uv3, int tex_idx, int norm_idx) {
    uvP[0] = uv1;
    uvP[1] = uv2;
    uvP[2] = uv3;
    texture_idx = tex_idx;
    normal_map_idx = norm_idx;
}

bool Triangle::Hit(Ray ray, HitInfo& hi){
    vec3 v1 = vertexList[vertexP[0]];
    vec3 v2 = vertexList[vertexP[1]];
    vec3 v3 = vertexList[vertexP[2]];

     //calculate triangle normal
    vec3 n = cross((v3-v1),(v2-v1)).normalized();
    float ctheta = dot(ray.d,n);
    if (fabs(ctheta)<MIN_T) return false; //ray is parallel to triangle plane
 
    float t = (ctheta>0)?(-(dot(-n,v1) - dot(-n,ray.p))/ctheta) : ((dot(n,v1) - dot(n,ray.p))/ctheta);
    if (t<MIN_T) return false;

    vec3 p0 = ray.p + t * ray.d; // intercept point
    //check if the point is inside triangle
    double a = Area(v1,v2,v3);
    double a3 = Area(v1, p0, v2)/a;
    double a1 = Area(v2, p0, v3)/a;
    double a2 = Area(v1, p0, v3)/a;
    
    if ((a1<1) && (a2<1) && (a3<1) && (a1 + a2 + a3)<1.000001){
        hi.hitPos = p0;
        hi.hitNorm = (ctheta<0)?n:-n;
        // hi.hitNorm = (posn)? n :-n;
        hi.m = materialList[midx];
        hi.rayDepth = ray.depth;  
        hi.v = -ray.d;
        hi.t = t;
        hi.texture_map = (texture_idx >= 0) ? &textureList[texture_idx] : nullptr;
        if (texture_idx >= 0) {
            hi.tex_coord = a1 * uvList[uvP[0]] + a2 * uvList[uvP[1]] + a3 * uvList[uvP[2]];
        }
        if (normal_map_idx >= 0) {
            hi.tex_coord = a1 * uvList[uvP[0]] + a2 * uvList[uvP[1]] + a3 * uvList[uvP[2]];
            // create a Tangent, BiTangent, Normal Matrix and use it to get the normal at this point
            vec3 edge1 = vertexList[vertexP[2]] - vertexList[vertexP[0]];
            vec3 edge2 = vertexList[vertexP[1]] - vertexList[vertexP[0]];
            vec3 uv1 = uvList[uvP[2]] - uvList[uvP[0]];
            vec3 uv2 = uvList[uvP[1]] - uvList[uvP[0]];
            glm::vec3 tangent, bitangent, normal;
            float f = 1.0f / (uv1.x * uv2.y - uv2.x * uv1.y);

            tangent.x = f * (uv2.y * edge1.x - uv1.y * edge2.x);
            tangent.y = f * (uv2.y * edge1.y - uv1.y * edge2.y);
            tangent.z = f * (uv2.y * edge1.z - uv1.y * edge2.z);
            tangent = glm::normalize(tangent);

            normal.x = hi.hitNorm.x;
            normal.y = hi.hitNorm.y;
            normal.z = hi.hitNorm.z;
            normal = glm::normalize(normal);

            bitangent = glm::cross(normal, tangent);

            glm::mat3 tbn(tangent, bitangent, normal);
            // get the color (normal) in the image
            Color normal_in_rgb = textureList[normal_map_idx].Sample(hi.tex_coord.x, hi.tex_coord.y);
            // convert the color into a vector
            glm::vec3 norm_in_map;
            norm_in_map.x = 2 * normal_in_rgb.r - 1.0;
            norm_in_map.y = 2 * normal_in_rgb.g - 1.0;
            norm_in_map.z = 2 * normal_in_rgb.b - 1.0;
            // bring the normal into the triangle's basis
            glm::vec3 triangle_pt_normal = tbn * norm_in_map;
            hi.hitNorm = vec3(triangle_pt_normal.x, triangle_pt_normal.y, triangle_pt_normal.z);
        }
        return true;    
    }
    return false;
}

// float mymin(float a, float b, float c){
//     float out = MAX_T;
//     out = (out>a)? a:out;
//     out = (out>b)? b:out;
//     out = (out>c)? c:out;
//     return out;
// }

// float mymax(float a, float b, float c){
//     float out = -MAX_T;
//     out = (out<a)? a:out;
//     out = (out<b)? b:out;
//     out = (out<c)? c:out;
//     return out;
// }

vec3 Triangle::GetBoundMin(){
    float minx = mymin(vertexList[vertexP[0]].x, vertexList[vertexP[1]].x,vertexList[vertexP[2]].x);
    float miny = mymin(vertexList[vertexP[0]].y, vertexList[vertexP[1]].y,vertexList[vertexP[2]].y);
    float minz = mymin(vertexList[vertexP[0]].z, vertexList[vertexP[1]].z,vertexList[vertexP[2]].z);
    return vec3(minx,miny,minz)-vec3(MIN_T,MIN_T,MIN_T);
}

vec3 Triangle::GetBoundMax(){
    float maxx = mymax(vertexList[vertexP[0]].x, vertexList[vertexP[1]].x,vertexList[vertexP[2]].x);
    float maxy = mymax(vertexList[vertexP[0]].y, vertexList[vertexP[1]].y,vertexList[vertexP[2]].y);
    float maxz = mymax(vertexList[vertexP[0]].z, vertexList[vertexP[1]].z,vertexList[vertexP[2]].z);
    return vec3(maxx,maxy,maxz)+vec3(MIN_T,MIN_T,MIN_T);
}

bool TriangleNormal::Hit(Ray ray, HitInfo& hi){
    vec3 v1 = vertexList[vertexP[0]];
    vec3 v2 = vertexList[vertexP[1]];
    vec3 v3 = vertexList[vertexP[2]];

     //calculate triangle normal
    vec3 n = cross((v3-v1),(v2-v1)).normalized();
    float ctheta = dot(ray.d,n);
    if (fabs(ctheta)<MIN_T) return false; //ray is parallel to triangle plane
    
     float t = (ctheta>0)?(-(dot(-n,v1) - dot(-n,ray.p))/ctheta) : ((dot(n,v1) - dot(n,ray.p))/ctheta);
    if (t<MIN_T) return false;

    vec3 p0 = ray.p + t * ray.d; // intercept point
    //check if the point is inside triangle
    long double a = Area(v1,v2,v3);
    long double a3 = Area(v1, p0, v2)/a;
    long double a1 = Area(v2, p0, v3)/a;
    long double a2 = Area(v1, p0, v3)/a;  
    // if (a<0.0001) {printf("a=%Lf, a1=%Lf, a2=%Lf, a3=%Lf\n",a,a1,a2,a3); assert(false);}

    if ((a1<1) && (a2<1) && (a3<1) && (a1 + a2 + a3)<1.0001){
        hi.hitPos = p0;
        vec3 temp_n = a1*normalList[normalP[0]] + a2*normalList[normalP[1]] + a3*normalList[normalP[2]];
        hi.hitNorm = (ctheta>0)? temp_n :-temp_n;
        hi.m = materialList[midx];
        hi.rayDepth = ray.depth;
        hi.v = -ray.d;
        hi.t = t;
        hi.texture_map = (texture_idx >= 0) ? &textureList[texture_idx] : nullptr;
        if (texture_idx >= 0) {
            hi.tex_coord = a1 * uvList[uvP[0]] + a2 * uvList[uvP[1]] + a3 * uvList[uvP[2]];
        }
        return true;       
    }
    return false;
}

Color PointLight::Shading(HitInfo hi) {
    vec3 l = (p - hi.hitPos).normalized();     
    float diffuse_a = dot(hi.hitNorm,l);
    diffuse_a = (diffuse_a>0)? diffuse_a : 0;
    // vec3 h = (1.0/(hi.v + l).length())*(hi.v + l);   
    // float specular_a = dot(hi.hitNorm,h.normalized());
    vec3 r = -l+2*dot(l,hi.hitNorm)*hi.hitNorm;
    float specular_a = dot(hi.v,r.normalized());
    specular_a = (specular_a>0)? pow(specular_a,hi.m.ns) : 0;
    // float fade_f = 1/((p.x-hi.hitPos.x)*(p.x-hi.hitPos.x)+(p.y-hi.hitPos.y)*(p.y-hi.hitPos.y)+(p.z-hi.hitPos.z)*(p.z-hi.hitPos.z));
    float fade_f = 1/(p-hi.hitPos).lengthsq(); 
    
    Color c_out = (hi.m.dc * diffuse_a + hi.m.sc * specular_a) * c * fade_f ;
    return c_out ;
};

vec3 PointLight::CalDir(vec3 p0){
    return (p-p0).normalized();
}

Color SpotLight::Shading(HitInfo hi) {

    Color c_out;
    vec3 ldir = (p - hi.hitPos).normalized(); //direction to the light    
    // angle between the direction of the light and the direction to the hit
    float angle = dot(-ldir.normalized(), d.normalized());
    if (angle>0 && angle > a2){
        float term = (angle<a1)?(1.0-(a1-angle)/(a1-a2)) : 1.0;
        float diffuse_a = dot(hi.hitNorm,ldir);
        // float diffuse_a = dot(-d.normalized(),ldir);
        diffuse_a = (diffuse_a>0)? diffuse_a : 0;
        // vec3 h = (1.0/(hi.v + ldir).length())*(hi.v + ldir);
        // float specular_a = dot(hi.hitNorm,h.normalized());
        vec3 r = -ldir+2*dot(ldir,hi.hitNorm)*hi.hitNorm;
        float specular_a = dot(hi.v,r.normalized());
        specular_a = (specular_a>0)? pow(specular_a,hi.m.ns) : 0;
        float fade_f = 1/(p-hi.hitPos).lengthsq();           
        c_out = (hi.m.dc * diffuse_a + hi.m.sc * specular_a) * c * fade_f *term ;
    } else {
        c_out = Color(0,0,0);
    }
    return c_out ;
};

vec3 SpotLight::CalDir(vec3 p0){
    return (p-p0).normalized();
}

Color DirectionLight::Shading(HitInfo hi) {
    
    float diffuse_a = dot(hi.hitNorm.normalized(),-d.normalized());
    diffuse_a = (diffuse_a>0)? diffuse_a : 0;
    // vec3 h = (1/(hi.v + -d).length())*(hi.v + -d);
    // float specular_a = dot(hi.hitNorm.normalized(),h.normalized());
    vec3 r = d+2*dot(-d,hi.hitNorm)*hi.hitNorm;
    float specular_a = dot(hi.v,r.normalized());
    specular_a = (specular_a>0)? pow(specular_a,hi.m.ns) : 0;
    
    Color c_out = (hi.m.dc * diffuse_a + hi.m.sc * specular_a) * c ;
    return c_out ;
};

vec3 DirectionLight::CalDir(vec3 p0){
    return -d.normalized();
}