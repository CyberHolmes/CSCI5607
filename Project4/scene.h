#ifndef SCENE_H
#define SCENE_H

#include <stdio.h>
#include <vector>
#include <math.h>
#include "objects.h"

class Scene{
public:
    int nr, nc; //size of the map: number of rows and number of cols
    int cx, cy, cz; //cell size    
    Entity* me;
    std::vector<char> map;
    std::vector<char> mapSaved;
    std::vector<Obj*> objs;
    std::vector<int> map_objIdx; //link cell and index of objs
    std::vector<glm::vec3> lights; //point lights
    std::vector<Zombie*> zoms;
    bool win;
    bool gameOver;    
    
    Scene() : cx(6), cy(8), cz(6), win(false), gameOver(false){
        // entity = new Obj();
        objs.get_allocator().allocate(100);
        map.get_allocator().allocate(200);
        mapSaved.get_allocator().allocate(200);
        map_objIdx.get_allocator().allocate(200);
        lights.get_allocator().allocate(20);
        zoms.get_allocator().allocate(20);
    }
    ~Scene() {
        // printf("delete obj\n");
        for (auto const& obj : objs) {
            delete obj;
        }
        // printf("delete zom\n");
        for (auto const& zom : zoms) {
            delete zom;
        }
        // printf("delete entity\n");
        delete me;
        std::vector<char>().swap(map);
        std::vector<char>().swap(mapSaved);
        std::vector<int>().swap(map_objIdx);
        std::vector<glm::vec3>().swap(lights);
        // printf("end of delete scene\n");
    }
    void ReadMap(const char* fileName);
    // bool LocValid(glm::vec3 new_p, float r);
    bool GetCells(glm::vec3 p, float l, int* cells, int& nCells, int& side);
    bool PointPointVisibilityTest(glm::vec3 p1, glm::vec3 p2); //check cells between two points
    bool OutOfBound(int n) {return (n<0 || n>nr*nc-1);}
    bool IsWall(int n) {return (map[n] == 'W');}
    bool IsDoor(int n) {return ((map[n] >= 'A') && (map[n] <= 'E'));}
    bool IsKey(int n) {return ((map[n] >= 'a') && (map[n] <= 'e'));}
    bool IsGoal(int n) {return (map[n] == 'G');}
    bool FirstPersonUpdate(glm::vec3 p, float dt);
    void ZomUpdate(float dt);
};

#endif