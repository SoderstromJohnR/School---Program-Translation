/*******************************
 * Author: John Soderstrom
 * Due Date: 5/14/2020
 *
 * Contains all functions for creating a parse tree,
 * and the related error messages.
 */

#ifndef PARSER_H
#define PARSER_H

#include "node.h"
#include <string>

Node* parser();
Node* program();

Node* vars();
Node* block();

Node* stats();
Node* mStat();
Node* stat();

Node* in();
Node* out();
Node* iffy();
Node* loop();
Node* assign();
Node* label();
Node* goto_();

Node* RO();
Node* expr();

Node* N();
Node* A();
Node* M();
Node* R();

void errorParse(std::string, std::string);

#endif
