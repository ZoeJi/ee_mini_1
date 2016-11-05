//
// Created by Qianying Ji on 11/4/16.
//

#include "cell.h"

cell::cell(){
    cellName = "init";
}
cell::cell(string name){
    cellName = name;
}

void cell::setCin(double cin){
    Cin = cin;
}

double* cell::getTauInArray(){
    return (double *) &TauIn;
}

string cell::getCellName(){
    return cellName;
}

string cell::getCellType(){
    return cellType;
}

void cell::setCellType(string type) {
    cellType = type;
}