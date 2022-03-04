/************************************
 * Author: John Soderstrom
 * Due Date: 5/14/2020
 *
 * Handles file reading using file pointer on file or stdin.
 * Given a file pointer, will provide a single character
 * or peek at the next character without consuming it.
 *
 * Intended to work with driver.cpp. The driver will
 * ask for characters to provide the actual token in return.
 */

#include "token.h"
#include "driver.h"
#include "scanner.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

// Stores a single file pointer for character retrieval
// without having to reset it on each call.
static FILE *input = NULL;

/* Sets static file pointer for two functions below
 */
void setInput(FILE *file)
{
  input = file;
  if(input == NULL)
  {
    cout << "Error: input file does not exist or cannot be opened.\n";
    cout << "Usage: scanner [file]\n";
    exit(1);
  }
}

/* Directs driver to create and return a token, then
 * sends the token to its original caller.
 */
token scanToken()
{
  token nToken = getToken();
  return nToken;
}

/* Retrieves and consumes the next character from
 * the input file/file pointer. Returns a negative
 * value on EOF to easily check for the end of file.
 */
int getChar()
{
  int c = fgetc(input);

  if(feof(input))
  {
    c = -1;
  }

  return c;
}

/* Retrieves the next character from input file/file
 * pointer without consuming.
 * Stores the file pointer location before retrieving
 * the character so it is immediately sent back to the
 * character it just consumed.
 */
int lookupChar()
{
  fpos_t pos;
  fgetpos(input, &pos);
  int c = fgetc(input);
  fsetpos(input, &pos);
  return c;
}
