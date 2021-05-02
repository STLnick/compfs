//
// Created by Nick Ray on 4/16/21.
//
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "node.hpp"
#include "statSem.hpp"
#include "statSemStack.hpp"
#include "statSemStackItem.hpp"

// dup error by default --- not declared error will require 3rd arg of 1
void printErrorAndExit(std::string varName, int line, bool isDuplicateError = true) {
    if (isDuplicateError) {
        std::cout << "[ERROR]: " << varName << " already declared on line " << line << std::endl;
    } else {
        std::cout << "[ERROR]: " << varName << " not declared before being used on line " << line << std::endl;
    }

    exit(1);
}

void statSem(node *treeNode, StatSemStack &stack, int level, std::ofstream &outFile) {
    /* - - - - - - - - - - - - - - - - - - - */
    // Handle actions on different node types
    /* - - - - - - - - - - - - - - - - - - - */
    if (treeNode->label == "block_nt") {
        stack.pushBlock();
    }

    if (treeNode->label == "vars_nt") {
        int declaredLineNum = stack.find(treeNode->tokens[1].stringVal);

        if (declaredLineNum != -1) {
            // error - already declared
            printErrorAndExit(treeNode->tokens[1].stringVal, declaredLineNum);
        }

       StatSemStackItem* newItem = new StatSemStackItem(
                treeNode->tokens[1].stringVal,
                treeNode->tokens[1].charCol,
                treeNode->tokens[1].lineNum
            );

        stack.push(newItem);


        // TODO: Write to file!
        outFile << "LOAD " << treeNode->tokens[3].stringVal << "\n";
        outFile << "STORE " << treeNode->tokens[1].stringVal << "\n";


    } else { // Check that all ID_tk's used have been declared in all other nodes
        int found = 1;
        int i;

        for (i = 0; i < treeNode->tokens.size(); i++) {
            if (treeNode->tokens[i].tokenId == ID_tk) {
                found = stack.find(treeNode->tokens[i].stringVal);
                if (found == -1) {
                    printErrorAndExit(treeNode->tokens[i].stringVal, treeNode->tokens[i].lineNum, false);
                }
            }
        }
    }

    if (treeNode->label == "stats_nt") {
        std::cout << "stats_nt node!" << std::endl;
    }

    if (treeNode->label == "stat_nt") {
        std::cout << "stat_nt node!" << std::endl;
    }

    if (treeNode->label == "mStat_nt") {
        std::cout << "mStat_nt node!" << std::endl;
    }

    if (treeNode->label == "out_nt") { // WRITE operation
        outFile << "WRITE ";
    }

    if (treeNode->label == "in_nt") { // READ operation
        outFile << "READ " << treeNode->tokens[1].stringVal << "\n";
    }

    if (treeNode->label == "expr_nt") {
        std::cout << "expr_nt node!" << std::endl;
    }

    if (treeNode->label == "N_nt") {
        std::cout << "N_nt node!" << std::endl;
    }

    if (treeNode->label == "A_nt") {
        std::cout << "A_nt node!" << std::endl;
    }

    if (treeNode->label == "M_nt") {
        std::cout << "M_nt node!" << std::endl;
    }

    if (treeNode->label == "R_nt") {
        std::cout << "R_nt node --- " << treeNode->tokens[0].stringVal << std::endl;

        if (treeNode->tokens[0].tokenId == ID_tk || treeNode->tokens[0].tokenId == NUM_tk) {
            outFile << treeNode->tokens[0].stringVal << "\n";
        }

        // TODO: Handle ( <expr> ) production

    }

        /*
         * I think I do need to just write to file if I get down here to <R>
         * <R> can be both an int or an Identifier
         * So maybe check if the token.tokenId == ID_tk
         * if (ID_tk) then write to file for instructions to load a variable into ACC
         * else (should be a NUM_tk then) just load the number into ACC
         */
    }

    /* - - - - - - - - - - */
    // Traverse child nodes
    /* - - - - - - - - - - */
    if (treeNode->ntOne != NULL) {
        statSem(treeNode->ntOne, stack, level + 1, outFile);
    }

    if (treeNode->ntTwo != NULL) {
        statSem(treeNode->ntTwo, stack, level + 1, outFile);
    }

    if (treeNode->ntThree != NULL) {
        statSem(treeNode->ntThree, stack, level + 1, outFile);
    }

    if (treeNode->ntFour != NULL) {
        statSem(treeNode->ntFour, stack, level + 1, outFile);
    }


    /* - - - - - - - - - - - - - - - - - */
    // Pop statSemStack until end of block
    /* - - - - - - - - - - - - - - - - - */
    if (treeNode->label == "block_nt") {
        while (stack.getItemsSize() > 0 && stack.isNotOnBlockStop()) {
            stack.pop();
        }
        stack.pop(); // Remove BLOCK_STOP fake item
    }
}
