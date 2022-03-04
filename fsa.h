/*******************************
 * Author: John Soderstrom
 * Due Date: 5/14/2020
 *
 * Stores function declarations for the fsa
 *  (finite state automata, 2d array)
 */

#ifndef FSA_H
#define FSA_H

void initFSA();
void initLetter();
void initNum();
void initOps();
void initWS();
void initComment();

int setState(int);
int getNextState(int);

#endif
