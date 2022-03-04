/*******************************
 * Author: John Soderstrom
 * Due Date: 5/14/2020
 *
 * Usage:
 * comp [file]
 *
 * Scans a file as part of the compilation process.
 * Whitespace is not required to separate tokens.
 *
 * Characters in the file are grouped into tokens and marked
 * with an identifier. Each token includes the id, the string
 * associated with it from the file, and the line number it
 * appeared in.
 *
 * Tokens are then passed to the parser, which builds the parse tree.
 *
 * Parser will return the root node of the complete tree.
 *
 * Passing the root node into the semantics test will return success
 * or fail. Failed tests will print errors as they occur, but
 * a passed test gives no indication. Returning a boolean allows
 * main to output success or failure and must pass to
 * move on to code generation.
 */

#include "token.h"
#include "parser.h"
#include "scanner.h"
#include "lib.h"
#include "node.h"
#include "semantics.h"
#include "codeGen.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

int main(int argc, char *argv[])
{
  // Set file pointer for input and send to scanner
  // and pull the filename or set a generic one.
  string filename = "";
  FILE *input = handleArgs(argc, argv, filename);
  setInput(input);
  filename += ".asm";

  // Get root node for a parse tree.
  Node* root = parser();
  // Test for success or failure on semantics and output to user.
  bool testSem = checkSemantics(root);
  // Generate code on success and output success message.
  // Allows program to end without comment if there are semantics errors.
  if(testSem)
  {
    codeGeneration(root, filename);
    cout << filename << " generated.\n";
  }

  // Close file if file pointer sees it
  if(input != NULL)
  {
    fclose(input);
    input = NULL;
  }
  
  return 0;
}

/* Accepts command line arguments and handles changes in program accordingly.
 * No more than one extra argument is allowed.
 * If the file has the implicit extension, it will be stripped here.
 * Returns a file pointer to the file or stdin for funneled input.
 * Provides a generic filename if none was provided.
 */
FILE *handleArgs(int argc, char* argv[], string &filename)
{
  // Sets a file pointer, either for stdin or a file
  FILE* input = NULL;

  if(argc == 2)
  {
    // If an argument was passed, it is expected to be a filename.
    // Check for the implicit file extension. If it is not present, add it.
    filename = argv[1];
    string extension = ".sp2020";
    string fullFile = "";

    bool check1 = filename.length() > extension.length();
    bool check2 = true;
    if(check1)
    {
      check2 = filename.compare(filename.length() - extension.length(),
        extension.length(), extension) == 0;
    }
    if(!check1 || !check2)
    {
      fullFile = filename + extension;
    }
    else
    {
      fullFile = filename;
      filename = filename.substr(0, filename.length() - extension.length());
    }
    // Directs file pointer to the input file
    input = fopen(fullFile.c_str(), "r");

    if(input == NULL)
    {
      cout << "Unable to open file " << fullFile << endl;
      cout << "Usage: comp [file]" << endl;
      exit(1);
    }
  }

  // If no arguments were passed, handle input funneled from a file.
  // Sets file pointer to standard input
  else if(argc == 1)
  {
    filename = "kb";
    input = stdin;
    cout << "Expecting funneled input.\n";
    cout << "If none, strange behavior may result.\n";
    cout << "Press ctrl+d to simulate end of file.\n";
    cout << "In an emergency, press ctrl+c to terminate program.\n";
  }
  
  // If more than one argument was passed, output an error with usage message, then exit.
  else
  {
    cout << "Error: Unexpected number of arguments.\n";
    cout << "usage: comp [file]\n";
    exit(1);
  }

  // Test for empty file. If the file is not empty, move the file pointer back to
  // the first character.
  fgetc(input);
  if(feof(input))
  {
    cout << "Empty file\n";
    fclose(input);
    exit(1);
  }
  rewind(input);

  return input;
}
