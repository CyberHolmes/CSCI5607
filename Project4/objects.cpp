#include "objects.h"

void Entity::AddKey(char k){
    keys[k-97] = 1;
};
bool Entity::HasKey(char k){
    return keys[k-97];
};