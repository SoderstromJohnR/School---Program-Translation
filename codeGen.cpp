/**********************************
 * Author: John Soderstrom
 * Due Date: 5/14/2020
 *
 * Given a valid parse tree, generates .asm code for VirtMach.
 * Handles creation of variable names for labels and temporary variables.
 * Assumes programmers will not use variables in the format of
 * 	T#
 * 	L#
 * and uses these to mark these special program generated variables.
 */

#include "codeGen.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <stdlib.h>
#include <map>
using namespace std;

// I was going to rename all variables for code generation to avoid
// any duplicates with temporary variables, but after the video
// I'll leave them as they are and mark temp variables the other way.
static map<string, int> decTemp;	// Store values for variables when declared.
static ofstream outFile;		// Allows multiple functions easy access to file.

static int labelCount = 0;	// Track number of unique labels
				// Labels cannot be reused in the same way as temp variables

static const int VAR_DEFAULT = 2;
static int varCount = VAR_DEFAULT;	// Track number of unique temporary variables
				// T0 reserved for some <stat>s, set to VAR_DEFAULT to reset
				// Multiple temporary variables are not needed
				// across <expr>'s from different statements
static bool t0out = true;	// Track existance of T0 without searching map

/* Auxiliary function for code generation. Takes the root node of the
 * parse tree and a filename to output code to.
 */
void codeGeneration(Node* root, string filename)
{
  // On file write success, generate code. Else output error.
  outFile.open(filename.c_str());
  if(outFile.is_open())
  {
    recGen(root);
  }
  else
  {
    cout << "Unable to write to " << filename << ".\n";
    cout << "Please check permissions and try again.\n";
    exit(1);
  }

  // Finish off .asm with final keyword and initial values.
  writeFinal();

  if(outFile.is_open())
  {
    outFile.close();
  }
}

/* Generate temporary variable names and labels.
 * Anything that uses expressions will make use of temporary
 * variables T#. Iffy and loop statements use these labels.
 */
string newName(nameType type)
{
  stringstream name;
  if(type == VAR)
  {
    name << "T" << varCount++;

    // Stores any new temporary variable to be initialized
    if(decTemp.find(name.str()) == decTemp.end())
    {
      decTemp.insert(pair<string, int>(name.str(), 0));
    }
  }
  else
  {
    name << "L" << labelCount++;
  }

  return name.str();
}

/* Recursive preorder traversal given a tree. Nodes that 
 * generate code are handled specifically. <expr> and <RO>
 * will be handled in deeper functions.
 */
void recGen(Node* node)
{
  // Return from null nodes before going into other code
  if(!node)
  {
    return;
  }

  // <vars> may have children, so after generating code
  // travel to its child.
  if(node->label.compare("vars") == 0)
  {
    genVars(node);
    recGen(node->child1);
    return;
  }

  // <in> has no children, generate code and return
  else if(node->label.compare("in") == 0)
  {
    genIn(node);
    return;
  }

  // <out> has one child handled in its function. Generate
  // code and return.
  else if(node->label.compare("out") == 0)
  {
    genOut(node);
    return;
  }

  // <iffy> has four children handled in its function.
  // Generate code and return. It will call this function
  // before returning.
  else if(node->label.compare("iffy") == 0)
  {
    genIffy(node);
    return;
  }

  // <loop> has four children handled in its function.
  // Generate code and return. It will call this function
  // before returning.
  else if(node->label.compare("loop") == 0)
  {
    genLoop(node);
    return;
  }

  // <assign> has one child handled in its function.
  // Generate code and return.
  else if(node->label.compare("assign") == 0)
  {
    genAssign(node);
    return;
  }

  // <label> has no children. Generate code and return.
  else if(node->label.compare("label") == 0)
  {
    genLabel(node);
    return;
  }

  // <goto> has no children. Generate code and return.
  else if(node->label.compare("goto") == 0)
  {
    genGoto(node);
    return;
  }

  // Generic preorder traversal for non-code generating nodes.
  else
  {
    recGen(node->child1);
    recGen(node->child2);
    recGen(node->child3);
    recGen(node->child4);
  }
}

/* Add declared variable name and initial value to a map.
 * After the traversal is complete, all declarations
 * will be added to the end of file with initial values.
 */
void genVars(Node* node)
{
  string name = node->token1.tokenString;
  int val = atoi(node->token2.tokenString.c_str());
  decTemp.insert(pair<string, int>(name, val));
}

/* Take user input and store into an argument.
 */
void genIn(Node* node)
{
  outFile << "READ " << node->token1.tokenString << endl;
}

/* Use a temp variable for the value from the expression.
 * Always uses the same one to save on space.
 * Stores the value and outputs it to the user.
 * Ends by resetting the temp variable count to reuse names.
 */
void genOut(Node* node)
{
  string temp = "T0";
  if(t0out)
  {
    decTemp.insert(pair<string, int>(temp, 0));
    t0out = false;
  }
  genExpr(node->child1);
  outFile << "STORE " << temp << endl;
  outFile << "WRITE " << temp << endl;
  varCount = VAR_DEFAULT;
}

/* Use a temp variable for first expression and set a label.
 * Subtracts result of one expression from another to
 * compare in <RO>. Recursively calls recGen to write
 * statements before setting a label to skip to.
 */
void genIffy(Node* node)
{
  string label = newName(LABEL);
  string temp = "T0";
  if(t0out)
  {
    decTemp.insert(pair<string, int>(temp, 0));
    t0out = false;
  }
  genExpr(node->child3);
  outFile << "STORE " << temp << endl;
  genExpr(node->child1);
  outFile << "SUB " << temp << endl;
  genRO(node->child2, label);
  // Takes the place of going into a <stat>
  recGen(node->child4);
  outFile << label << ": NOOP\n";
  varCount = VAR_DEFAULT;
}

/* Set up two labels for start and end of loop. Uses a
 * temp variable for first expression. Subtracts result of one
 * from another to compare in <RO>. Recursively calls recGen to
 * write statements before setting the exit label. Just before
 * the exit label, goto the loop label always to test
 * expression results again.
 * Warning: modify expressions inside statements or risk
 * an infinite loop.
 */
void genLoop(Node* node)
{
  string loopLabel = newName(LABEL);
  string exitLabel = newName(LABEL);
  //string temp = newName(VAR);
  string temp = "T0";
  if(t0out)
  {
    decTemp.insert(pair<string, int>(temp, 0));
    t0out = false;
  }

  outFile << loopLabel << ": NOOP\n";
  genExpr(node->child3);
  outFile << "STORE " << temp << endl;
  genExpr(node->child1);
  outFile << "SUB " << temp << endl;
  genRO(node->child2, exitLabel);
  // Takes the place of going into a <stat>
  recGen(node->child4);
  outFile << "BR " << loopLabel << endl;
  outFile << exitLabel << ": NOOP\n";
  varCount = VAR_DEFAULT;
}

/* Add branching instructions based on relational operators.
 * Expects the result of the second expression in a comparison
 * to be subtracted from the first.
 * If the first expression is larger, the value left is positive.
 * If the first expression is smaller, the value left is negative.
 * If they are equal, the value left is 0.
 * Because these labels skip the iffy and loop statements, the
 * branching instruction should act on the opposite condition.
 * So for "<<" which does the statement on less than or equal,
 * only skip when a positive value remains.
 */
void genRO(Node* node, string label)
{
  if(node->token1.tokenString.compare("<") == 0)
  {
    // "<<" less than or equal to
    if(node->token2.tokenString.compare("<") == 0)
    {
      outFile << "BRPOS " << label << endl;
    }
    // "<>" not equal to
    else if(node->token2.tokenString.compare(">") == 0)
    {
      outFile << "BRZERO " << label << endl;
    }
    // "<" less than
    else
    {
      outFile << "BRZPOS " << label << endl;
    }
  }
  else if(node->token1.tokenString.compare(">") == 0)
  {
    // ">>" greater than or equal to
    if(node->token2.tokenString.compare(">") == 0)
    {
      outFile << "BRNEG " << label << endl;
    }
    // ">" greater than
    else
    {
      outFile << "BRZNEG " << label << endl;
    }
  }
  // "==" equal to
  else
  {
    outFile << "BRNEG " << label << endl;
    outFile << "BRPOS " << label << endl;
  }
}

/* Value from expression does not need to be saved to a temporary
 * variable, the variable we want it saved to is given.
 */
void genAssign(Node* node)
{
  genExpr(node->child1);
  outFile << "STORE " << node->token1.tokenString << endl;
}

/* Set up a label with no actual instruction, for goto statements.
 */
void genLabel(Node* node)
{
  outFile << node->token1.tokenString << ": NOOP\n";
}

/* Goto a label under all conditions, no check needed.
 */
void genGoto(Node* node)
{
  outFile << "BR " << node->token1.tokenString << endl;
}

/* With only one child, simply call the child's function.
 * With two children, store right side in a temporary
 * variable and subtract from result of left side with it.
 * Does not result variable counter because an unknown
 * number of them are needed.
 */
void genExpr(Node* node)
{
  if(node->child2)
  {
    genExpr(node->child2);
    string temp = newName(VAR);
    outFile << "STORE " << temp << endl;
    genN(node->child1);
    outFile << "SUB " << temp << endl;
  }
  else
  {
    genN(node->child1);
  }
}

/* With one child, just call child's function.
 * With either other option, store result of right side
 * in a temporary variable, then divide or multiply - 
 * modify - result of the left side with it.
 * As with <expr>, unknown number of temporary variables
 * are needed so they are not reset.
 */
void genN(Node* node)
{
  if(node->token1.tokenString.compare("/") == 0)
  {
    genN(node->child2);
    string temp = newName(VAR);
    outFile << "STORE " << temp << endl;
    genA(node->child1);
    outFile << "DIV " << temp << endl;
  }
  else if(node->token1.tokenString.compare("*") == 0)
  {
    genN(node->child2);
    string temp = newName(VAR);
    outFile << "STORE " << temp << endl;
    genA(node->child1);
    outFile << "MULT " << temp << endl;
  }
  else
  {
    genA(node->child1);
  }
}

/* Repeat comments for <expr> and <N>.
 * Use addition instead of other operators.
 */
void genA(Node* node)
{
  if(node->child2)
  {
    genA(node->child2);
    string temp = newName(VAR);
    outFile << "STORE " << temp << endl;
    genM(node->child1);
    outFile << "ADD " << temp << endl;
  }
  else
  {
    genM(node->child1);
  }
}

/* If a token exists, negate (multiply by -1) the value
 * and recursively call on the child. Otherwise,
 * call the child's function.
 */
void genM(Node* node)
{
  if(node->token1.tokenString.compare("*") == 0)
  {
    genM(node->child1);
    outFile << "MULT -1\n";
  }
  else
  {
    genR(node->child1);
  }
}

/* If a child exists, recursively call <expr> to repeat the
 * cycle. If no child exists, load the integer or variable
 * into the accumulator.
 */
void genR(Node* node)
{
  if(node->child1)
  {
    genExpr(node->child1);
  }
  else
  {
    outFile << "LOAD " << node->token1.tokenString << endl;
  }
}

/* Add the final STOP to .asm. Loop through all declared
 * and temporary variables with initial values and print
 * to the end of the file.
 */
void writeFinal()
{
  outFile << "STOP\n";
  map<string, int>::iterator it = decTemp.begin();
  while(it != decTemp.end())
  {
    outFile << it->first << " " << it->second << endl;
    it++;
  }
}
