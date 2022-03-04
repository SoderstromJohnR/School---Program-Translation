/***************************
 * Author: John Soderstrom
 * Due Date: 5/14/2020
 *
 * Contains structure of the class Node
 * and function declarations inside it.
 *
 * Also contains a function declaration not of the class,
 * but intimately connected to it.
 */

#ifndef NODE_H
#define NODE_H

#include "token.h"
#include <string>

class Node
{
  public:
    Node(std::string);
    ~Node();

    std::string label;
    token token1;
    token token2;
    //token token3;
    //token token4;
    //token token5;
    Node* child1;
    Node* child2;
    Node* child3;
    Node* child4;

    Node* getNode(std::string);
    void printPreorder();

  private:
    void eraseTree(Node*);

    void getNodeString(Node*, int);
    void printPreorder(Node*, int = 0);
};

Node* getNode(std::string);

#endif
