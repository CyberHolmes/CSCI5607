#ifndef MAP_H
#define MAP_H

#include <stdio.h>
#include "scene.h"

//Read game map and generate number of objects
void ReadMap(const char* fileName, Scene* scene){
    FILE *fp;
    char line[1024]; //assume no line is longer than 1024 characters
    int nr,nc; //number of rows and number of columns

    fp = fopen(fileName,"r");
    //file open error handling
    if (!fp){
        printf("Cannot open map file: %s\n", fileName);
        exit(1);
    }
    // assume the first line is the size of the map
    fgets(line, 1024, fp);
    sscanf(line, "%d %d", &nr, &nc);
    scene->nc = nc;
    scene->nr = nr;
    int n = 0;
    while(1){
        char c = fgetc(fp);        
        if (c == EOF) break;
        scene->map.emplace_back(c);
        scene->mapSaved.emplace_back(c);
    }
}

#endif