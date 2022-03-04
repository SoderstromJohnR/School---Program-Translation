/****************************
 * Author: John Soderstrom
 * Due Date: 5/14/2020
 *
 * Provides all needed functionality for codeGen.cpp.
 */

#ifndef CODEGEN_H
#define CODEGEN_H

#include "node.h"
#include <string>

typedef enum {VAR, LABEL} nameType;

void codeGeneration(Node*, std::string);
std::string newName(nameType);
void recGen(Node*);
void genVars(Node*);
void genIn(Node*);
void genOut(Node*);
void genIffy(Node*);
void genLoop(Node*);
void genRO(Node*, std::string);
void genAssign(Node*);
void genLabel(Node*);
void genGoto(Node*);
void genExpr(Node*);
void genN(Node*);
void genA(Node*);
void genM(Node*);
void genR(Node*);

void writeFinal();

#endif
