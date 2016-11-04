#include <iostream>
#include <fstream>
#include "gate.h"
#include "gateList.h"
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

    int inputCount = 0, outputCount = 0;
    ifstream fileInput ( argv[2] );

    if ( argc < 3 ) // argc should be more than 3 for correct execution
        cout<<"usage: "<< argv[0] <<" <read command> <filename>\n <read command> options: read_ckt, read_nldm\n";
    else {
        if(strncmp(argv[1],"read_ckt", 9) == 0){
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
                                        aFaninGate = myGateList.searchGate(*j);
                                        /* if met this gate before*/
                                        if(aFaninGate != NULL && aFaninGate->getTypeName() != "OUTP") {
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
            if(strncmp(argv[2], "delays", 6) == 0){
                if ( !fileInput.is_open() )
                    cout<<"Could not open file\n";
                else{
                    while(!fileInput.eof()){
                        getline(fileInput,aline);
                        if(!aline.empty()) {
                            if(aline.substr(0,4) == "cell"){
                                cellName = getIOname(aline);
                                cout << cellName << endl;
                            }
                        }
                    }
                }

//                write_result_delays();
            }
            else if(strncmp(argv[2], "slews", 6) == 0){


//                write_result_slews();
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

vector<string> readGateFanin(string str)
{
    int a;
    string subStr;
    vector<string> vector1;
    str = getIOname(str);
    while(str.find(',')!=-1)
    {
        a = str.find(',');
        subStr=str.substr(0,a);
        /*discards spaces at the head of the string*/
        while(*(subStr.begin())==' ')
        {
            subStr.erase(subStr.begin());
        }
        /*discards spaces at the tail of the string*/
        while(*(subStr.rbegin())==' ')
        {
            subStr=subStr.substr(0,subStr.size()-1);
        }
        vector1.push_back(subStr);
        str.erase(0,a+1);
    }
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
    vector1.push_back(str);
    return vector1;
}
