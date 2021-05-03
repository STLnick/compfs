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
static int stackIndex = 0;
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
            if (varFound == -99) {
                printErrorAndExit(treeNode->tokens[checkIndex].stringVal, treeNode->tokens[checkIndex].lineNum, false);
            }
        }
    }
}

void processInorder(node *treeNode, StatSemStack &stack, int level, std::ofstream &outFile) {
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
}

void statSem(node *treeNode, StatSemStack &stack, int level, std::ofstream &outFile) {
    std::string localNewName;
    std::string localNewLabel;
    std::string localNewLabelTwo;

    /* - - - - - - - - - - - - - - - - - - - */
    // Handle actions on different node types
    /* - - - - - - - - - - - - - - - - - - - */
    if (treeNode->label == "parser") {
        processInorder(treeNode->ntOne, stack, level + 1, outFile);
    }

    if (treeNode->label == "program_nt") {
        processInorder(treeNode->ntOne, stack, level + 1, outFile);
    }

    if (treeNode->label == "block_nt") {
        if (stack.getIsGlobal()) {
            stack.setIsGlobal(false);
        }

        stack.pushBlock();

        processInorder(treeNode, stack, level + 1, outFile);

        // Pop statSemStack until end of block
        while (stack.getItemsSize() > 0 && stack.isNotOnBlockStop()) {
            stack.pop();
            if (stackIndex > 0) {
                outFile << "POP" << std::endl;
                stackIndex--;
            }
        }
        stack.pop(); // Remove BLOCK_STOP fake item
    }

    // Add variables to static semantic stack if in vars node
    if (treeNode->label == "vars_nt") {
        int declaredLineNum = stack.findAndReturnLineNum(treeNode->tokens[1].stringVal);

        if (declaredLineNum != -1) {
            printErrorAndExit(treeNode->tokens[1].stringVal, declaredLineNum);
        }


        if (stack.getIsGlobal()) {
            stack.push(new StatSemStackItem(
                    treeNode->tokens[1].stringVal,
                    treeNode->tokens[1].charCol,
                    treeNode->tokens[1].lineNum
                ));

            stack.pushVarName(treeNode->tokens[1].stringVal);
            outFile << "LOAD " << treeNode->tokens[3].stringVal << std::endl;
            outFile << "STORE " << treeNode->tokens[1].stringVal << std::endl;
        } else {
            // TODO: Use the VirtMach stack to push and store values and such

            stack.push(new StatSemStackItem(
                    treeNode->tokens[1].stringVal,
                    treeNode->tokens[1].charCol,
                    treeNode->tokens[1].lineNum,
                    stackIndex
            ));

            outFile << "LOAD " << treeNode->tokens[3].stringVal << std::endl;
            outFile << "PUSH" << std::endl;
            outFile << "STACKW " << stackIndex++ << std::endl;
        }

        processInorder(treeNode, stack, level + 1, outFile);


    } else { // Check that all ID_tk's used have been declared in all other nodes
        checkVariablesWereDeclared(treeNode, stack);
    }

    if (treeNode->label == "stats_nt") {
        processInorder(treeNode, stack, level + 1, outFile);
    }

    if (treeNode->label == "stat_nt") {
        processInorder(treeNode, stack, level + 1, outFile);
    }

    if (treeNode->label == "mStat_nt") {
        processInorder(treeNode, stack, level + 1, outFile);
    }

    if (treeNode->label == "out_nt") { // WRITE operation

        statSem(treeNode->ntOne, stack, level + 1, outFile);
        outFile << "STORE " << getNewName(VAR) << std::endl;
        outFile << "WRITE " << newName << std::endl;
        stack.pushVarName(std::string(newName));
    }

    if (treeNode->label == "in_nt") { // READ operation
        int foundStackIndex = stack.find(treeNode->tokens[1].stringVal);

        if (foundStackIndex == -1) { // GLOBAL
            outFile << "READ " << treeNode->tokens[1].stringVal << std::endl;
        } else if (foundStackIndex >= 0) { // LOCAL
            localNewName = getNewName(VAR);
            outFile << "READ " << localNewName << std::endl;
            stack.pushVarName(localNewName);
            outFile << "LOAD " << localNewName << std::endl;
            outFile << "STACKW " << foundStackIndex << std::endl;
        } else {
            printErrorAndExit(treeNode->tokens[0].stringVal, treeNode->tokens[0].lineNum, false);
        }

    }

    if (treeNode->label == "expr_nt") {
        if (treeNode->tokens.size()) {
            statSem(treeNode->ntTwo, stack, level + 1, outFile);
            localNewName = getNewName(VAR);
            outFile << "STORE " << localNewName << std::endl;
            stack.pushVarName(localNewName);
        }

        statSem(treeNode->ntOne, stack, level + 1, outFile);

        if (treeNode->tokens.size()) {
            outFile << "SUB " << localNewName << std::endl;
        }
    }

    if (treeNode->label == "N_nt") {
        if (treeNode->tokens.size()) {
            statSem(treeNode->ntTwo, stack, level + 1, outFile);
            localNewName = getNewName(VAR);
            outFile << "STORE " << localNewName << std::endl;
            stack.pushVarName(localNewName);
        }

        statSem(treeNode->ntOne, stack, level + 1, outFile);

        if (treeNode->tokens.size()) {
            if (treeNode->tokens[0].tokenId == MULT_tk) {
                outFile << "MULT " << localNewName << std::endl;
            } else if (treeNode->tokens[0].tokenId == DIVIDE_tk) {
                outFile << "DIV " << localNewName << std::endl;
            }
        }
    }

    if (treeNode->label == "A_nt") {
        if (treeNode->tokens.size()) {
            statSem(treeNode->ntTwo, stack, level + 1, outFile);
            localNewName = getNewName(VAR);
            outFile << "STORE " << localNewName << std::endl;
            stack.pushVarName(localNewName);
        }

        statSem(treeNode->ntOne, stack, level + 1, outFile);

        if (treeNode->tokens.size()) {
            outFile << "ADD " << localNewName << std::endl;
        }
    }

    if (treeNode->label == "M_nt") {

        statSem(treeNode->ntOne, stack, level + 1, outFile);

        if (treeNode->tokens.size()) {
            outFile << "MULT -1" << std::endl;
        }
    }

    if (treeNode->label == "R_nt") {
        if (treeNode->tokens[0].tokenId == ID_tk) {
            int foundStackIndex = stack.find(treeNode->tokens[0].stringVal);

            if (foundStackIndex == -1) { // GLOBAL
                outFile << "LOAD " << treeNode->tokens[0].stringVal << std::endl;
            } else if (foundStackIndex >= 0) { // LOCAL
                outFile << "STACKR " << foundStackIndex << std::endl;
            } else {
                printErrorAndExit(treeNode->tokens[0].stringVal, treeNode->tokens[0].lineNum, false);
            }
        } else if (treeNode->tokens[0].tokenId == NUM_tk) {
            outFile << "LOAD " << treeNode->tokens[0].stringVal << std::endl;
        }

        if (treeNode->ntOne != NULL) { // ( <expr> )
            statSem(treeNode->ntOne, stack, level + 1, outFile);
        }
    }

    if (treeNode->label == "if_nt") {
        statSem(treeNode->ntThree, stack, level + 1, outFile);
        localNewName = getNewName(VAR);
        outFile << "STORE " << localNewName << std::endl;
        stack.pushVarName(localNewName);

        statSem(treeNode->ntOne, stack, level + 1, outFile);
        statSem(treeNode->ntTwo, stack, level + 1, outFile);

        localNewLabel = getNewName(LABEL);

        if (treeNode->ntTwo->tokens[0].stringVal == "==") {
            outFile << "SUB " << localNewName << std::endl;
            outFile << "BRNEG " << localNewLabel << std::endl;
            outFile << "BRPOS " << localNewLabel << std::endl;
        } else if (treeNode->ntTwo->tokens[0].stringVal == "=>") {
            outFile << "SUB " << localNewName << std::endl;
            outFile << "BRNEG " << localNewLabel << std::endl;
        } else if (treeNode->ntTwo->tokens[0].stringVal == "=<") {
            outFile << "SUB " << localNewName << std::endl;
            outFile << "BRPOS " << localNewLabel << std::endl;
        } else if (treeNode->ntTwo->tokens[0].stringVal == "[") {
            outFile << "SUB " << localNewName << std::endl;
            outFile << "BRZERO " << localNewLabel << std::endl;
        } else if (treeNode->ntTwo->tokens[0].stringVal == "%") {
            outFile << "MULT " << localNewName << std::endl;
            outFile << "BRPOS " << localNewLabel << std::endl;
        }

        statSem(treeNode->ntFour, stack, level + 1, outFile);

        outFile << localNewLabel << ":\tNOOP" << std::endl;
    }

    if (treeNode->label == "loop_nt") {
        localNewLabel = getNewName(LABEL);
        outFile << localNewLabel << ": ";

        statSem(treeNode->ntThree, stack, level + 1, outFile);
        localNewName = getNewName(VAR);
        outFile << "STORE " << localNewName << std::endl;
        stack.pushVarName(localNewName);

        statSem(treeNode->ntOne, stack, level + 1, outFile);
        statSem(treeNode->ntTwo, stack, level + 1, outFile);

        localNewLabelTwo = getNewName(LABEL);

        if (treeNode->ntTwo->tokens[0].stringVal == "==") {
            outFile << "SUB " << localNewName << std::endl;
            outFile << "BRNEG " << localNewLabelTwo << std::endl;
            outFile << "BRPOS " << localNewLabelTwo << std::endl;
        } else if (treeNode->ntTwo->tokens[0].stringVal == "=>") {
            outFile << "SUB " << localNewName << std::endl;
            outFile << "BRNEG " << localNewLabelTwo << std::endl;
        } else if (treeNode->ntTwo->tokens[0].stringVal == "=<") {
            outFile << "SUB " << localNewName << std::endl;
            outFile << "BRPOS " << localNewLabelTwo << std::endl;
        } else if (treeNode->ntTwo->tokens[0].stringVal == "[") {
            outFile << "SUB " << localNewName << std::endl;
            outFile << "BRZERO " << localNewLabelTwo << std::endl;
        } else if (treeNode->ntTwo->tokens[0].stringVal == "%") {
            outFile << "MULT " << localNewName << std::endl;
            outFile << "BRPOS " << localNewLabelTwo << std::endl;
        }

        statSem(treeNode->ntFour, stack, level + 1, outFile);

        outFile << "BR " << localNewLabel << std::endl;
        outFile << localNewLabelTwo << ":\tNOOP" << std::endl;
    }

    if (treeNode->label == "assign_nt") {
        if (treeNode->ntOne != NULL) {
            statSem(treeNode->ntOne, stack, level + 1, outFile);
        }

        outFile << "STORE " << treeNode->tokens[1].stringVal << std::endl;
    }

    if (treeNode->label == "R0_nt") {
        processInorder(treeNode, stack, level + 1, outFile);
    }

    if (treeNode->label == "label_nt") {
        outFile << treeNode->tokens[1].stringVal << ": ";
    }

    if (treeNode->label == "goto_nt") {
        outFile << "BR " << treeNode->tokens[1].stringVal << std::endl;
    }

}
