/***************************
 * Author: John Soderstrom
 * Due Date: 5/14/2020
 *
 * Stores function declarations for the scanner.
 * Requires token structure from token.h and
 * the File pointer type.
 */

#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include "token.h"

void setInput(FILE *);
token scanToken();
int getChar();
int lookupChar();

#endif
