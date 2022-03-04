/****************************
 * Author: John Soderstrom
 * Due Date: 5/14/2020
 *
 * Token structure and tokenID are needed across multiple files
 * Few changes are expected to this file compared to others.
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <string>

// For any changes, check testScanner.cpp to change tokenNames array
// If EOF_tk changes index, change TOKEN_NUM in driver.cpp to match
enum tokenID {IDENT_tk, NUM_tk, COLON_tk, CEQUAL_tk, DEQUAL_tk,
              LESS_tk, GREATER_tk, PLUS_tk, MINUS_tk, TIMES_tk,
              DIVIDE_tk, PERCENT_tk, DOT_tk, OPAREN_tk, CPAREN_tk,
              COMMA_tk, OBRACE_tk, CBRACE_tk, SCOLON_tk, OBRACKET_tk,
              CBRACKET_tk, LABEL_tk, GOTO_tk, LOOP_tk, VOID_tk,
              DECLARE_tk, RETURN_tk, IN_tk, OUT_tk, PROGRAM_tk,
              IFFY_tk, THEN_tk, ASSIGN_tk, DATA_tk, EOF_tk};

struct token
{
  tokenID id;		   // Id has associated strings to identify in testScanner.cpp
  std::string tokenString; // String from file that formed the token
  int lineNum;		   // Line number of string from input file
};

#endif
