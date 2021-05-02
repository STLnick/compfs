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
public:
    StatSemStack();
    void push(StatSemStackItem *newItem);
    void pushBlock();
    void pop();
    int find(std::string bufferName);
    bool isNotOnBlockStop();
    int getItemsSize();
    int getVariablesNamesSize();
    std::string getVariableName(int index);
};

#endif //STATSEM_STATSEMSTACK_HPP
