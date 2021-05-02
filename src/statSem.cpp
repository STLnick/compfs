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
        // TODO: Determine the construct needed in VirtMach to delineate a _block_
        // TODO: i.e. how to establish a scope for variables essentially (?) - think through this more prolly
    }

    // Add variables to static semantic stack if in vars node
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
        // TODO: Handle ( <stat> <mStat> ) production (?)
    }

    if (treeNode->label == "stat_nt") {
        std::cout << "stat_nt node!" << std::endl;
        // TODO: Handle ( <in> ; <out> ; <block> ; <if> ; <loop> ; <assign> ; <goto> ; <label> ; ) productions (?)
    }

    if (treeNode->label == "mStat_nt") {
        std::cout << "mStat_nt node!" << std::endl;
        // TODO: Handle ( <stat> <mStat> ) production (?)
    }

    if (treeNode->label == "out_nt") { // WRITE operation
        outFile << "WRITE ";
    }

    if (treeNode->label == "in_nt") { // READ operation
        outFile << "READ " << treeNode->tokens[1].stringVal << "\n";
    }

    if (treeNode->label == "expr_nt") {
        std::cout << "expr_nt node!" << std::endl;
        // TODO: Handle ( <N> - <expr> ) production
        // TODO: Handle ( <N> ) production (?)
    }

    if (treeNode->label == "N_nt") {
        std::cout << "N_nt node!" << std::endl;
        // TODO: Handle ( <A> / <N> ) production
        // TODO: Handle ( <A> * <N> ) production
        // TODO: Handle ( <A> ) production (?)
    }

    if (treeNode->label == "A_nt") {
        std::cout << "A_nt node!" << std::endl;
        // TODO: Handle ( <M> + <A> ) production
        // TODO: Handle ( <M> ) production (?)
    }

    if (treeNode->label == "M_nt") {
        std::cout << "M_nt node!" << std::endl;
        // TODO: Handle ( *<M> ) production
        // TODO: Handle ( <R> ) production (?)
    }

    if (treeNode->label == "R_nt") {
        std::cout << "R_nt node --- " << treeNode->tokens[0].stringVal << std::endl;

        if (treeNode->tokens[0].tokenId == ID_tk || treeNode->tokens[0].tokenId == NUM_tk) {
            outFile << treeNode->tokens[0].stringVal << "\n";
        }

        // TODO: Handle ( <expr> ) production

    }

    if (treeNode->label == "if_nt") {
        std::cout << "if_nt node!" << std::endl;
        // TODO:
    }

    if (treeNode->label == "loop_nt") {
        std::cout << "loop_nt node!" << std::endl;
        // TODO:
    }

    if (treeNode->label == "assign_nt") {
        std::cout << "assign_nt node!" << std::endl;
        std::cout << "assign_nt size(): " << treeNode->tokens.size() << std::endl;
        for (int i = 0; i < treeNode->tokens.size(); i++) {
            std::cout << treeNode->tokens[i].stringVal << std::endl;
        }
        outFile << "LOAD ";
    }

    if (treeNode->label == "R0_nt") {
        std::cout << "R0_nt node!" << std::endl;
        // TODO:
    }

    if (treeNode->label == "label_nt") {
        std::cout << "label_nt node!" << std::endl;
        // TODO:
    }

    if (treeNode->label == "goto_nt") {
        std::cout << "goto_nt node!" << std::endl;
        // TODO:
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
    // Post processing for certain nodes
    /* - - - - - - - - - - - - - - - - - */

    if (treeNode->label == "assign_nt") {
        outFile << "STORE " << treeNode->tokens[1].stringVal << "\n";
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
