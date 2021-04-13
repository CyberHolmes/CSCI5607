#include "scene.h"
//Read game map and generate number of objects
void Scene::ReadMap(const char* fileName){
    FILE *fp;
    char line[1024]; //assume no line is longer than 1024 characters

    fp = fopen(fileName,"r");
    //file open error handling
    if (!fp){
        printf("Cannot open map file: %s\n", fileName);
        exit(1);
    }
    // assume the first line is the size of the map
    fgets(line, 1024, fp);
    sscanf(line, "%d %d", &nr, &nc);
    //0-cube, 1-door, 2-girl, 3-treasure chest, 4-key, 5-sphere, 6-teapot, 7-knot
    //floor
    int obj_cnt = 0;
    float yshift = 0.0;
    Obj* tmp = new Obj(0,4,glm::vec3((nc+1)*cx/2.0,-cy/2.0,(nr+1)*cz/2.0),glm::vec3((nc+1)*cx,1,(nr+1)*cz));
    objs.emplace_back(tmp); obj_cnt++;
    //walls
    // for (int i=1; i<=nc; i++){
    //     tmp = new Obj(0,1,glm::vec3(i*cx,0,cz*0.5),glm::vec3(cx,cy,1));
    //     objs.emplace_back(tmp); obj_cnt++;
    //     tmp = new Obj(0,1,glm::vec3(i*cx,0,nc*cz+cz),glm::vec3(cx,cy,1));
    //     objs.emplace_back(tmp); obj_cnt++;
    // }
    // for (int i=1; i<=nr; i++){
    //     tmp = new Obj(0,1,glm::vec3(cx*0.5,0,i*cz),glm::vec3(1,cy,cz));
    //     objs.emplace_back(tmp); obj_cnt++;
    //     tmp = new Obj(0,1,glm::vec3(nr*cx+cx,0,i*cz),glm::vec3(1,cy,cz));
    //     objs.emplace_back(tmp); obj_cnt++;
    // }
    float marginf = 0.41;
    tmp = new Obj(0,1,glm::vec3((nc+1)*cx/2.0,yshift,cz*marginf),glm::vec3((nc+1)*cx,cy,1));
    objs.emplace_back(tmp); obj_cnt++;
    tmp = new Obj(0,1,glm::vec3(cx*marginf,yshift,(nr+1)*cz/2.0),glm::vec3(1,cy,(nr+1)*cz));
    objs.emplace_back(tmp); obj_cnt++;
    // tmp->show = true;
    tmp = new Obj(0,1,glm::vec3((nc+1)*cx/2.0,yshift,(nr+1)*cz-cz*marginf),glm::vec3((nc+1)*cx,cy,1));
    objs.emplace_back(tmp); obj_cnt++;
    tmp = new Obj(0,1,glm::vec3((nc+1)*cx-cx*marginf,yshift,(nr+1)*cz/2.0),glm::vec3(1,cy,(nr+1)*cz));
    objs.emplace_back(tmp); obj_cnt++;
    int n = 0;
    //5 distinct colors for doors and matching keys
    //red, blue, green, yellow, purple
    glm::vec3 colors[5] = {glm::vec3(1.0,0.8,0.0), glm::vec3(0.9,0.1,0.2)
        ,glm::vec3(0.2,0.8,0.2),glm::vec3(0.1,0.6,1.0),glm::vec3(0.5,0.0,0.5)};
    float doorScale = 0.64;
    while(1){
        int curR = nr - n/nc;
        int curC = nc - n%nc;
        glm::vec3 curPos =glm::vec3(float(curC*cx),0,float(curR*cz));
        char c = fgetc(fp);        
        if (c == EOF) break;
        if (c != '\n' && c!= ' '){
            // printf("n/nr=%d,n mod nr=%d, c=%c\n",n/nr,n%nc,c);
            map.emplace_back(c);            
            mapSaved.emplace_back(c);
            switch (c){ //0-cube, 1-door, 2-girl, 3-treasure chest, 4-key, 5-sphere, 6-teapot, 7-knot 
            //0-wood, 1-brick, 2-brick1, 3-laminate1, 4-tile1       
                case 'W': //wall
                    tmp = new Obj(0,1,curPos,glm::vec3(cx,cy,cz));
                    // printf("curPos=%f,%f,%f\n",curPos.x,curPos.y,curPos.z);
                    objs.emplace_back(tmp);
                    map_objIdx.emplace_back(obj_cnt);
                    obj_cnt++;
                    break;
                case 'A': //doors
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                    if (map[n-1]=='0'){
                        tmp = new Obj(1,-1,curPos+glm::vec3(0,-3.7,0),glm::vec3(cx*doorScale,2.5,cz*doorScale),glm::vec3(0,1,0), 1.57, colors[c-65]);
                    } else {
                        tmp = new Obj(1,-1,curPos+glm::vec3(0,-3.7,0),glm::vec3(cx*doorScale,2.5,cz*doorScale),glm::vec3(0,1,0), 0.0, colors[c-65]);
                    }
                    objs.emplace_back(tmp);
                    map_objIdx.emplace_back(obj_cnt);
                    obj_cnt++;
                    break;
                case 'G': //goal
                    tmp = new Obj(3,-1,curPos+glm::vec3(-1,-3.5,1),glm::vec3(1.5,1.5,1.5),glm::vec3(0,1,0), 0.0, glm::vec3(1,0,0));
                    objs.emplace_back(tmp);
                    map_objIdx.emplace_back(obj_cnt);
                    obj_cnt++;
                    break;
                case 'L': //light
                    lights.emplace_back(glm::vec3(curPos.x,cy-2,curPos.z));
                    map_objIdx.emplace_back(-1);
                    break;
                case 'a': //keys
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                    tmp = new Obj(4,-1,curPos+glm::vec3(0,-1,0),glm::vec3(1.5,1.5,1.5),glm::vec3(0,1,1), 3.14/2.0, colors[c-97]);
                    objs.emplace_back(tmp);
                    map_objIdx.emplace_back(obj_cnt);
                    obj_cnt++;
                    break;
                case 'S': //start position
                    me = new Entity(2,-1,curPos,glm::vec3(1,1,1),glm::vec3(0,1,0),3.14*3/2.0,glm::vec3(0.1,0.55,0.1));
                    // for (int i=0; i<5; i++){
                    //     printf("key %d = %d\n",i,me->keys[i]);
                    // }
                    map_objIdx.emplace_back(-1);
                    break;
                default :
                    map_objIdx.emplace_back(-1);
                    break;
            }
            n++;
        }
    }
}

bool Scene::GetCells(glm::vec3 p, float l, int* cells, int& nCells){
    if ((p.z-l-cz/2)<0 || (p.x-l-cx/2)<0 || (p.z+l)>(nr+0.5)*cz || (p.x+l)>(nc+0.5)*cx) return false;
    int r = (round(p.z/cz)<1)?1:round(p.z/cz), c = (round(p.x/cx)<1)?1:round(p.x/cx);
    int n = (nr-r)*nc + nc-c;
    cells[nCells] = n;
    nCells++;
    bool b0 = (p.z-me->GetSize()/2.0)<(r-0.5)*cz;
    bool b1 = (p.x-me->GetSize()/2.0)<(c-0.5)*cx;
    bool b2 = (p.z+me->GetSize()/2.0)>(r+0.5)*cz;
    bool b3 = (p.x+me->GetSize()/2.0)>(c+0.5)*cx;
    if (b0) {
        cells[nCells] = n+nc;
        nCells++;
    }
    if (b1) {
        cells[nCells] = n+1;
        nCells++;
    }
    if (b2) {
        cells[nCells] = n-nc;
        nCells++;
    }
    if (b3) {
        cells[nCells] = n-1;
        nCells++;
    }
    if (b0 && b1) {
        cells[nCells] = n+nc+1;
        nCells++;
    }
    if (b2 && b3) {
        cells[nCells] = n-nc-1;
        nCells++;
    }
    if (b0 && b3) {
        cells[nCells] = n+nc-1;
        nCells++;
    }
    if (b2 && b1) {
        cells[nCells] = n-nc+1;
        nCells++;
    }
    return true;
}

bool Scene::Update(glm::vec3 p){ //only update scene if p is valid
    int nCells=0;
    int cells[5]; //no more than 5 cells can be occupied by an entity at a given time
    if (!GetCells(p,me->GetSize()/2,cells,nCells)) return false;
    for (int i=0; i<nCells; i++){
        int n = cells[i]; //map index
        if (IsWall(n)){
            return false;
        }
        if (IsDoor(n)){
            //check if I have the right key
            char k = map[n] + 32;
            if (me->HasKey(k)){
                //open the door and update the map to make it free to pass
                int obj_idx = map_objIdx[n];
                objs[obj_idx]->rotRad +=3.14/2.0;
                objs[obj_idx]->pos += glm::vec3(cx/2.0,0,cz/2.0);
                map[n] = 'O'; //open door
            } else {
                return false;
            }
        }
        if (IsKey(n)){
            char k = map[n];
            me->AddKey(k);
            int obj_idx = map_objIdx[n];
            objs[obj_idx]->show = false;
            map[n] = '0';
        }
        if (IsGoal(n)){
            win = true;
        }
    }
    return true;   
}