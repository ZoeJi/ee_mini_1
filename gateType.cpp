//
// Created by Qianying Ji on 10/10/16.
//

#include <list>
#include "gateType.h"
gateType::gateType(string name) {
    typeName = name;
    typeCount = 0;
}

void gateType::setTypeName(string name){
    typeName = name;
}
string gateType::getTypeName(){
    return typeName;
}
void gateType::typeCountIncr(){
    typeCount++;
}
int gateType::getTypeCount(){
    return typeCount;
}

