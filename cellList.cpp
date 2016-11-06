//
// Created by Qianying Ji on 11/4/16.
//

#include "cellList.h"

cell* cellList::searchCellByType(string type){
    list<cell*>::iterator i;
    for(i = cellList.begin(); i != cellList.end(); i++){
        if ((*i)->getCellType() == type)
        {
            return *i;
        }
    }
    return NULL;
}

list<cell*>* cellList::getCellList(){
    return &cellList;
}