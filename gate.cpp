//
// Created by Qianying Ji on 10/10/16.
//

#include "gate.h"
#include <string>
using namespace std;

gate::gate()
{
    gateName = "init";
}

gate::gate(string name, string type, list<string> fanin) {
    gateName = name;
    gateType = type;
    fanInGates = fanin;
}

gate::gate(string name, string type) {
    gateName = name;
    gateType = type;
}

string gate::getGateName(){
    return gateName;
}

void gate::setTypeName(string name)
{
    gateType=name;
}

string gate::getTypeName()
{
    return gateType;
}

list<string>* gate::getFaninVector() {
    return &fanInGates;
}

list<string>* gate::getFanoutVector() {
    return &fanOutGates;
}

void gate::addFanoutGates(string newFanout){
    fanOutGates.push_back(newFanout);
}