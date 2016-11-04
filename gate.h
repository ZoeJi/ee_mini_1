//
// Created by Qianying Ji on 10/10/16.
//
#include<string>
#include <list>
#include <vector>
#ifndef EEMINIPROJECT_GATE_H
#define EEMINIPROJECT_GATE_H
using namespace std;


class gate {
private:
    string gateName;
    string gateType;
    vector<gate*> fanInGates;
    vector<gate*> fanOutGates;

public:
    gate();
    gate(string name);
    gate(string name, string type);
//    gate(string name, string type, list<string> fanin);
    string getGateName();
    void setTypeName(string name);
    string getTypeName();

    vector<gate*>* getFainPtrs();
    vector<gate*>* getFanoutPtrs();
};

#endif //EEMINIPROJECT_GATE_H
