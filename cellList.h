//
// Created by Qianying Ji on 11/4/16.
//
#include "cell.h"
#include <list>
#ifndef EEMINIPROJECT_CELLLIST_H
#define EEMINIPROJECT_CELLLIST_H


class cellList {
private:
    list<cell*> cellList;

public:
    cell* searchCellByType(string type);
    list<cell*>* getCellList();
};


#endif //EEMINIPROJECT_CELLLIST_H
