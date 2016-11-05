#include <iostream>
#include <fstream>
#include <sstream>
#include "gate.h"
#include "gateList.h"
#include "cell.h"
#include <list>
#include "gateType.h"

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
void write_result_delays(list<cell*> *mycellList);
void write_result_slews(list<cell*> *mycellList);



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
    list<cell*> mycellList;

    int inputCount = 0, outputCount = 0;

    if ( argc < 3 ) // argc should be more than 3 for correct execution
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
                                        inputGate->getFanoutPtrs()->push_back(newGate);
                                    }

                                    myGateList.getGatelist()->push_back(newGate);

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
                                            newGate->getFanoutPtrs()->push_back(existGate);
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
                                            aFaninGate->getFanoutPtrs()->push_back(newGate);
                                        }
                                        /* if not met this gate before*/
                                        else {
                                            aFaninGate = new gate(*j);
                                            newGate->getFainPtrs()->push_back(aFaninGate);
                                            aFaninGate->getFanoutPtrs()->push_back(newGate);
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
                            newCell->setCellType(cellType);
                            mycellList.push_back(newCell);
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
    }
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

void write_result_delays(list<cell*> *mycellList){
    string resultFile = "delay_LUT.txt";
    ofstream result;
    list<cell*>::iterator k;

    result.open(resultFile.c_str());

    result << "------------------------------------------------" << endl;
    for( k = mycellList->begin(); k != mycellList->end(); k++) {
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

void write_result_slews(list<cell*> *mycellList){
    string resultFile = "slew_LUT.txt";
    ofstream result;
    list<cell*>::iterator k;

    result.open(resultFile.c_str());

    result << "------------------------------------------------" << endl;
    for( k = mycellList->begin(); k != mycellList->end(); k++) {
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
