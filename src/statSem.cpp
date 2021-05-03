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

static int varFound = 1;
static int checkIndex;
static int labelCounter = 0; /* counting unique labels generated */
static int varCounter = 0; /* counting unique temporaries generated */
typedef enum {VAR, LABEL} nameType;
static char newName[20]; /* for creation of unique names */

static char *getNewName(nameType type) {
    if (type == VAR) // creating new temporary
        sprintf(newName, "T%d" , varCounter++); /* generate a new label as T0, T1, etc */
    else // creating new Label
        sprintf(newName, "L%d" , labelCounter++); /* new labels as L0, L1, etc */
    return(newName);
}

// dup error by default --- not declared error will require 3rd arg of 1
void printErrorAndExit(std::string varName, int line, bool isDuplicateError = true) {
    if (isDuplicateError) {
        std::cout << "[ERROR]: " << varName << " already declared on line " << line << std::endl;
    } else {
        std::cout << "[ERROR]: " << varName << " not declared before being used on line " << line << std::endl;
    }

    exit(1);
}

void checkVariablesWereDeclared(node *treeNode, StatSemStack &stack) {
    varFound = 1;

    for (checkIndex = 0; checkIndex < treeNode->tokens.size(); checkIndex++) {
        if (treeNode->tokens[checkIndex].tokenId == ID_tk) {
            varFound = stack.find(treeNode->tokens[checkIndex].stringVal);
            if (varFound == -1) {
                printErrorAndExit(treeNode->tokens[checkIndex].stringVal, treeNode->tokens[checkIndex].lineNum, false);
            }
        }
    }
}

void statSem(node *treeNode, StatSemStack &stack, int level, std::ofstream &outFile) {
            if (!stack.getIsGlobal()){
                stack.setIsGlobal(true);
            }
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

        if (stack.getIsGlobal()) {
            stack.pushVarName(treeNode->tokens[1].stringVal);
            outFile << "LOAD " << treeNode->tokens[3].stringVal << std::endl;
            outFile << "STORE " << treeNode->tokens[1].stringVal << std::endl;
        } else {
            outFile << "PUSH" << std::endl;
        }
    } else { // Check that all ID_tk's used have been declared in all other nodes
        checkVariablesWereDeclared(treeNode, stack);
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
        outFile << "READ " << treeNode->tokens[1].stringVal << std::endl;
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
        if (treeNode->tokens[0].tokenId == ID_tk || treeNode->tokens[0].tokenId == NUM_tk) {
            outFile << treeNode->tokens[0].stringVal << std::endl;
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
        outFile << "STORE " << treeNode->tokens[1].stringVal << std::endl;
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
