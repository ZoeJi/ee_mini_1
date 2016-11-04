//
// Created by Qianying Ji on 11/4/16.
//
#include <string>
#ifndef EEMINIPROJECT_CELL_H
#define EEMINIPROJECT_CELL_H
using namespace std;

class cell {
private:
    string cellName;
    double Cin;

public:
    double TauIn[7];
    double cL[7];
    double delays[7][7];
    double slews[7][7];
    cell();
    cell(string name);
    void setCin(double cin);
    double* getTauInArray();
    string getCellName();
};


#endif //EEMINIPROJECT_CELL_H
