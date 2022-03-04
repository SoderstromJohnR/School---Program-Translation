/****************************
 * Author: John Soderstrom
 * Due Date: 5/14/2020
 *
 * Stores function declartions for the driver.
 * Requires the token structure from token.h
 */

#ifndef DRIVER_H
#define DRIVER_H

#include "token.h"

token getToken();
token buildToken(int);
void checkKeyword(token &);
void checkOperator(token &);

int filterInput();

void handleError(int, char);
void errorExit(int, char);

#endif
