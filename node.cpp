/************************************
 * Author: John Soderstrom
 * Due Date: 5/14/2020
 *
 * Controls nodes storing parse tree information. Does not handle the
 * structure of the tree because that is covered in the parser.
 *
 * The largest number of productions in a single line is 5, from
 * <vars>. The largest number of children in a single line is 4.
 *
 * Printing function starts with a given node (expecting the root)
 * and accesses all children of it. It handles indentation
 * and printing of other information about the node.
 */

#include "node.h"
#include "token.h"
#include <iostream>
#include <stdlib.h>
using namespace std;

/* Constructor for a Node object,
 * Initializes pointers to child nodes for productions to null.
 * Sets the label to the input string, which should be
 * the name of the LHS.
 */
Node::Node(string input)
{
  label = input;
  child1 = NULL;
  child2 = NULL;
  child3 = NULL;
  child4 = NULL;
}

/* Destructor for a Node object, expecting the root node.
 * Recursively calls the destructor for all children.
 */
Node::~Node()
{
  eraseTree(this);
}

/* Recursively deletes all nodes in the tree.
 * Travels down to leaf nodes and deletes before
 * deleting the parents.
 */
void Node::eraseTree(Node *node)
{
  if(node != NULL)
  {
    eraseTree(node->child1);
    eraseTree(node->child2);
    eraseTree(node->child3);
    eraseTree(node->child4);
  }
  delete node;
}

/* Used during traversal.
 * Indents each node based on its depth, then prints the label, or name of the function.
 * Then prints out information on the tokens inside.
 */
void Node::getNodeString(Node *node, int depth)
{
  cout << " ";
  for(int i = 0; i < depth; i++)
  {
    cout << "|-";
  }
  if(depth > 0)
  {
    cout << " ";
  }
  cout << node->label << ", tokens:";
  if(node->token1.tokenString.empty())
  {
    cout << " <none>";
  }
  token tokens[2] = {node->token1, node->token2};
  for(int i = 0; i < 2; i++)
  {
    if(!tokens[i].tokenString.empty())
    {
      cout << " " << tokens[i].tokenString;
    }
  }
  cout << endl;
}

/* Prints node information starting from the node that called this.
 * Expected to run from the root, but will get all children
 * of the starting node.
 */
void Node::printPreorder()
{
  cout << "Parse tree preorder traversal:\n";

  printPreorder(this);
}

/* Recursively travels through the node and its children.
 * Warns if the starting node is empty.
 * Preorder traversal prints all node information before going
 * to the children, starting with the leftmost/first.
 */
void Node::printPreorder(Node *node, int depth)
{
  if(node != NULL)
  {
    // Increments depth by 1 with each call, tracking the actual
    // depth for indenting properly.
    getNodeString(node, depth);
    printPreorder(node->child1, depth + 1);
    printPreorder(node->child2, depth + 1);
    printPreorder(node->child3, depth + 1);
    printPreorder(node->child4, depth + 1);
  }
  // Warns if the starting node is null.
  else if(depth == 0)
  {
    cout << "Warning: Empty tree\n";
  }
}

/* Not part of the Node class, but intimiately related.
 * Dynamically creates a new Node object and returns it.
 */
Node* getNode(string input)
{
  Node* node = new Node(input);
  return node;
}
