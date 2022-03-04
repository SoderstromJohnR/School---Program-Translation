/***********************************
 * Author: John Soderstrom
 * Due Date: 5/14/2020
 *
 * Declares functions needed for semantics.cpp.
 * Includes node.h to allow for Node and Node pointers.
 */

#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "node.h"

bool checkSemantics(Node*);
void checkSemanticsNode(Node*);
void checkNode(Node*);
bool insertIdent(Node*);
bool verifyIdent(Node*);
void declareError(Node*);
void statError(Node*);

#endif
