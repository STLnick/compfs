//
// Created by Nick Ray on 4/19/21.
//

#ifndef STATSEM_STATSEMSTACKITEM_HPP
#define STATSEM_STATSEMSTACKITEM_HPP

struct StatSemStackItem {
    std::string name; // The identifying string value for the identifier
    int stackIndex;   // Index on VirtMach stack of variables
    int charCol;      // Column on line token begins
    int lineNum;      // Line number which the token is on in source

    StatSemStackItem(std::string _name, int _charCol, int _lineNum, int _stackIndex = -1) {
        name = _name;
        charCol = _charCol;
        lineNum = _lineNum;
        stackIndex = _stackIndex;
    }
};

#endif //STATSEM_STATSEMSTACKITEM_HPP
