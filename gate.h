//
// Created by Qianying Ji on 10/10/16.
//
#include<string>
#include <list>
#ifndef EEMINIPROJECT_GATE_H
#define EEMINIPROJECT_GATE_H
using namespace std;


class gate {
private:
    string gateName;
    string gateType;
//    vector<gate*>* fanInGates;
//    vector<gate*>* fanOutGates;
    list<string> fanInGates;
    list<string> fanOutGates;

public:
    gate();
    gate(string name, string type);
    gate(string name, string type, list<string> fanin);
    string getGateName();
    void setTypeName(string name);
    string getTypeName();

    list<string>* getFaninVector();
    list<string>* getFanoutVector();
    void addFanoutGates(string newFanout);
};

#endif //EEMINIPROJECT_GATE_H
