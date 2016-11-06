#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <set>
#include "gate.h"
#include "gateList.h"
#include "cell.h"
#include <list>
#include "gateType.h"
#include "cellList.h"

using namespace std;

/*
 * getIOname is used to extract the name of primary input or output
 */
void write_result_ckt(int inputCount, int outputCount, string fileName, gateList *myGateList);
string getIOname(string str);
string readGateType(string str);
string readGateName(string str);
vector<string> readGateFanin(string str);
string discardSpace(string str);
string readCapacitance(string str);
string readTauIn(string str);
string readValue(string str);
string readCellType(string str);
inline string& ltrim(string& s);
inline string& rtrim(string& s);
inline string& trim(string& s);
void write_result_delays(cellList *mycellList);
void write_result_slews(cellList *mycellList);
void traversCkt(gateList *myGateList, cellList *myCellList);
void calculateCout(gateList *myGateList, cellList *myCellList);
double calculateDelaysOrSlews(cell *thisCell, double ipslew, double C_load, int faninNum, int delay_or_slew);


int main(int argc, char *argv[]) {

    string aline;
    string gateName;
    string thisGateType;
    string cellName;
    gateType* newGateType;
    gate* newGate;
    gate* aFaninGate;;
    gate* existGate;
    gate* inputGate;
    gateList myGateList;
    vector<string> faninVect;

    cell* newCell;
    cellList mycellList;

    int inputCount = 0, outputCount = 0;

    if ( argc < 2 ) // argc should be more than 3 for correct execution
        cout<<"usage: "<< argv[0] <<" <read command> <filename>\n <read command> options: read_ckt, read_nldm\n";
    else {
        if(strncmp(argv[1],"read_ckt", 9) == 0){
            ifstream fileInput ( argv[2] );
            if ( !fileInput.is_open() )
                cout<<"Could not open file\n";
            else {
                while(!fileInput.eof()) {
                    getline(fileInput,aline);
                    if(isalpha(aline[0]) || (aline[0]>='0' && aline[0]<= '9')) {
                        if(!aline.empty()) {
                            if(aline[0]!='#') {
                                // inputs
                                if(aline.substr(0,5)=="INPUT") {
                                    inputCount++;
                                    gateName = getIOname(aline);
                                    newGate = new gate(gateName, "INP");
                                    myGateList.getGatelist()->push_back(newGate);
                                    myGateList.getInputs()->push_back(newGate);
                                }
                                // outputs
                                else if(aline.substr(0,6)=="OUTPUT") {
                                    outputCount++;
                                    gateName = getIOname(aline);
                                    newGate = new gate(gateName, "OUTP");

                                    inputGate = myGateList.searchGate(gateName);

                                    /*check if exist input directly connect to output*/
                                    if(inputGate != NULL)
                                    {
                                        newGate->getFainPtrs()->push_back(inputGate);
                                        newGate->incrFaninNumber();
                                        inputGate->getFanoutPtrs()->push_back(newGate);
                                        inputGate->incrFanoutNumber();
                                    }

                                    myGateList.getGatelist()->push_back(newGate);
                                    myGateList.getOutputs()->push_back(newGate);

                                }
                                // other logical gates
                                else {
                                    // gate type count
                                    thisGateType = readGateType(aline);
                                    newGateType = myGateList.searchGateType(thisGateType);
                                    if(newGateType == NULL) {
                                        myGateList.getTypeList()->push_back(new gateType(thisGateType));
                                        newGateType = myGateList.searchGateType(thisGateType);
                                        newGateType->typeCountIncr();
                                    }
                                    else {
                                        newGateType->typeCountIncr();
                                    }

                                    // gate fanout, fanin
                                    gateName = readGateName(aline);
                                    thisGateType = readGateType(aline);
                                    faninVect = readGateFanin(aline);

                                    /*check if this gate already meet before*/
                                    newGate = myGateList.searchGate(gateName);
                                    /*if not met this gate before*/
                                    if(newGate == NULL){
                                        newGate = new gate(gateName, thisGateType);
                                        myGateList.getGatelist()->push_back(newGate);
                                    }
                                    /*if met this gate before*/
                                    else {
                                        if(newGate->getTypeName() == "OUTP"){
                                            existGate = newGate;
                                            newGate = new gate(gateName, thisGateType);
                                            existGate->getFainPtrs()->push_back(newGate);
                                            existGate->incrFaninNumber();
                                            newGate->getFanoutPtrs()->push_back(existGate);
                                            newGate->incrFanoutNumber();
                                            myGateList.getGatelist()->push_back(newGate);
                                        }
                                        else {
                                            newGate->setTypeName(thisGateType);
                                        }
                                    }

                                    // handle Fan-in
                                    vector<string>::iterator j;
                                    for(j=faninVect.begin();j!=faninVect.end();j++)
                                    {
                                        aFaninGate = myGateList.searchNameNotType(*j, "OUTP");
                                        /* if met this gate before*/
                                        if(aFaninGate != NULL) {
                                            newGate->getFainPtrs()->push_back(aFaninGate);
                                            newGate->incrFaninNumber();
                                            aFaninGate->getFanoutPtrs()->push_back(newGate);
                                            aFaninGate->incrFanoutNumber();
                                        }
                                        /* if not met this gate before*/
                                        else {
                                            aFaninGate = new gate(*j);
                                            newGate->getFainPtrs()->push_back(aFaninGate);
                                            newGate->incrFaninNumber();
                                            aFaninGate->getFanoutPtrs()->push_back(newGate);
                                            aFaninGate->incrFanoutNumber();
                                            myGateList.getGatelist()->push_back(aFaninGate);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            write_result_ckt(inputCount, outputCount, argv[2], &myGateList);
        }
        else if(strncmp(argv[1],"read_nldm", 9) == 0){
            ifstream fileInput ( argv[3] );
            bool delayFlag = false;
            bool slewFlag = false;
            bool cellFlag = false;

            if ( !fileInput.is_open() )
                cout<<"Could not open file\n";
            else{
                while(!fileInput.eof()){
                    getline(fileInput,aline);

                    if(!aline.empty()) {

                        aline = trim(aline);

                        if(aline.substr(0, 5) == "cell "){
                            cellFlag = true;
                            cellName = getIOname(aline);
                            newCell = new cell(cellName);
                            string cellType = readCellType(cellName);
                            cout << cellType << endl;
                            newCell->setCellType(cellType);
                            mycellList.getCellList()->push_back(newCell);
                        }
                        else if(aline.substr(0, 11) == "capacitance"){
                            double cin = stod(readCapacitance(aline));
                            newCell->setCin(cin);
                        }
                        else if(aline.substr(0, 10) == "cell_delay"){
                            delayFlag = true;
                        }
                        else if(aline.substr(0, 7) == "index_1" && cellFlag == true){
                            string index1 = readTauIn(aline);
                            stringstream sstream(index1);
                            double x;
                            int i = 0;
                            while(sstream >> x){
                                (*newCell).TauIn[i] = x;
                                if(sstream.peek() == ',') sstream.ignore();
                                i++;
                            }
                        }
                        else if(aline.substr(0, 7) == "index_2" && cellFlag == true){
                            string index2 = readTauIn(aline);
                            stringstream sstream(index2);
                            double x;
                            int i = 0;
                            while(sstream >> x){
                                (*newCell).cL[i] = x;
                                if(sstream.peek() == ',') sstream.ignore();
                                i++;
                            }
                        }
                        else if(delayFlag == true && slewFlag == false && aline.substr(0, 6) == "values"){
                            string valueD = readValue(aline);
//                                cout << valueD << endl;
                            stringstream sstream(valueD);
                            double x;
                            int i = 0, j = 0;
                            while(sstream >> x){
                                (*newCell).delays[i][j] = x;
//                                    cout << i << ", " << j << "----" << (*newCell).delays[i][j] << endl;
                                if(sstream.peek() == ',') sstream.ignore();
                                j++;
                            }
                            i++;

                            for(int k = 0; k < 6; k++){
                                getline(fileInput, aline);
                                aline = trim(aline);
                                string valueD = readValue(aline);
//                                cout << valueD << endl;
                                stringstream sstream(valueD);
                                double x;
                                int j = 0;
                                while(sstream >> x){
                                    (*newCell).delays[i][j] = x;
//                                        cout << i << ", " << j << "----" << (*newCell).delays[i][j] << endl;
                                    if(sstream.peek() == ',') sstream.ignore();
                                    j++;
                                }
                                i++;
                            }
                            delayFlag = false;

                        }
                        else if(aline.substr(0, 11) == "output_slew"){
                            slewFlag = true;
                        }
                        else if(slewFlag == true && aline.substr(0, 6) == "values"){
                            string valueD = readValue(aline);
//                                cout << valueD << endl;
                            stringstream sstream(valueD);
                            double x;
                            int i = 0, j = 0;
                            while(sstream >> x){
                                (*newCell).slews[i][j] = x;
//                                    cout << i << ", " << j << "----" << (*newCell).slews[i][j] << endl;
                                if(sstream.peek() == ',') sstream.ignore();
                                j++;
                            }
                            i++;

                            for(int k = 0; k < 6; k++){
                                getline(fileInput, aline);
                                aline = trim(aline);
                                string valueD = readValue(aline);
//                                cout << valueD << endl;
                                stringstream sstream(valueD);
                                double x;
                                int j = 0;
                                while(sstream >> x){
                                    (*newCell).slews[i][j] = x;
//                                        cout << i << ", " << j << "----" << (*newCell).slews[i][j] << endl;
                                    if(sstream.peek() == ',') sstream.ignore();
                                    j++;
                                }
                                i++;
                            }

                            slewFlag = false;
                            cellFlag = false;
                        }
                    }
                }
            }

            if(strncmp(argv[2], "delays", 6) == 0){
                write_result_delays(&mycellList);
            }
            else if(strncmp(argv[2], "slews", 6) == 0){
                write_result_slews(&mycellList);
            }
            else {
                cout<<"usage: "<< argv[0] <<" read_nldm <arguments> <filename>\n <arguments> options: delays, slews\n";
            }
        }
        else{
            /* phase 2 */

            ifstream nldmFile ( "sample_NLDM.lib" );
            bool delayFlag = false;
            bool slewFlag = false;
            bool cellFlag = false;

            if ( !nldmFile.is_open() )
                cout<<"Could not open file\n";
            else{
                while(!nldmFile.eof()){
                    getline(nldmFile,aline);

                    if(!aline.empty()) {

                        aline = trim(aline);

                        if(aline.substr(0, 5) == "cell "){
                            cellFlag = true;
                            cellName = getIOname(aline);
                            newCell = new cell(cellName);
                            string cellType = readCellType(cellName);
                            cout << cellType << endl;
                            newCell->setCellType(cellType);
                            mycellList.getCellList()->push_back(newCell);
                        }
                        else if(aline.substr(0, 11) == "capacitance"){
                            double cin = stod(readCapacitance(aline));
                            newCell->setCin(cin);
                        }
                        else if(aline.substr(0, 10) == "cell_delay"){
                            delayFlag = true;
                        }
                        else if(aline.substr(0, 7) == "index_1" && cellFlag == true){
                            string index1 = readTauIn(aline);
                            stringstream sstream(index1);
                            double x;
                            int i = 0;
                            while(sstream >> x){
                                (*newCell).TauIn[i] = x;
                                if(sstream.peek() == ',') sstream.ignore();
                                i++;
                            }
                        }
                        else if(aline.substr(0, 7) == "index_2" && cellFlag == true){
                            string index2 = readTauIn(aline);
                            stringstream sstream(index2);
                            double x;
                            int i = 0;
                            while(sstream >> x){
                                (*newCell).cL[i] = x;
                                if(sstream.peek() == ',') sstream.ignore();
                                i++;
                            }
                        }
                        else if(delayFlag == true && slewFlag == false && aline.substr(0, 6) == "values"){
                            string valueD = readValue(aline);
//                                cout << valueD << endl;
                            stringstream sstream(valueD);
                            double x;
                            int i = 0, j = 0;
                            while(sstream >> x){
                                (*newCell).delays[i][j] = x;
//                                    cout << i << ", " << j << "----" << (*newCell).delays[i][j] << endl;
                                if(sstream.peek() == ',') sstream.ignore();
                                j++;
                            }
                            i++;

                            for(int k = 0; k < 6; k++){
                                getline(nldmFile, aline);
                                aline = trim(aline);
                                string valueD = readValue(aline);
//                                cout << valueD << endl;
                                stringstream sstream(valueD);
                                double x;
                                int j = 0;
                                while(sstream >> x){
                                    (*newCell).delays[i][j] = x;
//                                        cout << i << ", " << j << "----" << (*newCell).delays[i][j] << endl;
                                    if(sstream.peek() == ',') sstream.ignore();
                                    j++;
                                }
                                i++;
                            }
                            delayFlag = false;

                        }
                        else if(aline.substr(0, 11) == "output_slew"){
                            slewFlag = true;
                        }
                        else if(slewFlag == true && aline.substr(0, 6) == "values"){
                            string valueD = readValue(aline);
//                                cout << valueD << endl;
                            stringstream sstream(valueD);
                            double x;
                            int i = 0, j = 0;
                            while(sstream >> x){
                                (*newCell).slews[i][j] = x;
//                                    cout << i << ", " << j << "----" << (*newCell).slews[i][j] << endl;
                                if(sstream.peek() == ',') sstream.ignore();
                                j++;
                            }
                            i++;

                            for(int k = 0; k < 6; k++){
                                getline(nldmFile, aline);
                                aline = trim(aline);
                                string valueD = readValue(aline);
//                                cout << valueD << endl;
                                stringstream sstream(valueD);
                                double x;
                                int j = 0;
                                while(sstream >> x){
                                    (*newCell).slews[i][j] = x;
//                                        cout << i << ", " << j << "----" << (*newCell).slews[i][j] << endl;
                                    if(sstream.peek() == ',') sstream.ignore();
                                    j++;
                                }
                                i++;
                            }

                            slewFlag = false;
                            cellFlag = false;
                        }
                    }
                }
            }

//            write_result_delays(&mycellList);

            ifstream fileInput ( argv[1] );
            if ( !fileInput.is_open() )
                cout<<"Could not open file\n";
            else {
                while(!fileInput.eof()) {
                    getline(fileInput,aline);
                    if(isalpha(aline[0]) || (aline[0]>='0' && aline[0]<= '9')) {
                        if(!aline.empty()) {
                            if(aline[0]!='#') {
                                // inputs
                                if(aline.substr(0,5)=="INPUT") {
                                    inputCount++;
                                    gateName = getIOname(aline);
                                    newGate = new gate(gateName, "INP");
                                    myGateList.getGatelist()->push_back(newGate);
                                    myGateList.getInputs()->push_back(newGate);
                                }
                                    // outputs
                                else if(aline.substr(0,6)=="OUTPUT") {
                                    outputCount++;
                                    gateName = getIOname(aline);
                                    newGate = new gate(gateName, "OUTP");

                                    inputGate = myGateList.searchGate(gateName);

                                    /*check if exist input directly connect to output*/
                                    if(inputGate != NULL)
                                    {
                                        newGate->getFainPtrs()->push_back(inputGate);
                                        newGate->incrFaninNumber();
                                        inputGate->getFanoutPtrs()->push_back(newGate);
                                        inputGate->incrFanoutNumber();
                                    }

                                    myGateList.getGatelist()->push_back(newGate);
                                    myGateList.getOutputs()->push_back(newGate);

                                }
                                    // other logical gates
                                else {
                                    // gate type count
                                    thisGateType = readGateType(aline);
                                    newGateType = myGateList.searchGateType(thisGateType);
                                    if(newGateType == NULL) {
                                        myGateList.getTypeList()->push_back(new gateType(thisGateType));
                                        newGateType = myGateList.searchGateType(thisGateType);
                                        newGateType->typeCountIncr();
                                    }
                                    else {
                                        newGateType->typeCountIncr();
                                    }

                                    // gate fanout, fanin
                                    gateName = readGateName(aline);
                                    thisGateType = readGateType(aline);
                                    faninVect = readGateFanin(aline);

                                    /*check if this gate already meet before*/
                                    newGate = myGateList.searchGate(gateName);
                                    /*if not met this gate before*/
                                    if(newGate == NULL){
                                        newGate = new gate(gateName, thisGateType);
                                        myGateList.getGatelist()->push_back(newGate);
                                    }
                                        /*if met this gate before*/
                                    else {
                                        if(newGate->getTypeName() == "OUTP"){
                                            existGate = newGate;
                                            newGate = new gate(gateName, thisGateType);
                                            existGate->getFainPtrs()->push_back(newGate);
                                            existGate->incrFaninNumber();
                                            newGate->getFanoutPtrs()->push_back(existGate);
                                            newGate->incrFanoutNumber();
                                            myGateList.getGatelist()->push_back(newGate);
                                        }
                                        else {
                                            newGate->setTypeName(thisGateType);
                                        }
                                    }

                                    // handle Fan-in
                                    vector<string>::iterator j;
                                    for(j = faninVect.begin(); j != faninVect.end(); j++)
                                    {
                                        aFaninGate = myGateList.searchNameNotType(*j, "OUTP");
                                        /* if met this gate before*/
                                        if(aFaninGate != NULL) {
                                            newGate->getFainPtrs()->push_back(aFaninGate);
                                            newGate->incrFaninNumber();
                                            aFaninGate->getFanoutPtrs()->push_back(newGate);
                                            aFaninGate->incrFanoutNumber();
                                        }
                                            /* if not met this gate before*/
                                        else {
                                            aFaninGate = new gate(*j);
                                            newGate->getFainPtrs()->push_back(aFaninGate);
                                            newGate->incrFaninNumber();
                                            aFaninGate->getFanoutPtrs()->push_back(newGate);
                                            aFaninGate->incrFanoutNumber();
                                            myGateList.getGatelist()->push_back(aFaninGate);
                                        }
                                    }

                                    // handle cin
                                    cell* findCell = mycellList.searchCellByType(thisGateType);
                                        if(findCell != NULL){
                                        newGate->setCin(findCell->getCin());
                                    }
                                }
                            }
                        }
                    }
                }
            }
//            write_result_ckt(inputCount, outputCount, argv[1], &myGateList);
            // Calculation starts from here



            calculateCout( &myGateList, &mycellList);

            traversCkt(&myGateList, &mycellList);

        }
    }
}

void traversCkt(gateList *myGateList, cellList *myCellList){
    list<gate*>::iterator it;
    vector<gate*>::iterator itFanout;
    queue<gate*> myQueue;
    gate* thisGate;
    gate* fanoutGate;
    set<string> checkSet;
    cell* thisCell;

    for(it = myGateList->getInputs()->begin(); it != myGateList->getInputs()->end(); it++)
    {
        thisGate = (*it);
        thisGate->setaOut(0);
        thisGate->setTauOut((double) 2/1000);

        double aOut = 0.0;
        double d = 0.0;
        double tauOut = 0.0;
        double tauIn = 0.0;
        double aIn = 0.0;
        double cL = 0.0;
        int faninNum = 0;
        for(itFanout = thisGate->getFanoutPtrs()->begin(); itFanout != thisGate->getFanoutPtrs()->end(); itFanout++){
            fanoutGate = (*itFanout);
            fanoutGate->incrCompletedFaninNumber();

            // calculate fanout ain, tauin
            tauIn = thisGate->getTauOut();
            aIn = thisGate->getAout();
            cL = fanoutGate->getCout();
            faninNum = fanoutGate->getFaninNumber();

            // read LUT, find d
            thisCell = myCellList->searchCellByType(fanoutGate->getTypeName());
            if(thisCell != NULL){
                d = calculateDelaysOrSlews(thisCell, tauIn, cL, faninNum, 1);
            }
            aOut = aIn + d;

            // update
            if(aOut > fanoutGate->getAout()){
                fanoutGate->setTauIn(tauIn);
                fanoutGate->setaIn(aIn);
                fanoutGate->setaOut(aOut);
                fanoutGate->setD(d);
                tauOut = calculateDelaysOrSlews(thisCell, tauIn, cL, faninNum,0);
                fanoutGate->setTauOut(tauOut);
                fanoutGate->setCriticalPtr(thisGate);
            }

            const bool is_in = checkSet.find(fanoutGate->getGateName()) != checkSet.end();
            if(!is_in){
                // if not in myQueue, push into myQueue
                myQueue.push(fanoutGate);
                checkSet.insert(fanoutGate->getGateName());
            }
        }
    }



    while(!myQueue.empty()) {
        thisGate = myQueue.front();

        // can calculate tauOut and aOut for this gate
        if(thisGate->getCompletedFaninNumber() == thisGate->getFaninNumber()){

//            cout << "find a completed gate: " << thisGate->getGateName() << endl;

            // find all fanOuts
            if(thisGate->getFanoutNumber() != 0){

                double aOut = 0.0;
                double d = 0.0;
                double tauOut = 0.0;
                double tauIn = 0.0;
                double aIn = 0.0;
                double cL = 0.0;
                int faninNum = 0;
                for(itFanout = thisGate->getFanoutPtrs()->begin(); itFanout != thisGate->getFanoutPtrs()->end(); itFanout++){
                    fanoutGate = (*itFanout);

                    if(fanoutGate->getTypeName() == "OUTP"){
                        aIn = thisGate->getAout();
                        fanoutGate->setaOut(aIn);
                        fanoutGate->setCriticalPtr(thisGate);
                        cout << thisGate->getGateName() << "has a fanout gate is output: " << fanoutGate->getGateName() << endl;

                    }
                    else{
                        fanoutGate->incrCompletedFaninNumber();

                        // calculate next ain, tauIn
                        tauIn = thisGate->getTauOut();
                        aIn = thisGate->getAout();
                        cL = fanoutGate->getCout();
                        faninNum = fanoutGate->getFaninNumber();

                        // read LUT, find d
                        thisCell = myCellList->searchCellByType(fanoutGate->getTypeName());
                        if(thisCell != NULL){
                            d = calculateDelaysOrSlews(thisCell, tauIn, cL, faninNum, 1);
                        }
                        aOut = aIn + d;

                        // update
                        if(aOut > fanoutGate->getAout()){
                            fanoutGate->setTauIn(tauIn);
                            fanoutGate->setaIn(aIn);
                            fanoutGate->setaOut(aOut);
                            fanoutGate->setD(d);
                            tauOut = calculateDelaysOrSlews(thisCell, tauIn, cL, faninNum,0);
                            fanoutGate->setTauOut(tauOut);
                            fanoutGate->setCriticalPtr(thisGate);
//                            cout << "critical############# " << fanoutGate->getCriticalPtr()->getGateName() << endl;
                        }

                        // check if this fanout gate already in myQueue
                        const bool is_in = checkSet.find(fanoutGate->getGateName()) != checkSet.end();
                        if(!is_in){
                            // if not in myQueue, push into myQueue
                            myQueue.push(fanoutGate);
                            checkSet.insert(fanoutGate->getGateName());
                        }

                    }

                }
            }

            checkSet.erase(thisGate->getGateName());
        }
        else {
            myQueue.push(thisGate);
        }
        myQueue.pop();
    }

    gate* criticalPathEnd;

    // find max output arrival time
    double criticalAout = 0;
    for(it = myGateList->getOutputs()->begin(); it != myGateList->getOutputs()->end(); it++)
    {
        if((*it)->getAout() > criticalAout){
            criticalAout = (*it)->getAout();
            criticalPathEnd = (*it);
        }
    }

    list<gate*> criticalPath;

    while(criticalPathEnd != NULL){
        criticalPath.push_front(criticalPathEnd);
        criticalPathEnd = criticalPathEnd->getCriticalPtr();
    }

    // calculate required time
    double requireTime = criticalAout * 1.1;

//    for(it = myGateList->getOutputs()->begin(); it != myGateList->getOutputs()->end(); it++){
//        cout << (*it)->getGateName() << endl;
//        (*it)->setReqTime(requireTime);
//        cout << "OUTPUT req: " << (*it)->getReqTime() << endl;
//        gate* prev = (*it)->getCriticalPtr();
//        prev->setReqTime(requireTime);
//        while( prev != NULL){
////            cout << "delay: " << prev->getD() << endl;
//            double newReq = prev->getReqTime() - prev->getD();
//            prev->getCriticalPtr()->setReqTime(newReq);
////            cout << "req: " << prev->getCriticalPtr()->getReqTime() << endl;
//            prev = prev->getCriticalPtr();
//        }
//    }

//    for(it = myGateList->getGatelist()->begin(); it != myGateList->getGatelist()->end(); it++){
//
//    }



        // write reslult in a file
    string resultFile = "ckt_traversal.txt";
    ofstream result;
    list<gate*>::iterator k;

    result.open(resultFile.c_str());

    result << "---------------------------------------------------------------" << endl;
    result << "Circuit delay: " << criticalAout * 1000 << " ps" << endl << endl;

    result << "Gate slacks: " << endl;

    result << "Critical Path: " << endl;

    for(it = criticalPath.begin(); it != criticalPath.end(); it++){
        result << (*it)->getTypeName() << "-" << (*it)->getGateName() << ", ";
    }

    result << endl;
    result << "---------------------------------------------------------------" << endl << endl;

    result.close();
    cout << "Result is output to the file: " << resultFile << endl;
}

double calculateDelaysOrSlews(cell *thisCell, double ipslew, double C_load, int faninNum, int delay_or_slew){
    //First find T1 and T2 between which input slew (ipslew) lies
    //Then find C1 and C2 between which load cap (C_load) lies
    int N = 7;
    double T1, T2;
    double C1, C2;
    double val;
    //indices of cap and slew values, and the indices of the LUT initialized to -1
    int Ci=-1, Si=-1, i1=-1, i2=-1, j1=-1, j2=-1;

    /*Let the vector of slews be thisCell->TauIn and vector of caps be thisCell->cL
     * (specified as the range in the LUT). These are 1xN vectors, and your LUT is 7x7
     */

    for(int i=0;i<N;i++) {
        /*Si is the index of the slew value within the range of slews which is just greater than ipslew,
         * so your ipslew lies between thisCell->TauIn[Si-1] and thisCell->TauIn[Si]
         */
        if(thisCell->TauIn[i] > ipslew) {Si=i; break;}
    }

    if(Si<=6 && Si>0){T2 = thisCell->TauIn[Si]; T1=thisCell->TauIn[Si-1]; i2=Si; i1=Si-1;} //ipslew within the slew range

    /*But if ipslew is smaller than the 1st entry of thisCell->TauIn, then Si is 0;
     * if ipslew larger than the last entry of thisCell->TauIn, then Si is -1.
     * For both cases, you need to extrapolate.
     */
    if(Si==0){T2=thisCell->TauIn[1]; T1=thisCell->TauIn[0]; i2=1; i1=0;}//1st entry in slew range is bigger than ipslew
    if(Si==-1){T2=thisCell->TauIn[N-1]; T1=thisCell->TauIn[N-2]; i2=N-1; i1=N-2;} //even the last entry is smaller than ipslew

    for(int i=0;i<N;i++) {
        if(thisCell->cL[i]>C_load) {Ci=i; break;}
        /*Ci is the index of the cap value within the range of caps which is just greater than C_load,
         * so your load cap lies between thisCell->cL[Ci-1] and thisCell->cL[Ci]
         */
    }

    if(Ci<=6 && Ci>0){C2=thisCell->cL[Ci]; C1=thisCell->cL[Ci-1]; j2=Ci; j1=Ci-1;} //cap value within the cap range

    /*If C_load is smaller than the 1st entry of thisCell->cL, then Ci is 0;
    * if C_load larger than the last entry of thisCell->cL, then Ci is -1.
    * For both cases, you need to extrapolate.
    */

    if(Ci==0){C2=thisCell->cL[1]; C1=thisCell->cL[0]; j2=1; j1=0;}//1st entry in caps range is bigger than C_load
    if(Ci==-1){C2=thisCell->cL[N-1]; C1=thisCell->cL[N-2];j2=N-1; j1=N-2;} //even the last entry is smaller than C_load

    /*Let your delay LUT is D[7][7] and output slew LUT is S[7][7].
     * Let delay_or_slew be 1 when you want to look up delay, and 0, when you want to look up slew.
     */

    /*You want to obtain the delay (or output slew) value corresponding to (C_load, ipslew).
     * Then you have to use the four values, D[i1][j1], D[i1][j2], D[i2][j1],
     * and D[i2][j2] (or S[i1][j1], S[i1][j2], S[i2][j1], and S[i2][j2]),
     * and apply the interpolation formula given in the handout of MP1.
     */

    //look up delay
    if(delay_or_slew == 1)
    {
        val = (thisCell->delays[i1][j1]*(C2-C_load)*(T2-ipslew) + thisCell->delays[i1][j2]*(C_load-C1)*(T2-ipslew) +
                thisCell->delays[i2][j1]*(C2-C_load)*(ipslew-T1) + thisCell->delays[i2][j2]*(C_load-C1)*(ipslew-T1) )/((C2-C1)*(T2-T1));

        //if the calculated delay is negative, reset it to zero.
        if(val < 0) val = 0;

        /*scaling for a larger gate with fanin count greater than 2.
         * The if condition takes care that you don't wrongfully scale the value for gates with single input,
         * otherwise for inverters and buffers you will take half of their actual delay values.
         */
        if(faninNum > 2) {
            val = (double(faninNum)/2.0) * val;
        }
    }
    //look up slew
    else {
        val = thisCell->slews[i1][j1]*(C2-C_load)*(T2-ipslew) + thisCell->slews[i1][j2]*(C_load-C1)*(T2-ipslew) +
                thisCell->slews[i2][j1]*(C2-C_load)*(ipslew-T1) + thisCell->slews[i2][j2]*(C_load-C1)*(ipslew-T1)/((C2-C1)*(T2-T1));
        /*scaling for a larger gate with fanin count greater than 2.
         * The if condition takes care that you don't wrongfully scale the value for gates with single input,
         * otherwise for inverters and buffers you will take half of their actual delay slews.
         */
        if(faninNum > 2) {
            val = (double(faninNum)/2.0) * val;
        }
        //resetting slew values to 2 ps if the calculated slew is negative
        if(val < 0) val = 0.002;
    }
    return val;
}

void calculateCout(gateList *myGateList, cellList *myCellList){
    list<gate*>::iterator j;
    vector<gate*>::iterator i;
    gate* fanoutGate;
    double outputCout;

    outputCout = 4.0 * myCellList->searchCellByType("NOT")->getCin();

    for(j = myGateList->getOutputs()->begin(); j != myGateList->getOutputs()->end(); j++){
            (*j)->setCin(outputCout);
//        cout << (*j)->getCout() << endl;
    }

    for(j = myGateList->getGatelist()->begin(); j != myGateList->getGatelist()->end(); j++){
        double myCin = 0.0;
        if((*j)->getTypeName() != "OUTP"){
            for(i = (*j)->getFanoutPtrs()->begin(); i != (*j)->getFanoutPtrs()->end(); i++){
                fanoutGate = (*i);
                myCin = myCin + fanoutGate->getCin();
            }
            (*j)->setCout(myCin);
        }
    }

    // for debug

//    string resultFile = "testCap.out";
//    ofstream result;
//    list<gate*>::iterator k;
//
//    result.open(resultFile.c_str());
//
//    result << "------------------------------------------------" << endl;
//
//    for( k = myGateList->getGatelist()->begin(); k != myGateList->getGatelist()->end(); k++) {
//        result<<(*k)->getTypeName() << "-" << (*k)->getGateName()<< ": " << (*k)->getCout() << endl;
//    }
//    result << "------------------------------------------------" << endl;
//    result.close();
//    cout << "Result is output to the file: " << resultFile << endl;
}

void write_result_ckt(int inputCount, int outputCount, string fileName, gateList *myGateList){
    string resultFile = fileName + ".out";
    ofstream result;
    list<gateType*>::iterator k;
    list<gate*>::iterator j;
    vector<gate*>::iterator i;
    gate* thisGate;

    result.open(resultFile.c_str());

    result << "------------------------------------------------" << endl;
    result << inputCount << " primary inputs" << endl;
    result << outputCount << " primary outputs"<< endl;
    for( k = myGateList->getTypeList()->begin(); k != myGateList->getTypeList()->end(); k++) {
        result<<(*k)->getTypeCount() << ' ' << (*k)->getTypeName()<< ' ' << "gates"<< endl;
    }

    result << endl;
    result << "Fanout..." << endl;

    for( j = myGateList->getGatelist()->begin(); j != myGateList->getGatelist()->end(); j++){
        if((*j)->getTypeName() != "INP" && (*j)->getTypeName() != "OUTP"){
            result << (*j)->getTypeName() << "-" << (*j)->getGateName() << ": ";

            for(i = (*j)->getFanoutPtrs()->begin(); i != (*j)->getFanoutPtrs()->end(); i++){
                thisGate = (*i);
                result << thisGate->getTypeName() << "-" << thisGate->getGateName() << ", ";
            }
            result << endl;
        }
    }

    result << endl;
    result << "Fanin..." << endl;

    for( j = myGateList->getGatelist()->begin(); j != myGateList->getGatelist()->end(); j++){
        if((*j)->getTypeName() != "INP" && (*j)->getTypeName() != "OUTP" && (*j)->getTypeName() != ""){
            result << (*j)->getTypeName() << "-" << (*j)->getGateName() << ": ";

            for(i = (*j)->getFainPtrs()->begin(); i != (*j)->getFainPtrs()->end(); i++){
                thisGate = (*i);
                result << thisGate->getTypeName() << "-" << thisGate->getGateName() << ", ";
            }
            result << endl;
        }
    }

    result << "------------------------------------------------" << endl;
    result.close();
    cout << "Result is output to the file: " << resultFile << endl;
}

void write_result_delays(cellList *mycellList){
    string resultFile = "delay_LUT.txt";
    ofstream result;
    list<cell*>::iterator k;

    result.open(resultFile.c_str());

    result << "------------------------------------------------" << endl;
    for( k = mycellList->getCellList()->begin(); k != mycellList->getCellList()->end(); k++) {
        result << "cell: " << (*k)->getCellName() << endl;

        result << "input slews: " << endl;
        for(int i = 0; i < 6; i++){
            result << (*k)->TauIn[i] << ",";
        }
        result << (*k)->TauIn[6] << endl;

        result << "load cap: " << endl;
        for(int i = 0; i < 6; i++){
            result << (*k)->cL[i] << ",";
        }
        result << (*k)->cL[6] << endl;

        result << endl;

        result << "delays: " << endl;
        for(int i = 0; i < 7; i++){
            for(int j = 0; j < 6; j++){
                result << (*k)->delays[i][j] << ",";
            }
            result << (*k)->delays[i][6] << ";" << endl;
        }

        result << endl;

    }
    result << "------------------------------------------------" << endl;
    result.close();
    cout << "Result is output to the file: " << resultFile << endl;
}

void write_result_slews(cellList *mycellList){
    string resultFile = "slew_LUT.txt";
    ofstream result;
    list<cell*>::iterator k;

    result.open(resultFile.c_str());

    result << "------------------------------------------------" << endl;
    for( k = mycellList->getCellList()->begin(); k != mycellList->getCellList()->end(); k++) {
        result << "cell: " << (*k)->getCellName() << endl;

        result << "input slews: " << endl;
        for(int i = 0; i < 6; i++){
            result << (*k)->TauIn[i] << ",";
        }
        result << (*k)->TauIn[6] << endl;

        result << "load cap: " << endl;
        for(int i = 0; i < 6; i++){
            result << (*k)->cL[i] << ",";
        }
        result << (*k)->cL[6] << endl;

        result << endl;

        result << "slews: " << endl;
        for(int i = 0; i < 7; i++){
            for(int j = 0; j < 6; j++){
                result << (*k)->slews[i][j] << ",";
            }
            result << (*k)->slews[i][6] << ";" << endl;
        }

        result << endl;

    }
    result << "------------------------------------------------" << endl;
    result.close();
    cout << "Result is output to the file: " << resultFile << endl;
}

string getIOname(string str) {
    int head,tail;
    head=str.find('(')+1;
    tail=str.find(')');
    tail=tail-head;
    str =str.substr(head,tail);
    /*discard spaces at the head of the string*/
    while(*(str.begin())==' ')
    {
        str.erase(str.begin());
    }
    /*discards spaces at the tail of the string*/
    while(*(str.rbegin())==' ')
    {
        str=str.substr(0,str.size()-1);
    }
    return str;
}

string readGateType(string str)
{
    int head,tail;
    head=str.find('=')+1;
    tail=str.find('(');
    tail=tail-head;
    str =str.substr(head,tail);
    /*discards spaces at the head of the string*/
    while(*(str.begin())==' ')
    {
        str.erase(str.begin());
    }
    /*discards spaces at the tail of the string*/
    while(*(str.rbegin())==' ')
    {
        str=str.substr(0,str.size()-1);
    }
    return str;
}

string readGateName(string str)
{
    int a;
    a=str.find('=');
    str=str.substr(0,a);
    /*discard spaces at the head of the string*/
    while(*(str.begin())==' ')
    {
        str.erase(str.begin());
    }
    /*discards spaces at the tail of the string*/
    while(*(str.rbegin())==' ')
    {
        str=str.substr(0,str.size()-1);
    }
    return str;

}

string readCellType(string str) {
    int a;
    a = str.find('_');
    str = str.substr(0, a);
    if(isdigit(str[a-1])){
        str = str.substr(0, a-1);
    }
    if(str == "INV"){
        str = "NOT";
    }
    if(str == "BUF"){
        str = "BUFF";
    }
    return str;
}

vector<string> readGateFanin(string str)
{
    int a;
    string subStr;
    vector<string> vector1;
    str = getIOname(str);
    while(str.find(',') != -1)
    {
        a = str.find(',');
        subStr=str.substr(0,a);
        /*discards spaces at the head of the string*/
        while(*(subStr.begin()) == ' ')
        {
            subStr.erase(subStr.begin());
        }
        /*discards spaces at the tail of the string*/
        while(*(subStr.rbegin()) == ' ')
        {
            subStr=subStr.substr(0, subStr.size() - 1);
        }
        vector1.push_back(subStr);
        str.erase(0, a+1);
    }
    str = discardSpace(str);

    vector1.push_back(str);
    return vector1;
}

string readCapacitance(string str){
    int head,tail;
    head=str.find(':') + 1;
    tail=str.find(';');
    tail=tail-head;
    str =str.substr(head,tail);
    /*discard spaces at the head of the string*/
    while(*(str.begin())==' ')
    {
        str.erase(str.begin());
    }
    /*discards spaces at the tail of the string*/
    while(*(str.rbegin())==' ')
    {
        str=str.substr(0,str.size() - 1);
    }
    return str;
}


string readTauIn(string str){
    int head,tail;
    head=str.find('"') + 1;
    tail=str.find(')') - 1;
    tail=tail-head;
    str =str.substr(head,tail);
    /*discard spaces at the head of the string*/
    while(*(str.begin())==' ')
    {
    str.erase(str.begin());
    }
    /*discards spaces at the tail of the string*/
    while(*(str.rbegin())==' ')
    {
    str=str.substr(0,str.size()-1);
    }
    return str;
}

string readValue(string str){
    int head,tail;
    head=str.find('"') + 1;
    tail=str.find(", ") - 1;
    tail=tail-head;
    str =str.substr(head,tail);
    /*discard spaces at the head of the string*/
    while(*(str.begin())==' ')
    {
        str.erase(str.begin());
    }
    /*discards spaces at the tail of the string*/
    while(*(str.rbegin())==' ')
    {
        str=str.substr(0,str.size()-1);
    }
    return str;
}

string discardSpace(string str){
    /*discards spaces at the head of the string*/
    while(*(str.begin())==' ')
    {
        str.erase(str.begin());
    }
    /*discards spaces at the tail of the string*/
    while(*(str.rbegin())==' ')
    {
        str=str.substr(0,str.size()-1);
    }
    return str;
}

//trim a string from left
inline string& ltrim(string& s)
{
    const char* t = " \t\n\r\f\v";
    s.erase(0, s.find_first_not_of(t));
    return s;
}
//trim a string from right
inline string& rtrim(string& s)
{
    const char* t = " \t\n\r\f\v";
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}
//trim a sting from left & right
inline string& trim(string& s)
{
    return ltrim(rtrim(s));
}
