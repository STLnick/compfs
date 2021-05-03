//
// Created by Nick Ray on 4/17/21.
//

#ifndef STATSEM_STATSEMSTACK_HPP
#define STATSEM_STATSEMSTACK_HPP

#include "statSemStackItem.hpp"

class StatSemStack {
private:
    std::vector<StatSemStackItem*> items;
    std::vector<std::string> variableNames;
    bool isGlobal;
public:
    StatSemStack();
    bool getIsGlobal();
    void setIsGlobal(bool val);
    void push(StatSemStackItem *newItem);
    void pushVarName(std::string name);
    void pushBlock();
    void pop();
    int find(std::string bufferName);
    int findAndReturnLineNum(std::string bufferName);
    bool isNotOnBlockStop();
    int getItemsSize();
    int getVariablesNamesSize();
    std::string getVariableName(int index);
};

#endif //STATSEM_STATSEMSTACK_HPP
