//
// Created by Qianying Ji on 10/10/16.
//

#include <list>
#include "gateList.h"

// search the gateList to see if a gate type already exist
gateType* gateList::searchGateType(string name){
    list<gateType*>::iterator i;
    for(i=typeList.begin();i!=typeList.end();i++)
    {
        if (name==((*i)->getTypeName()))
            return *i;
    }
    return NULL;
}

list<gateType*>* gateList::getTypeList(){
    return &typeList;
}

gate* gateList::searchGate(string name) {
    list<gate*>::iterator i;
    for(i = gateList.begin(); i != gateList.end(); i++){
        if ((*i)->getGateName()==name)
        {
            return *i;
        }
    }
    return NULL;
}

gate* gateList::searchNameNotType(string name, string type)
{
    list<gate*>::iterator j;
    for(j=gateList.begin(); j!=gateList.end();j++)
    {
        if ((*j)->getGateName() == name && (*j)->getTypeName() != type)
        {
            return *j;
        }
    }
    return NULL;
}

list<gate*>* gateList::getGatelist(){
    return &gateList;
}

list<gate*>* gateList::getInputs(){
    return &inputs;
}
list<gate*>* gateList::getOutputs(){
    return &outputs;
}

