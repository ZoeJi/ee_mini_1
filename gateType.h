//
// Created by Qianying Ji on 10/10/16.
//
#include<string>
#ifndef EEMINIPROJECT_GATETYPE_H
#define EEMINIPROJECT_GATETYPE_H
using namespace std;

class gateType {
private:
    string typeName;
    int typeCount;
public:
    gateType(string name);
    void setTypeName(string name);
    string getTypeName();
    void typeCountIncr();
    int getTypeCount();
};

#endif //EEMINIPROJECT_GATETYPE_H
