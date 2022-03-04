/*******************************
 * Author: John Soderstrom
 * Due Date: 5/14/2020
 *
 * Handles testing semantics of a complete parse tree.
 * For purposes of this assignment, only declaration of
 * and usage of declared variables is tested. The auxiliary
 * function returns true or false for a successful or
 * failed semantics test, allowing indication of a passed
 * test and an easy check before moving to code generation.
 */

#include "node.h"
#include "semantics.h"
#include <string>
#include <iostream>
#include <map>
using namespace std;

static map<string, int> symbols;	// Store variable strings and line numbers
static bool passedSemantics = true;	// Sets to false on any error and returns

/****************
 * Auxiliary function. Pass the root node of a tree
 * and it will return true if the semantics test passes,
 * false if it fails.
 */
bool checkSemantics(Node* root)
{
  checkSemanticsNode(root);
  return passedSemantics;
}

/****************
 * Recursively traverses tree as a preorder traversal.
 * Skips over null pointers, and checks each node only
 * if the first token contains something.
 *
 * Syntax tokens have been removed from the tree, so only
 * identifiers and integers/numbers will be considered.
 */
void checkSemanticsNode(Node* node)
{
  if(node)
  {
    if(node->token1.tokenString.compare("") != 0)
    {
      checkNode(node);
    }

    checkSemanticsNode(node->child1);
    checkSemanticsNode(node->child2);
    checkSemanticsNode(node->child3);
    checkSemanticsNode(node->child4);
  }
}

/****************
 * Tests a node with at least one token. Checks <vars> nodes
 * for declaring variables, and any nodes that may contain
 * an identifier to ensure they have already been declared.
 * Such nodes are <in> <assign> <label> <goto> <R>.
 * <stat> does not contain any tokens as identifiers are further
 * down in the tree. <expr> does not contain identifiers directly,
 * they are also further down in <R>.
 */
void checkNode(Node* node)
{
  // Make just one function call for testing if variables are declared
  // by setting this boolean.
  bool statSuccess = true;

  // Test <vars> node for declaring an undeclared variable.
  if(node->label.compare("vars") == 0)
  {
    // If variable is already declared, print an error.
    if(!insertIdent(node))
    {
      declareError(node);
    }
  }

  // Test other nodes for using a declared variable.
  else if(node->label.compare("in") == 0)
  {
    statSuccess = verifyIdent(node);
  }
  else if(node->label.compare("assign") == 0)
  {
    statSuccess = verifyIdent(node);
  }
  else if(node->label.compare("label") == 0)
  {
    statSuccess = verifyIdent(node);
  }
  else if(node->label.compare("goto") == 0)
  {
    statSuccess = verifyIdent(node);
  }
  else if(node->label.compare("R") == 0)
  {
    if(node->token1.id == IDENT_tk)
    {
      statSuccess = verifyIdent(node);
    }
  }

  // If an undeclared variable is used, print an error.
  if(!statSuccess)
  {
    statError(node);
  }
}

/*****************
 * Add a declared variable to the map structure with a line number.
 * If the variable is already declared, skip and return false for
 * an error.
 */
bool insertIdent(Node* node)
{
  string tok = node->token1.tokenString;
  if(symbols.find(tok) == symbols.end())
  {
    // Variable names act as a key, line numbers a value.
    // This allows printing the line number of the original
    // declaration.
    int line = node->token1.lineNum;
    symbols.insert(pair<string, int>(tok, line));
    return true;
  }
  else
  {
    return false;
  }
}

/****************
 * Check map structure for a given variable.
 * If it does not exist, return false for failure.
 */
bool verifyIdent(Node* node)
{
  string tok = node->token1.tokenString;
  if(symbols.find(tok) != symbols.end())
  {
    return true;
  }
  else
  {
    return false;
  }
}

/***************
 * Flags tree as failing the semantics check.
 * Given a node that declares a previously declared variable,
 * retrieve the original line number.
 * Prints the line number of the previous declaration and
 * the new declaration, along with the variable's name or string.
 */
void declareError(Node* node)
{
  passedSemantics = false;
  string tok = node->token1.tokenString;
  int line = node->token1.lineNum;
  int origLine = symbols.find(tok)->second;
  cout << "SEMANTICS ERROR: Identifier '" << tok << "' on line " << line << " was already declared.\n";
  cout << "  Original declaration of '" << tok << "' occurs on line " << origLine << ".\n";
}

/**************
 * Flags tree as failing the semantics check.
 * Given a node that contains an undeclared variable,
 * print the name and line number of the variable.
 */
void statError(Node* node)
{
  passedSemantics = false;
  string tok = node->token1.tokenString;
  int line = node->token1.lineNum;
  cout << "SEMANTICS ERROR: Identifier '" << tok << "' on line " << line << " has not been delcared.\n";
}
