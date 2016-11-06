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

    /* phase 2 */
    double cIn = 0; // capacitance
    double cOut = -1.0; // sum of all fanout Cin
    double tauIn = 0; // input_slew
    double aIn = 0; // fanin arrival time
    double d = 0; // cell delay corresponding to each path
    double tauOut = 0; // output_slew---get from LUT
    double aOut = 0; // arrivel time out
    gate* criticalPathPtr = NULL;
    int faninNumber = 0; // number of fanin gates
    int fanoutNumber = 0; // number of fanout gates
    int completeFaninNum = 0; // number of fanin gates that has been calculated
    double reqTime = 0;

public:
    gate();
    gate(string name);
    gate(string name, string type);
    string getGateName();
    void setTypeName(string name);
    string getTypeName();

    vector<gate*>* getFainPtrs();
    vector<gate*>* getFanoutPtrs();

    /* phase 2*/
    gate* getCriticalPtr();
    void setCriticalPtr(gate* ptr);
    void setaOut(double aout);
    void setTauOut(double tauout);
    void setCin(double cin);
    void setCout(double cout);
    double getCin();
    double getCout();
    void incrFaninNumber();
    int getFaninNumber();
    void incrCompletedFaninNumber();
    int getCompletedFaninNumber();
    void incrFanoutNumber();
    int getFanoutNumber();
    double getAout();
    double getTauOut();
    void setTauIn(double in);
    void setaIn(double ain);
    void setD(double dd);
    double getD();
    void setReqTime(double time);
    double getReqTime();
};

#endif //EEMINIPROJECT_GATE_H
