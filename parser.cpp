/********************************
 * Author: John Soderstrom
 * Due Date: 5/14/2020
 *
 * Contains all functions related to creating the parse tree
 * and the error messages.
 *
 * All functions other than the error message return a pointer
 * to a Node object. The parser function creates and returns
 * the root node that is used to access the rest of the tree.
 */

#include "token.h"
#include "scanner.h"
#include "parser.h"
#include "node.h"
#include <string>
#include <iostream>
#include <stdlib.h>
using namespace std;

// Stores a token object used across functions.
// Several of them will need to access the next token
// or the one left from the previous function.
static token tk;

/* Begins creating the parse tree. Creates the root node
 * and returns once the tree is finished. If the tree is
 * somehow finished without ending on the end of file token,
 * the program will error and exit.
 */
Node* parser()
{ 
  // Initializes token tk with the first token, and the root node.
  tk = scanToken();
  Node* root = program();

  // Returns the parse tree if the program was successful and
  // finished on the end of file.
  if(tk.id == EOF_tk)
  {
    return root;
  }
  else
  {
    errorParse(root->label, "End of File");
  }
}

/* FIRST(program) = FIRST(vars) = {DECLARE_tk, empty} U {OBRACE_tk}
 * Because empty is in the set, union with...
 * FIRST(block) = {OBRACE_tk}
 * The only choice is to go into vars and then block
 * DECLARE_tk is "declare"
 * OBRACE_tk is "{"
 */
Node* program()
{
  // Generates a new node labeled with the function,
  // and sets the children to vars and block appropriately.
  Node* node = getNode("program");
  node->child1 = vars();

  // vars can be empty, so block may end up as child 1 instead of 2.
  if(node->child1)
  {
    node->child2 = block();
  }
  else
  {
    node->child1 = block();
  }
  return node;
}

/* FIRST(vars) = {DECLARE_tk, empty}
 * DECLARE_tk is "declare"
 */
Node* vars()
{
  // Moved node creation out of switch structure because it errored inside
  Node* node = getNode("vars");
  switch(tk.id)
  {
  case DECLARE_tk:
    //node->token1 = tk;
    tk = scanToken();

    switch(tk.id)
    {
    case IDENT_tk:
      node->token1 = tk;
      tk = scanToken();

      switch(tk.id)
      {
      case CEQUAL_tk:
        //node->token3 = tk;
        tk = scanToken();

        switch(tk.id)
        {
        case NUM_tk:
          node->token2 = tk;
          tk = scanToken();

          switch(tk.id)
          {
          // At this point, 5 tokens have been generated successfully
          // declare IDENTIFIER := INTEGER ;
          // Now we check for a "vars" node before returning
          case SCOLON_tk:
            //node->token5 = tk;
            tk = scanToken();
            node->child1 = vars();
            return node;

          default:
            errorParse(node->label, ";");
          }

        default:
          errorParse(node->label, "INTEGER");
        }

      default:
        errorParse(node->label, ":=");
      }
    default:
      errorParse(node->label, "IDENTIFIER");
    }

  // If empty, return a null pointer instead of a node
  default:
    return NULL;
  }
  
}

/* FIRST(block) = {OBRACE_tk}
 * OBRACE_TK is "{"
 */
Node* block()
{
  Node* node = getNode("block");
  switch(tk.id)
  {
  // As in FIRST set, can only beging with an open brace
  case OBRACE_tk:
    //node->token1 = tk;
    tk = scanToken();
    node->child1 = vars();

    // As in program(), vars may be empty, which would
    // make stats the first child instead.
    if(node->child1)
    {
      node->child2 = stats();
    }
    else
    {
      node->child1 = stats();
    }

    // Expects a closing brace to finish the block
    switch(tk.id)
    {
    case CBRACE_tk:
      //node->token2 = tk;
      tk = scanToken();
      return node;

    default:
      errorParse(node->label, "}");
    }

  default:
    errorParse(node->label, "{");
  }
}

/* FIRST(stats) = FIRST(stat) = FIRST(in, out, block, loop, assign, goto, label) =
 *  = {IN_tk, OUT_tk, LBRACE_tk, LOOP_tk, IDENT_tk, GOTO_tk, LABEL_tk}, disjointed
 * Meanings are in comments for related functions
 * As with program(), errors are handled with functions further in
 */
Node* stats()
{
  Node* node = getNode("stats");
  // stat should not be empty, so no need to check if child1 is null
  node->child1 = stat();
  node->child2 = mStat();
  return node;
}

/* FIRST(mStat) = FIRST(stat) as above function, and empty
 * After all statements are filled, the next token will be CBRACE_tk }
 * which finishes a block, and lets us know mStat is empty.
 */
Node* mStat()
{
  Node* node = getNode("mStat");
  switch(tk.id)
  {
  // Closing brace marks end of the block, and mStat is empty
  // To put another way, FOLLOW(block) = {CBRACE_tk}
  // CBRACE_tk is "}"
  case CBRACE_tk:
    return NULL;

  // Special check for if the next token is in FIRST(stat)
  // If not, it was probably  meant to close the block instead
  case IN_tk:
  case OUT_tk:
  case OBRACE_tk:
  case LOOP_tk:
  case IDENT_tk:
  case GOTO_tk:
  case LABEL_tk:
  case IFFY_tk:
    node->child1 = stat();
    node->child2 = mStat();
    return node;

  default:
    errorParse("block", "}");
  }
}

/* FIRST(stat) = FIRST(in, out, block, loop, assign, goto, label, iffy) =
 *  = {IN_tk, OUT_tk, OBRACE_tk, LOOP_tk, IDENT_tk, GOTO_tk, LABEL_tk, IFFY_tk}, disjointed
 * Meanings are in comments for related functions
 * As with program(), errors are handled with functions further in
 * when tokens are actually put into nodes.
 * Errors return the child's label for more obvious information about
 * the statements causing errors.
 */
Node* stat()
{
  Node* node = getNode("stat");
  switch(tk.id)
  {
  // (get from function) in ...
  case IN_tk:
    node->child1 = in();
    
    switch(tk.id)
    {
    // in IDENTIFIER ;
    case SCOLON_tk:
      //node->token1 = tk;
      tk = scanToken();
      return node;

    default:
      errorParse(node->child1->label, ";");
    }
  // (get from function) out ...
  case OUT_tk:
    node->child1 = out();

    switch(tk.id)
    {
    // out <expr> ;
    case SCOLON_tk:
      //node->token1 = tk;
      tk = scanToken();
      return node;

    default:
      errorParse(node->child1->label, ";");
    }
  case OBRACE_tk:
    node->child1 = block();
    return node;
     
  // (get from function) iffy ... 
  case IFFY_tk:
    node->child1 = iffy();

    switch(tk.id)
    {
    // iffy [ <expr> <RO> <expr> ] then <stat> ;
    case SCOLON_tk:
      //node->token1 = tk;
      tk = scanToken();
      return node;

    default:
      errorParse(node->child1->label, ";");
    }
  // (get from function) loop ...
  case LOOP_tk:
    node->child1 = loop();

    switch(tk.id)
    {
    // loop [ <expr> <RO> <expr> ] <stat> ;
    case SCOLON_tk:
      //node->token1 = tk;
      tk = scanToken();
      return node;

    default:
      errorParse(node->child1->label, ";");
    }
  // (get from function) IDENTIFIER ... 
  case IDENT_tk:
    node->child1 = assign();

    switch(tk.id)
    {
    // IDENTIFIER := <expr> ;
    case SCOLON_tk:
      //node->token1 = tk;
      tk = scanToken();
      return node;

    default:
      errorParse(node->child1->label, ";");
    }
  // (get from function) label ...
  case LABEL_tk:
    node->child1 = label();

    switch(tk.id)
    {
    // label IDENTIFIER ;
    case SCOLON_tk:
      //node->token1 = tk;
      tk = scanToken();
      return node;

    default:
      errorParse(node->child1->label, ";");
    }
  // (get from function) goto ...
  case GOTO_tk:
    node->child1 = goto_();
    
    switch(tk.id)
    {
    // goto IDENTIFIER ;
    case SCOLON_tk:
      //node->token1 = tk;
      tk = scanToken();
      return node;

    default:
      // Get the label of the child to better specify
      // what kind of code started the error
      // This error will not come up if it leads to a block
      // because a block needs no tokens in this node. Instead,
      // it would get the error of the block or a statement inside it.
      errorParse(node->child1->label, ";");
    }

  // Invalid token
  default:
    string expected = "BLOCK, IDENTIFIER, or KEYWORD.\n";
    expected += "  Keywords: in, out, iffy, loop, goto, label";
    errorParse(node->label, expected);
  }
}

/* FIRST(in) = {IN_tk}
 * IN_tk is "in"
 * Do not need to check the first token since it was checked in stat()
 */
Node* in()
{
  Node* node = getNode("in");
  // Token has already been checked in stat()
  //node->token1 = tk;
  tk = scanToken();

  switch(tk.id)
  {
  case IDENT_tk:
    node->token1 = tk;
    tk = scanToken();
    return node;

  default:
    errorParse(node->label, "IDENTIFIER");
  }
}

/* FIRST(out) = {OUT_tk}
 * OUT_tk is "out"
 * Do not need to check the first token since it was checked in stat()
 */
Node* out()
{
  Node* node = getNode("out");
  // Token has already been checked in stat
  //node->token1 = tk;
  tk = scanToken();
  node->child1 = expr();
  return node;
  // No chance for an error here since the only required checks are 
  // already done or in expr()
}

/* FIRST(iffy) = {IFFY_tk}
 * IFFY_tk is "iffy"
 * Do not need to check the first token since it was checked in stat()
 */
Node* iffy()
{
  Node* node = getNode("iffy");
  //node->token1 = tk;
  tk = scanToken();

  switch(tk.id)
  {
  // iffy [ ...
  case OBRACKET_tk:
    //node->token2 = tk;
    tk = scanToken();
    node->child1 = expr();
    node->child2 = RO();
    node->child3 = expr();

    switch(tk.id)
    {
    // iffy [ <expr> <RO> <expr> ] ...
    case CBRACKET_tk:
      //node->token3 = tk;
      tk = scanToken();
      
      switch(tk.id)
      {
      // iffy [ <expr> <RO> <expr> ] then ...
      case THEN_tk:
        //node->token4 = tk;
        tk = scanToken();
        node->child4 = stat();
        return node;

      default:
        errorParse(node->label, "then");
      }

    default:
      errorParse(node->label, "]");
    }

  default:
    errorParse(node->label, "[");
  }
}

/* FIRST(loop) = {LOOP_tk}
 * LOOP_tk is "loop"
 * Do not need to check the first token since it was checked in stat()
 */
Node* loop()
{
  Node* node = getNode("loop");
  //node->token1 = tk;
  tk = scanToken();

  switch(tk.id)
  {
  // loop [ ...
  case OBRACKET_tk:
    //node->token2 = tk;
    tk = scanToken();
    node->child1 = expr();
    node->child2 = RO();
    node->child3 = expr();

    switch(tk.id)
    {
    // loop [ <expr> <RO> <expr> ] ...
    case CBRACKET_tk:
      //node->token3 = tk;
      tk = scanToken();
      node->child4 = stat();
      return node;

    default:
      errorParse(node->label, "]");
    }

  default:
    errorParse(node->label, "[");
  }
}

/* FIRST(assign) = {IDENT_tk}
 * IDENT_tk is an identifier token
 * Do not need to check the first token since it was checked in stat()
 */
Node* assign()
{
  Node* node = getNode("assign");
  node->token1 = tk;
  tk = scanToken();

  switch(tk.id)
  {
  case CEQUAL_tk:
    //node->token2 = tk;
    tk = scanToken();
    node->child1 = expr();
    return node;

  default:
    errorParse(node->label, ":=");
  }
}

/* FIRST(label) = {LABEL_tk}
 * LABEL_tk is "label"
 * Do not need to check the first token since it was checked in stat()
 */
Node* label()
{
  Node* node = getNode("label");
  //node->token1 = tk;
  tk = scanToken();

  switch(tk.id)
  {
  case IDENT_tk:
    node->token1 = tk;
    tk = scanToken();
    return node;

  default:
    errorParse(node->label, "IDENTIFIER");
  }
}

/* FIRST(goto) = {GOTO_tk}
 * GOTO_tk is "goto"
 * Do not need to check the first token since it was checked in stat()
 */
Node* goto_()
{
  Node* node = getNode("goto");
  //node->token1 = tk;
  tk = scanToken();

  switch(tk.id)
  {
  case IDENT_tk:
    node->token1 = tk;
    tk = scanToken();
    return node;

  default:
    errorParse(node->label, "IDENTIFIER");
  }
}

/* FIRST(expr) = FIRST(N) = FIRST(A) = FIRST(M) = {TIMES_tk} 
 * and FIRST(R) = {OPAREN_tk, IDENT_tk, NUM_tk}
 * TIMES_tk is "*"
 * OPAREN_tk is "("
 * IDENT_tk is an identifier
 * NUM_tk is a number, or integer
 */
Node* expr()
{
  Node* node = getNode("expr");
  switch(tk.id)
  {
  case TIMES_tk:
  case OPAREN_tk:
  case IDENT_tk:
  case NUM_tk:
    node->child1 = N();
    
    switch(tk.id)
    {
    case MINUS_tk:
      node->token1 = tk;
      tk = scanToken();
      node->child2 = expr();
      return node;

    default:
      return node;
    }

  default:
    errorParse(node->label, "*, (, IDENTIFIER, or INTEGER");
  }
}

/* FIRST(N) = FIRST(A) has the same results as expr()
 */
Node* N()
{
  Node* node = getNode("N");
  node->child1 = A();

  switch(tk.id)
  {
  // <A> * <N> | <A> / <N>
  case TIMES_tk:
  case DIVIDE_tk:
    node->token1 = tk;
    tk = scanToken();
    node->child2 = N();
    return node;

  // <A>
  default:
    return node;
  }
}

/* FIRST(A) = FIRST(M) has the same results as expr()
 */
Node* A()
{
  Node* node = getNode("A");
  node->child1 = M();

  switch(tk.id)
  {
  // <M> + <A>
  case PLUS_tk:
    node->token1 = tk;
    tk = scanToken();
    node->child2 = A();
    return node;

  // <M>
  default:
    return node;
  }
}

/* FIRST(M) has the same results as expr()
 * TIMES_tk stays in M
 * The other 3 shift to R
 */
Node* M()
{
  Node* node = getNode("M");
  switch(tk.id)
  {
  // * <M>
  case TIMES_tk:
    node->token1 = tk;
    tk = scanToken();
    node->child1 = M();
    return node;

  // <R>
  default:
    node->child1 = R();
    return node;
  }
}

/* FIRST(R) = {OPAREN_tk, IDENT_tk, NUM_tk}
 * OPAREN_tk is "("
 * IDENT_tk is an identifier
 * NUM_tk is a number, or integer
 */
Node* R()
{
  Node* node = getNode("R");
  switch(tk.id)
  {
  // ( <expr> ...
  case OPAREN_tk:
    //node->token1 = tk;
    tk = scanToken();
    node->child1 = expr();

    switch(tk.id)
    {
    // ( <expr> )
    case CPAREN_tk:
      //node->token2 = tk;
      tk = scanToken();
      return node;

    default:
      errorParse(node->label, ")");
    }

  // IDENTIFIER | INTEGER
  case IDENT_tk:
  case NUM_tk:
    node->token1 = tk;
    tk = scanToken();
    return node;

  default:
    errorParse(node->label, "IDENTIFIER, INTEGER, or (");
  }
}

/* FIRST(RO) = {LESS_tk, GRREATER_tk, DEQUAL_tk}
 * LESS_tk is "<"
 * GREATER_tk is ">"
 * DEQUAL_tk is "=="
 */
Node* RO()
{
  Node* node = getNode("RO");
  switch(tk.id)
  {
  // <
  case LESS_tk:
    node->token1 = tk;
    tk = scanToken();

    switch(tk.id)
    {
    // <<
    case LESS_tk:
      node->token2 = tk;
      tk = scanToken();
      return node;

    // <>
    case GREATER_tk:
      node->token2 = tk;
      tk = scanToken();
      return node;

    default:
      return node;
    }
  // >
  case GREATER_tk:
    node->token1 = tk;
    tk = scanToken();

    switch(tk.id)
    {
    // >>
    case GREATER_tk:
      node->token2 = tk;
      tk = scanToken();
      return node;

    default:
      return node;
    }
  // ==
  case DEQUAL_tk:
    node->token1 = tk;
    tk = scanToken();
    return node;

  default:
    errorParse(node->label, "<, <<, >, >>, <>, or ==");
  }
}

void errorParse(string label, string expected)
{
  cout << endl;
  cout << "ERROR: Building '" << label << "' on line " << tk.lineNum << "." << endl;
  cout << "      Found '" << tk.tokenString << "', expected '" << expected << "'." << endl;
  cout << endl;
  exit(1);
}
