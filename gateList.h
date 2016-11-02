//
// Created by Qianying Ji on 10/10/16.
//
#include<string>
#include<list>
#include "gateType.h"
#include "gate.h"

#ifndef EEMINIPROJECT_GATELIST_H
#define EEMINIPROJECT_GATELIST_H

using namespace std;

class gateList {
private:
    list<gate*> gateList;
    list<gateType*> typeList;
public:
    list<gateType*>* getTypeList();
    gateType* searchGateType(string name);
    list<gate*>* getGatelist();
    gate* searchGate(string name);
};

#endif //EEMINIPROJECT_GATELIST_H
