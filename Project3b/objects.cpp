#include "objects.h"

vec3 Obj::GetPos(){
    return pos;
}

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
        hi.t = (t1 < t2)? t1 : t2;
        if (hi.t < MAX_T){                
            hi.hitPos = ray.p + hi.t*ray.d;
            hi.hitNorm = (hi.hitPos-pos).normalized();
            hi.v = -ray.d; //p - hi.hitPos;
            hi.m = materialList[midx];
            hi.rayDepth = ray.depth;
            return true;
        }         
    }
    return false;
};
vec3 Sphere::GetBoundMin(){
    return pos - vec3(r,r,r);
}
vec3 Sphere::GetBoundMax(){
    return pos + vec3(r,r,r);
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
    return vec3(minx,miny,minz);
}

vec3 Triangle::GetBoundMax(){
    float maxx = mymax(vertexList[vertexP[0]].x, vertexList[vertexP[1]].x,vertexList[vertexP[2]].x);
    float maxy = mymax(vertexList[vertexP[0]].y, vertexList[vertexP[1]].y,vertexList[vertexP[2]].y);
    float maxz = mymax(vertexList[vertexP[0]].z, vertexList[vertexP[1]].z,vertexList[vertexP[2]].z);
    return vec3(maxx,maxy,maxz);
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
    float a = Area(v1,v2,v3);
    float a3 = Area(v1, p0, v2)/a;
    float a1 = Area(v2, p0, v3)/a;
    float a2 = Area(v1, p0, v3)/a;    

    if ((a1<1) && (a2<1) && (a3<1) && (a1 + a2 + a3)<1.000001){
        hi.hitPos = p0;
        vec3 temp_n = a1*normalList[normalP[0]] + a2*normalList[normalP[1]] + a3*normalList[normalP[2]];
        hi.hitNorm = (ctheta>0)? temp_n :-temp_n;
        // hi.hitNorm = (posn)? temp_n :-temp_n;
        hi.m = materialList[midx];
        hi.rayDepth = ray.depth;
        hi.v = -ray.d;
        hi.t = t;
        return true;       
    }
    return false;
}

Color PointLight::Shading(HitInfo hi) {
    vec3 l = (p - hi.hitPos).normalized();
    vec3 h = (1.0/(hi.v + l).length())*(hi.v + l);
    float diffuse_a = dot(hi.hitNorm,l);
    diffuse_a = (diffuse_a>0)? diffuse_a : 0;
    float specular_a = dot(hi.hitNorm,h.normalized());
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

    vec3 h = (1.0/(hi.v + ldir).length())*(hi.v + ldir);
    // angle between the direction of the light and the direction to the hit
    float angle = dot(-ldir.normalized(), d.normalized());

    if (angle>0 && angle > a2){
        float term = (angle<a1)?(1.0-(a1-angle)/(a1-a2)) : 1.0;
        float diffuse_a = dot(hi.hitNorm,ldir);
        // float diffuse_a = dot(-d.normalized(),ldir);
        diffuse_a = (diffuse_a>0)? diffuse_a : 0;
        float specular_a = dot(hi.hitNorm,h.normalized());
        // float specular_a = dot(-d.normalized(),h.normalized());
        specular_a = (specular_a>0)? pow(specular_a,hi.m.ns) : 0;
        // float fade_f = 1/((p.x-hi.hitPos.x)*(p.x-hi.hitPos.x)+(p.y-hi.hitPos.y)*(p.y-hi.hitPos.y)+(p.z-hi.hitPos.z)*(p.z-hi.hitPos.z)); 
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
    vec3 h = (1/(hi.v + -d).length())*(hi.v + -d);
    float diffuse_a = dot(hi.hitNorm.normalized(),-d.normalized());
    diffuse_a = (diffuse_a>0)? diffuse_a : 0;
    float specular_a = dot(hi.hitNorm.normalized(),h.normalized());
    specular_a = (specular_a>0)? pow(specular_a,hi.m.ns) : 0;
    
    Color c_out = (hi.m.dc * diffuse_a + hi.m.sc * specular_a) * c ;
    return c_out ;
};

vec3 DirectionLight::CalDir(vec3 p0){
    return -d.normalized();
}