//
// Created by Nick Ray on 4/17/21.
//

#include <iostream>
#include <vector>
#include "statSem.hpp"
#include "statSemStack.hpp"

StatSemStack::StatSemStack() {
    this->isGlobal = true;
}

bool StatSemStack::getIsGlobal() {
    return isGlobal;
}

void StatSemStack::setIsGlobal(bool val) {
    isGlobal = val;
}

void StatSemStack::push(StatSemStackItem *newItem) {
    items.push_back(newItem);
}

void StatSemStack::pushVarName(std::string name) {
    variableNames.push_back(name);
}

void StatSemStack::pushBlock() {
    StatSemStackItem* block = new StatSemStackItem(
            "BLOCK_STOP",
            -1,
            -1
    );
    this->push(block);
}

void StatSemStack::pop() {
    items.pop_back();
}

int StatSemStack::find(std::string bufferName) {
    std::vector<StatSemStackItem*>::iterator iter = items.begin();

    for (; iter != items.end(); ++iter) {
        if (bufferName == (*iter)->name) {
            return (*iter)->lineNum;
        }
    }

    return -1;
}

bool StatSemStack::isNotOnBlockStop() {
    return items.back()->name == "BLOCK_STOP" ? false : true;
}

int StatSemStack::getItemsSize() {
    return items.size();
}

int StatSemStack::getVariablesNamesSize() {
    return variableNames.size();
}

std::string StatSemStack::getVariableName(int index) {
    return variableNames[index];
}

