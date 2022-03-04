/*************************************
 * Author: John Soderstrom
 * Due Date: 5/14/2020
 *
 * Builds tokens from input, checking against an fsa table.
 * 
 * Requests characters from the scanner and filters
 * unneeded values. When a token's string is complete, builds a
 * token struct using the fsa results.
 *
 * TOKEN_NUM corresponds to the number of tokenId enum values
 * in token.h.
 */

#include "token.h"
#include "scanner.h"
#include "fsa.h"
#include "driver.h"
#include <iostream>
#include <string>
#include <stdlib.h>
using namespace std;

// Stores a list of keyword strings. Potential keyword
// tokens are compared to this array.
static const int KEYWORD_NUM = 13;
static string keywordNames[KEYWORD_NUM] = {"label", "goto", "loop", "void",
					   "declare", "return", "in", "out",
					   "program", "iffy", "then", "assign",
					   "data"};
// Stores a list of all operator strings
static const int OPERATOR_NUM = 19;
static string operatorNames[OPERATOR_NUM] = {":", ":=", "==", "<", ">", "+", "-",
                                             "*", "/", "%", ".", "(", ")", ",", "{", "}",
                                             ";", "[", "]"};

// Stores a reason for a given error using error state as index
static const int ERROR_NUM = 2;
static string errorNames[ERROR_NUM] = {"Alphabet", "Equal"};

// Change with token.h if needed. Stores number of tokenId values.
// Used for building EOF token.
static const int TOKEN_NUM = 35;
static string tokenString = ""; // Stores string to place into token
static int lineNum = 1; // Track line number of input file using newlines
static int columnNum = 0; // Track column number of input text for errors

/* Requests characters from scanner and checks against
 * fsa to build a token's string. When complete, calls
 * a function to build the actual token and returns it
 * to the scanner.
 *
 * If the fsa results in an error, a message is sent to
 * the user and the program exits.
 */
token getToken()
{
  // Initialize an empty string for token, and character/state values.
  int currentChar = filterInput();
  int nextChar = -1;
  int currentState = -1;
  int nextState = 0;

  // Ensures no accidentally setting the state to a large positive
  // value by mistake, not sure how it didn't cause a problem in p1.
  if(currentChar > -1)
  {
    currentState = setState(currentChar);
  }
  // Avoid issues with looking up character after EOF
  // or the next state from an error
  if(currentState > -1)
  {
    nextChar = lookupChar();
    nextState = getNextState(nextChar);
  }

  // Loop continues until EOF is reached.
  while(currentChar > -1)
  {
    // Adds new characters to the string one by one
    tokenString += static_cast<char>(currentChar);

    // If there is an error, send to error handling
    if(currentState < 0)
    {
      handleError(currentState, currentChar);
    }
    else if(nextState < 0)
    {
      columnNum++;
      handleError(nextState, nextChar);
    }

    // 100 is the first token end state.
    // When met, build the complete token and return it.
    if(nextState >= 100)
    {
      // Subtracting 100 from state value corresponds to enum tokenId
      token nToken = buildToken(nextState - 100);
      tokenString = ""; // Reset token string to empty for next token.
      return nToken;
    }

    // Requests next two characters and compares states anew
    currentChar = filterInput();
    // Return current token if we hit EOF, may happen if
    // a comment doesn't end, which will send a warning
    if(currentChar < 0)
    {
      nextState = getNextState(' ');
      token nToken = buildToken(nextState - 100);
      return nToken;
    }
    // If not EOF, continue token
    nextChar = lookupChar();
    currentState = setState(currentChar);
    nextState = getNextState(nextChar);
  }
  
  

  // If file pointer has reached EOF, build a token
  // using EOF_tk from enum token and return it.
  tokenString = "EOF";
  return buildToken(TOKEN_NUM - 1);
}

/* Builds a complete token based on state from fsa and built token string.
 * If the token is an identifier, it will check for a possible
 * keyword before returning the token.
 */
token buildToken(int state)
{
  token nToken;
  
  nToken.id = static_cast<tokenID>(state);
  nToken.tokenString = tokenString;
  nToken.lineNum = lineNum;

  // Only changes token id to keyword if it was an identifier
  // and passes the keyword test function.
  if(nToken.id == IDENT_tk)
  {
    checkKeyword(nToken);
  }
  // If token id was the first operator, check against all of them
  if(nToken.id == COLON_tk)
  {
    checkOperator(nToken);
  }

  return nToken;
}

/* Tests a token's string against keyword array.
 * Changes the token's id to the appropriate token.
 */
void checkKeyword(token &nToken)
{
  for(int i = 0; i < KEYWORD_NUM; i++)
  {
    if(keywordNames[i].compare(nToken.tokenString) == 0)
    {
      nToken.id = (tokenID)((int)LABEL_tk + i);
      return;
    }
  }
}

/* Tests a token's string against operator array.
 * Changes the token's id to the appropriate token.
 */
void checkOperator(token &nToken)
{
  for(int i = 0; i < OPERATOR_NUM; i++)
  {
    if(operatorNames[i].compare(nToken.tokenString) == 0)
    {
      nToken.id = (tokenID)((int)COLON_tk + i);
      return;
    }
  }
}

/* Filters whitespace from requested characters.
 * Checks for # starting or ending comments, ignoring
 * anything in the middle.
 */
int filterInput()
{
  int ch = getChar();
  bool comment = false;
  columnNum++;

  // Requests more characters as long as the last was
  // whitespace, a # marking the start or end of a comment,
  // or as long as it was part of a comment.
  // Nothing in a comment should be checked against the fsa.
  while(isspace(ch) || comment || ch == 35)
  {
    // Increment line number on newline and carriage return.
    if(ch == '\n' || ch == 13)
    {
      lineNum++;
      columnNum = 0;
    }

    // If a # is found while not in a comment, set it to true and loop.
    // If a # is found while in a comment, set it to false and possibly exit loop.
    if(ch == 35)
    {
      comment = !comment;
    }
    columnNum++;
    ch = getChar();

    // If a comment does not end before the file, accept EOF token and
    // warn the user.
    if(ch < 0 && comment)
    {
      comment = false;
      cout << endl << "WARNING: Comment does not end before end of file.\n\n";
    }
  }

  return ch;
}

/* Handle error preparations before printing the error.
 * Will pass the character causing a problem.
 */
void handleError(int state, char ch)
{
  // Set an error code as an index. Changes -1 and down to 0 and up.
  int errorCode = (state * -1) - 1;
  int holdCol = columnNum; // Temporarily store column number
  int counter = 0;

  // Finish token string without care for errors, to print to user.
  // Will end after 6 more characters or on newline/carriage return.
  while(counter < 6 && lookupChar() != '\n' && lookupChar() != 13)
  {
    tokenString += static_cast<char>(getChar());
    counter++;
  }

  // Reset column number, was incremented during token building
  columnNum = holdCol;

  // Token string is complete, column number incremented if
  // error happened on next character instead of current.
  errorExit(errorCode, ch);
}

/* Prints an error message with descriptive information
 * for the user. Includes the token it was trying to build
 * or marks an alphabet error, and prints the line and column number.
 */
void errorExit(int errorCode, char ch)
{
  string errorWord = errorNames[errorCode];
  cout << endl;

  if(errorWord.compare("Alphabet") == 0)
  {
    cout << "SCANNER ERROR: Character '" << ch << "' not in alphabet.\n";
  }
  else if(errorWord.compare("Equal") == 0)
  {
    cout << "SCANNER ERROR: '=' is not a valid token.\n";
  }
  else
  {
    cout << "SCANNER ERROR: Unknown error. User is not expected to see this.\n";
  }
  cout << "     Line: " << lineNum << " Column: " << columnNum;
  cout << " Context: \"" << tokenString << "\"" << endl << endl;

  exit(1);
}
