/*************************************
 * Author: John Soderstrom
 * Due Date: 5/14/2020
 *
 * Stores finite state automata in 2d array form.
 *
 * Sends state information to the driver and
 * sets state based on the current character in the driver.
 */

#include "fsa.h"
using namespace std;

// Constants for fsa and states grouped
// First 2 values are used in the size of the fsa
static const int ASCII_MAX = 128;
static const int MAX_STATE = 6;

// Driver will end on an error returned from here
// The state will not be set to these values outside of array
static const int EQ_ERROR = -2;
static const int ALPHA_ERROR = -1;

// state 0 is the initial state
// Following states are for tokens during building
static const int IDENTIFIER = 1;
static const int INTEGER = 2;

// Most operators are grouped into OPERATOR and single character
// Colon can be part of a single or double character while
// Equal sign cannot be a single character operator.
static const int OPERATOR = 3;
static const int COLON = 4;
static const int EQUAL = 5;

// States 100+ denote complete tokens
static const int ID_TK = 100;
static const int INT_TK = 101;
static const int OP_TK = 102;

// Build fsa table/2d array
static int fsa[ASCII_MAX][MAX_STATE] = {};
static bool uninitialized = true;

// Track current type of token
static int state = 0;

/* Initialize values in FSA using above static constants
 * Static constant names represent each state
 */
void initFSA()
{
  //Set alphabet error values first to partially overwrite in later steps
  for(int i = 0; i < ASCII_MAX; i++)
  {
    for(int j = 0; j < MAX_STATE; j++)
    {
      if(j == EQUAL)
      {
        fsa[i][j] = EQ_ERROR;
      }
      else
      {
        fsa[i][j] = ALPHA_ERROR;
      }
    }
  }

  //Set state values for each group of tokens
  initLetter();
  initNum();
  initOps();
  initWS();
  initComment();
}

/* Initialize states for incoming letters in fsa
 * Sets for every letter in one loop
 */
void initLetter()
{
  //Letters (A-Z, values 65-90, 
  //         a-z, values 97-122)
  for(int i = 65; i < 123; i++)
  {
    //Skip values 91-96, they are not letters
    //Allows us to keep changes for all letters in one place
    if(i > 90 && i < 97)
    {
      continue;
    }
    fsa[i][0] = IDENTIFIER;
    fsa[i][IDENTIFIER] = IDENTIFIER;
    fsa[i][INTEGER] = ID_TK;
    fsa[i][OPERATOR] = OP_TK;
    fsa[i][COLON] = OP_TK;
    fsa[i][EQUAL] = EQ_ERROR;
  }
}

/* Initialize states for incoming numbers in fsa
 */
void initNum()
{
  //Digits (0-9, values 48-57)
  for(int i = 48; i < 58; i++)
  {
    fsa[i][0] = INTEGER;
    fsa[i][IDENTIFIER] = IDENTIFIER;
    fsa[i][INTEGER] = INTEGER;
    fsa[i][OPERATOR] = OP_TK;
    fsa[i][COLON] = OP_TK;
    fsa[i][EQUAL] = EQ_ERROR;
  }
}

/* Initialize states for incoming operators and delimiters in fsa
 */
void initOps()
{
  //Operators ':', '=' (values 58, 61)
  //Valid operator tokens are ":", ":=", and "==", not "="
  fsa[58][0] = COLON;
  fsa[58][IDENTIFIER] = ID_TK;
  fsa[58][INTEGER] = INT_TK;
  fsa[58][OPERATOR] = OP_TK;
  fsa[58][COLON] = OP_TK;
  fsa[58][EQUAL] = EQ_ERROR;

  fsa[61][0] = EQUAL;
  fsa[61][IDENTIFIER] = ID_TK;
  fsa[61][INTEGER] = INT_TK;
  fsa[61][OPERATOR] = OP_TK;
  fsa[61][COLON] = OPERATOR;
  fsa[61][EQUAL] = OPERATOR;

  //Operators and  '<', '>', '+', '-', '*', '/', '%', '.', '(', ')', ',',
  //Delimiters     '{', '}', ';', '[', ']'
  //        (values 60, 62, 43, 45, 42, 47, 37, 46, 40, 41, 44, 123, 125, 59, 91, 93)
  //        (sorted 37, 40-47, 59-60, 62, 91, 93, 123, 125)
  //Only single characters of above operators are accepted.
  const int SIZE = 16;
  int opArr[SIZE] = {37, 40, 41, 42, 43, 44, 45, 46, 47, 59, 60, 62, 91, 93, 123, 125};

  for(int i = 0; i < SIZE; i++)
  {
    fsa[opArr[i]][0] = OPERATOR;
    fsa[opArr[i]][IDENTIFIER] = ID_TK;
    fsa[opArr[i]][INTEGER] = INT_TK;
    fsa[opArr[i]][OPERATOR] = OP_TK;
    fsa[opArr[i]][COLON] = OP_TK;
    fsa[opArr[i]][EQUAL] = EQ_ERROR;
  }
}

/* Initialize states for incoming whitespace characters in fsa.
 * There is no whitespace state, they are only expected
 * to show in lookup/lookahead characters that do not change state.
 */
void initWS()
{
  //Whitespace (Values 9, 10, 11, 12, 13, 32)
  //           (tab, newline, vertical tab, feed, carriage return, space) 
  const int SIZE = 6;
  int wsArr[SIZE] = {9, 10, 11, 12, 13, 32};

  for(int i = 0; i < SIZE; i++)
  {
    fsa[wsArr[i]][0] = 0;
    fsa[wsArr[i]][IDENTIFIER] = ID_TK;
    fsa[wsArr[i]][INTEGER] = INT_TK;
    fsa[wsArr[i]][OPERATOR] = OP_TK;
    fsa[wsArr[i]][COLON] = OP_TK;
    fsa[wsArr[i]][EQUAL] = EQ_ERROR;
  }
}

/* Allow a comment without breaking anything next to it.
 * Only expects to come from lookup/lookahead characters.
 */
void initComment()
{
  int comm = '#';
  fsa[comm][0] = 0;
  fsa[comm][IDENTIFIER] = ID_TK;
  fsa[comm][INTEGER] = INT_TK;
  fsa[comm][OPERATOR] = OP_TK;
  fsa[comm][COLON] = OP_TK;
  fsa[comm][EQUAL] = EQ_ERROR;
}

/* Initialize fsa if it has not already been.
 * Sets the state to its new state, which should
 * be the very first one for the token or seen already
 * from the lookahead character.
 */
int setState(int c)
{
  if(uninitialized)
  {
    uninitialized = false;
    initFSA();
  }
  state = fsa[c][state];
  return state;
}

/* Looks at what the next state would be for a lookahead character.
 * Will not change the state unless the current character
 * ends the token. Will then reset state to 0.
 */
int getNextState(int c)
{
  int next = fsa[c][state];
  if(next >= ID_TK)
  {
    state = 0;
  }

  return next;
}
