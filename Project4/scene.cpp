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
    //floor
    Obj* tmp = new Wall(0,4,glm::vec3((nc+1)*cx/2.0,-cy/2.0,(nr+1)*cz/2.0),glm::vec3((nc+1)*cx,1,(nr+1)*cz));
    objs.emplace_back(tmp); obj_cnt++;
    
    //ceiling
    tmp = new Wall(0,3,glm::vec3((nc+1)*cx/2.0,0.5*cy,(nr+1)*cz/2.0),glm::vec3((nc+1)*cx,1,(nr+1)*cz));
    objs.emplace_back(tmp); obj_cnt++;

    // tmp = new Wall(5,4,glm::vec3(cx*1.5,8,cz*0.5),glm::vec3(1,1,1));
    // objs.emplace_back(tmp); obj_cnt++;
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
    tmp = new Wall(0,1,glm::vec3((nc+1)*cx/2.0,yshift,cz*marginf),glm::vec3((nc+1)*cx,cy,1));
    objs.emplace_back(tmp); obj_cnt++;
    tmp = new Wall(0,1,glm::vec3(cx*marginf,yshift,(nr+1)*cz/2.0),glm::vec3(1,cy,(nr+1)*cz));
    objs.emplace_back(tmp); obj_cnt++;
    // tmp->show = true;
    tmp = new Wall(0,1,glm::vec3((nc+1)*cx/2.0,yshift,(nr+1)*cz-cz*marginf),glm::vec3((nc+1)*cx,cy,1));
    objs.emplace_back(tmp); obj_cnt++;
    tmp = new Wall(0,1,glm::vec3((nc+1)*cx-cx*marginf,yshift,(nr+1)*cz/2.0),glm::vec3(1,cy,(nr+1)*cz));
    objs.emplace_back(tmp); obj_cnt++;
    int n = 0;
    //5 distinct colors for doors and matching keys
    //red, blue, green, yellow, purple
    glm::vec3 colors[5] = {glm::vec3(1.0,0.8,0.0), glm::vec3(0.9,0.1,0.2)
        ,glm::vec3(0.2,0.8,0.2),glm::vec3(0.1,0.6,1.0),glm::vec3(0.5,0.0,0.5)};
    float doorScale = 0.6;
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
                    tmp = new Wall(0,1,curPos,glm::vec3(cx,cy,cz));
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
                    if (map[n-1]=='W'){
                        // tmp = new Obj(1,-1,curPos+glm::vec3(0,-3.7,0),glm::vec3(cx*doorScale,2.2,cz*doorScale),glm::vec3(0,1,0), 0.0, colors[c-65],
                        // curPos+glm::vec3(float(cx/2.0),-3.7,float(cz/2.0)), glm::vec3(0,1,0), 3.14/2);      
                        tmp = new Door(1,-1,curPos+glm::vec3(0,0,0),glm::vec3(cx*doorScale,2.2,cz*doorScale),glm::vec3(0,1,0), 0.0, colors[c-65],
                        curPos+glm::vec3(float(cx/2.0),0,float(cz/2.0)), glm::vec3(0,1,0), 3.14/2);                        
                    } else {
                        // tmp = new Obj(1,-1,curPos+glm::vec3(0,-3.7,0),glm::vec3(cx*doorScale,2.2,cz*doorScale),glm::vec3(0,1,0), 1.57, colors[c-65],
                        // curPos+glm::vec3(float(cx/2.0),-3.7,float(cz/2.0)), glm::vec3(0,1,0), 3.14);
                        /*Door(int modelID_, int texID_, glm::vec3 pos_, glm::vec3 scale_, glm::vec3 color_, glm::vec3 end_pos_, glm::vec3 end_rotAxis_, float end_rotRad_) :
                            Obj(modelID_, texID_, pos_, scale_, color_), endStateValid(false), end_pos(glm::vec3(0,0,0)), end_rotAxis(glm::vec3(0,1,0)), end_rotRad(0) 
                            {d_pos = end_pos - pos; d_rad = end_rotRad - rotRad;}*/
                        tmp = new Door(1,-1,curPos+glm::vec3(0,0,0),glm::vec3(cx*doorScale,2.2,cz*doorScale),glm::vec3(0,1,0), 1.57, colors[c-65],
                        curPos+glm::vec3(float(cx/2.0),0,float(cz/2.0)), glm::vec3(0,1,0), 3.14);              
                    }                    
                    objs.emplace_back(tmp);
                    map_objIdx.emplace_back(obj_cnt);
                    obj_cnt++;
                    break;
                case 'G': //goal
                    tmp = new GenericObj(3,-1,curPos+glm::vec3(-1,-3.5,1),glm::vec3(1.5,1.5,1.5),glm::vec3(0,1,0), 0.0, glm::vec3(1,0,0),2);
                    objs.emplace_back(tmp);
                    map_objIdx.emplace_back(obj_cnt);
                    obj_cnt++;
                    break;
                case 'L': //light
                    lights.emplace_back(glm::vec3(curPos.x,0.32*cy,curPos.z));
                    tmp = new Light(5,-1,curPos+glm::vec3(0,0.43*cy,0),glm::vec3(0.5,0.5,0.5)); //light bulb
                    objs.emplace_back(tmp);
                    map_objIdx.emplace_back(-1);
                    obj_cnt++;
                    break;
                case 'a': //keys
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                    tmp = new GenericObj(4,-1,curPos+glm::vec3(0,-1,0),glm::vec3(1,1,1),glm::vec3(0,1,1), 3.14/2.0, colors[c-97],1);
                    objs.emplace_back(tmp);
                    map_objIdx.emplace_back(obj_cnt);
                    obj_cnt++;
                    break;
                case 'S': //start position
                    me = new Entity(2,-1,curPos+glm::vec3(0,-4,0),glm::vec3(1,1,1),glm::vec3(0,1,0),3.14*3/2.0,glm::vec3(0.1,0.55,0.1),1);
                    map_objIdx.emplace_back(-1);
                    break;
                case 'Z': {//start position
                    Zombie* tmp2 = new Zombie(8,-1,curPos+glm::vec3(0,-3.5,0),glm::vec3(0.5,0.5,0.5),glm::vec3(0,1,0),3.14*1.5,
                        glm::vec3(rand()/(float)RAND_MAX,float(rand()/(float)RAND_MAX),float(rand()/(float)RAND_MAX)),1);
                    zoms.emplace_back(tmp2);
                    map_objIdx.emplace_back(-1);
                    break;
                }
                default:
                    map_objIdx.emplace_back(-1);
                    break;
            }
            n++;
            // printf("n=%d, r=%d, c=%d, x=%.0f, z=%.0f, m=%c\n",n-1,curR,curC,curPos.x,curPos.z,c);
        }
    }
    // assert(false);
}

bool Scene::GetCells(glm::vec3 p, float l, int* cells, int& nCells, int& side){
    if ((p.z-l-cz/2)<0 || (p.z+l)>(nr+0.5)*cz) {side = 0; return false;};
    if ((p.x-l-cx/2)<0  || (p.x+l)>(nc+0.5)*cx) {side = 1; return false;};
    int r = (round(p.z/cz)<1)?1:round(p.z/cz), c = (round(p.x/cx)<1)?1:round(p.x/cx);
    int n = (nr-r)*nc + nc-c;
    cells[nCells] = n;
    nCells++;
    bool b0 = (p.z-me->GetRadius())<(r-0.5)*cz;
    bool b1 = (p.x-me->GetRadius())<(c-0.5)*cx;
    bool b2 = (p.z+me->GetRadius())>(r+0.5)*cz;
    bool b3 = (p.x+me->GetRadius())>(c+0.5)*cx;
    if (b0) {
        cells[nCells] = n+nc;
        nCells++; side=0;
    }
    if (b1) {
        cells[nCells] = n+1;
        nCells++; side=1;
    }
    if (b2) {
        cells[nCells] = n-nc;
        nCells++; side=0;
    }
    if (b3) {
        cells[nCells] = n-1;
        nCells++; side=1;
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

bool Scene::PointPointVisibilityTest(glm::vec3 p1, glm::vec3 p2){
    int r1 = (round(p1.z/cz)<1)?1:round(p1.z/cz), c1 = (round(p1.x/cx)<1)?1:round(p1.x/cx);
    int r2 = (round(p2.z/cz)<1)?1:round(p2.z/cz), c2 = (round(p2.x/cx)<1)?1:round(p2.x/cx);
    // printf("r1=%d,r2=%d,c1=%d,c2=%d\n",r1,r2,c1,c2);
    if (abs(r1-r2)<2 && abs(c1-c2)<2) return true;
    int n1 = (nr-r1)*nc + nc-c1;
    int n2 = (nr-r2)*nc + nc-c2;
    // printf("n1=%d,n2=%d\n",n1,n2);
    int start_r, end_r, start_c, end_c; 
    if (r1<r2) {start_r=r1; end_r = r2;}
    else {start_r=r2; end_r = r1;}
    if (c1<c2) {start_c=c1; end_c = c2;}
    else {start_c=c2; end_c = c1;}
    if (r1==r2){ //same row
        for (int i=start_c; i<end_c; i++){
            int n = (nr-r1)*nc + nc-i;
            // printf("r1=r2=%d,n=%d\n",r1,n);
            if (IsDoor(n) || IsWall(n)) return false;
        }
    }
    if (c1==c2){ //same col
        for (int i=start_r; i<end_r; i++){
            int n = (nr-i)*nc + nc-c1;
            // printf("nr=%d,nc=%d,i=%d,c1=c2=%d,n=%d\n",nr,nc,i,c1,n);
            if (IsDoor(n) || IsWall(n)) return false;
        }
    }
    float xmin,xmax,zmin,zmax;
    if (p1.z<p2.z){zmin=p1.z;xmin=p1.x;zmax=p2.z;xmax=p2.x;}
    else {zmin=p2.z;xmin=p2.x;zmax=p1.z;xmax=p1.x;}
    // if (p1.x<p2.x){xmin=p1.x;xmax=p2.x;}
    // else {xmin=p2.x;xmax=p1.x;}
    for (int cur_r=start_r; cur_r<=end_r; cur_r++){
        for (int cur_c=start_c; cur_c<=end_c; cur_c++){
            float a = 0;
            float cell_x_min = (float(cur_c)-0.5+a)*float(cx);
            float cell_x_max = (float(cur_c)+0.5+a)*float(cx);
            float cell_z_min = (float(cur_r)-0.5+a)*float(cz);
            float cell_z_max = (float(cur_r)+0.5+a)*float(cz);
            int n = (nr-cur_r)*nc + nc-cur_c;
            // printf("r=%d, c=%d, n=%d, m=%c\n",cur_r,cur_c,n,map[n]);
            // printf("xmin=%.1f,xmax=%.1f,zmin=%.1f,zmax=%.1f\n",p1.x,p2.x,p1.z,p2.z);
            // printf("xmin=%.1f,xmax=%.1f,zmin=%.1f,zmax=%.1f\n",xmin,xmax,zmin,zmax);
            // printf("cell_x_min=%.1f,cell_x_max=%.1f,cell_z_min=%.1f,cell_z_max=%.1f\n",cell_x_min,cell_x_max,cell_z_min,cell_z_max);
            //((y2 - y1) * xcorner + (x1 - x2) * ycorner + (x2 * y1 - x1 * y2)) >= 0
            // bool corner1leftofline = (((zmax - zmin) * cell_x_min + (xmax - xmin) * cell_z_min  + (xmax * zmin - xmin * zmax)) >= 0);
            // bool corner2leftofline = (((zmax - zmin) * cell_x_min + (xmax - xmin) * cell_z_max  + (xmax * zmin - xmin * zmax)) >= 0);
            // bool corner3leftofline = (((zmax - zmin) * cell_x_max + (xmax - xmin) * cell_z_min  + (xmax * zmin - xmin * zmax)) >= 0);
            // bool corner4leftofline = (((zmax - zmin) * cell_x_max + (xmax - xmin) * cell_z_max  + (xmax * zmin - xmin * zmax)) >= 0);
            // glm::vec2 p_corner=glm::vec2(cell_x_min,cell_z_min), p11 = glm::vec2(p1.x,p1.z), p12 = glm::vec2(p2.x,p2.z);
            // glm::vec2 l1 = p_corner-p11, l2=p_corner-p12;         
            // bool corner1leftofline = ((l1.x*l2.y-l1.y*l2.x) >=0);
            // p_corner=glm::vec2(cell_x_min,cell_z_max);
            // l1 = p_corner-p11, l2=p_corner-p12;    
            // bool corner2leftofline = ((l1.x*l2.y-l1.y*l2.x) >=0);
            //  p_corner=glm::vec2(cell_x_max,cell_z_min);
            // l1 = p_corner-p11, l2=p_corner-p12;    
            // bool corner3leftofline = ((l1.x*l2.y-l1.y*l2.x) >=0);
            // p_corner=glm::vec2(cell_x_max,cell_z_max);
            // l1 = p_corner-p11, l2=p_corner-p12;    
            // bool corner4leftofline = ((l1.x*l2.y-l1.y*l2.x) >=0);
            bool corner1leftofline = (((cell_x_min - xmin) * (zmax -zmin) -(cell_z_min-zmin)*(xmax-xmin)) >= 0);
            bool corner2leftofline = (((cell_x_max - xmin) * (zmax -zmin) -(cell_z_max-zmin)*(xmax-xmin)) >=  0);
            bool corner3leftofline = (((cell_x_min - xmin) * (zmax -zmin) -(cell_z_max-zmin)*(xmax-xmin)) >=  0);
            bool corner4leftofline = (((cell_x_max - xmin) * (zmax -zmin) -(cell_z_min-zmin)*(xmax-xmin)) >=  0);
            // printf("1=%d, 2=%d, 3=%d, 4=%d\n",corner1leftofline,corner2leftofline,corner3leftofline,corner4leftofline);
            // printf("------------------------\n");
            if ((corner1leftofline && corner4leftofline && corner3leftofline && corner2leftofline) ||
                ((!corner1leftofline) && (!corner4leftofline) && (!corner3leftofline) && (!corner2leftofline))) 
                continue;
            else {            
            // printf("cur_r=%d,cur_c=%d,n=%d\n",cur_r,cur_c,n);
            if (IsDoor(n) || IsWall(n)) return false;
            }
            
        }        
    }
    // assert(false);
    return true;
}

bool Scene::FirstPersonUpdate(glm::vec3 p, float dt){ //only update scene if p is valid
    int nCells=0;
    int cells[5] = {0}; //no more than 5 cells can be occupied by an entity at a given time
    int side;
    if (!GetCells(p,me->GetRadius()/2,cells,nCells,side)) return false;
    
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
                //check which direction I am approaching the door
                // glm::vec3 me_pos = me->GetPos();
                // int r1 = (round(me_pos.z/cz)<1)?1:round(me_pos.z/cz), c1 = (round(me_pos.x/cx)<1)?1:round(me_pos.x/cx);
                // int me_n = (nr-r1)*nc + nc-c1;
                int obj_idx = map_objIdx[n];
                // if (me_n < n) { //approaching from top or left
                //     Door* temp_door = objs[obj_idx];
                // }
                
                if (!objs[obj_idx]->Animate(dt)) map[n] = '0'; //set map grid to 0 if animation is done
                // objs[obj_idx]->rotRad = objs[obj_idx]->end_rotRad;
                // objs[obj_idx]->pos = objs[obj_idx]->end_pos;
                // objs[obj_idx]->rotRad = (fabs(objs[obj_idx]->rotRad - objs[obj_idx]->end_rotRad)<0.001)? objs[obj_idx]->end_rotRad : objs[obj_idx]->rotRad;
                // objs[obj_idx]->pos = (glm::distance(objs[obj_idx]->pos, objs[obj_idx]->end_pos)<0.01)? objs[obj_idx]->end_pos : objs[obj_idx]->pos;
                // if ((fabs(objs[obj_idx]->rotRad - objs[obj_idx]->end_rotRad)<0.001) || (glm::distance(objs[obj_idx]->pos, objs[obj_idx]->end_pos)<0.01)){
                //     objs[obj_idx]->rotRad = objs[obj_idx]->end_rotRad;
                //     objs[obj_idx]->pos = objs[obj_idx]->end_pos;
                //     map[n] = 'O'; //open door
                // } else {
                //     objs[obj_idx]->rotRad +=3.14/2.0*dt*1;                
                //     objs[obj_idx]->pos = objs[obj_idx]->pos + glm::vec3(float(cx/2.0),0,float(cz/2.0)) * (dt*1);                    
                // }                
            } else {
                return false;
            }
        }
        if (IsKey(n)){
            int obj_idx = map_objIdx[n];
            if (glm::distance(p,objs[obj_idx]->GetPos())<2){
                char k = map[n];
                me->AddKey(k);            
                objs[obj_idx]->SetVisibility(false);
                map[n] = '0';
            }
        }
        if (IsGoal(n)){
            int obj_idx = map_objIdx[n];
            glm::vec3 p1 = objs[obj_idx]->GetPos();
            glm::vec3 p2 = me->GetPos();
            
            float d1 = (p1.x-p2.x)*(p1.x-p2.x) + (p1.z-p2.z)*(p1.z-p2.z);
            if (d1<4)
                win = true;
        }
    }
    return true;   
}

void Scene::ZomUpdate(float dt){
    for (auto zom : zoms){
        // printf("inside zomupdate\n");
        glm::vec3 zom_p = zom->GetPos();
        glm::vec3 me_p = me->GetPos();
        float d1 = (zom_p.x-me_p.x)*(zom_p.x-me_p.x) + (zom_p.z-me_p.z)*(zom_p.z-me_p.z);
        float d2 = (zom->GetRadius()+me->GetRadius())*(zom->GetRadius()+me->GetRadius());
        // If zom touched first person
        if (d1<d2) gameOver = true;
        //Check if I can see the first person
        if (PointPointVisibilityTest(zom_p,me_p)){
            zom->SetDir(glm::normalize(glm::vec3(me_p.x-zom_p.x,0,me_p.z-zom_p.z)));
            // return;
        }
        // printf("visible=%d\n",PointPointVisibilityTest(zom_p,me_p));
        glm::vec3 new_p = zom->GetPos() + zom->GetDir() * (zom->GetVel()*dt*1);
        int nCells=0;
        int cells[5] = {0}; //no more than 5 cells can be occupied by an entity at a given time
        int side = 0;
        if (!GetCells(new_p,0.5,cells,nCells,side)){
            // zom->SetDir(zom->GetDir()+glm::vec3(0.1,0.55,0.1));
            new_p = zom->GetPos() - zom->GetDir() * float(0.1); //(zom->GetVel()*dt*2);
            zom->SetPos(new_p);
            glm::vec3 new_dir = (side==0)? zom->GetDir() *glm::vec3(1,1,-1) : zom->GetDir() *glm::vec3(-1,1,1);
            new_dir = glm::normalize(new_dir);            
            zom->SetDir(new_dir);
        } else{
            for (int i=0; i<nCells; i++){
                int n = cells[i]; //map index
                if (IsDoor(n) || IsWall(n) || IsGoal(n)){
                    new_p = zom->GetPos() - zom->GetDir() * float(0.1); //(zom->GetVel()*dt*2);
                    zom->SetPos(new_p);
                    glm::vec3 new_dir = (side==0)? zom->GetDir() *glm::vec3(1,1,-1) : zom->GetDir() *glm::vec3(-1,1,1);
                    new_dir = glm::normalize(new_dir);
                    // new_p = zom->GetPos() + new_dir * float(0.1); //(zom->GetVel()*dt*2);
                    // zom->SetPos(new_p);
                    zom->SetDir(new_dir);
                }
            }
        }
        zom->SetPos(new_p);
    }
}