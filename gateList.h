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

    /* phase 2 */
    list<gate*> inputs;
    list<gate*> outputs;

public:
    list<gateType*>* getTypeList();
    gateType* searchGateType(string name);
    list<gate*>* getGatelist();
    gate* searchNameNotType(string name, string type);
    gate* searchGate(string name);

    /* phase 2 */
    list<gate*>* getInputs();
    list<gate*>* getOutputs();
};

#endif //EEMINIPROJECT_GATELIST_H
