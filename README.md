# Compilers
Thought this was lost after a windows update with my Operating Systems code, but I found a zipped copy of it.

We slowly wrote a large program in parts for this class which takes a basic programming language and translates it to something similar to assembly, which we ran in a special virtual machine. I am not sure if I still have the language description, but the .sp2020 test files may provide some help to someone wanting to understand it.

# Readme submitted with the project in class
Author: John Soderstrom
Due Date: 5/14/2020

All test files (.sp2020) are included in the directory and have been
successfully tested.
  basicExpr.sp2020 (tests all of the most basic expressions)
  basicInOut.sp2020 (tests a single basic in and out statement)
  basicGoto.sp2020 (tests goto with 2 labels, 
		    only positive numbers should be output)
  basicLoop.sp2020 (tests an invalid loop and a valid one, 
 		    positive output expected only)
  basicRO.sp2020 (tests all RO cases with iffy statements, 
		  positive output expected only)
  compExpr.sp2020 (tests more complex expressions, 
 		   did not do many because of next file)
  moreExpr.sp2020 (copied from another student, 
		   tests more complex expressions)
  compIffy.sp2020 (mixes iffy statements expected true and false, 
		   positive output expected only)
  compLoop.sp2020 (3 layered loops, expected output in comments in file)
  iffyLoop.sp2020 (mixes loop and iffy statements in both orders,
		   positive output expected only)

I attempted to do some slight temporary variable optimization.
It appears to work based on the above test files, but it's always
possible I missed something. I reserved T0 for iffy, loop, and
out. I didn't use it for expressions because I have no way of 
knowing how many of those will be in a single subtree. It probably
would have been better if I set it as a static variable or something
instead of hardcoding it into those files though.

I have not found a test file yet that didn't work, though some
that I checked were older test files that other students tried and I don't
have written down anymore.

I left the label identifiers as having to be declared per the email
(and the assignment details in P3).

As usual, the makefile is enough to compile the program. To clear it, use
make clean

I also noticed I hadn't added all of the headers to the makefile correctly.
That has been fixed.

I wish classes had not been disrupted. I enjoyed attempting to build
a compiler, and would have liked to try the full project. I might 
actually spend a little time trying to build one that works for other
languages now, though I'm not sure if I'll end up getting to it. 
If I do, I can imagine that'll be a much more intensive process than 
this! At least, if I end up trying to truly complete it.

#########################################################################################
#P3 readme details below
#########################################################################################

As before, the makefile is enough to compile the project alone.
In order to clear files, use
make clean

While doublechecking my files and adjusting a couple unchecked headers in my
makefile, I noticed one of my headers (I believe it was parser.h) did not have
an include guard. That has been added, along with the fixes on the makefile that
had not yet caused a problem.

I tried to limit the number of headers needed for the semantics check while also
trying to keep it separate from the parser file. It felt a little awkward since I
didn't have a good way in mind to run a preorder traversal that passed each node
one by one to the semantics check. This month has been hectic, so I might have
figured out a good way given a bit more time, but it seems more awkward than
handling passing tokens one by one. If I had fully incorporated it into the parser,
I suppose I would have been able to handle it as the parse tree built.

##########################################################################################
#P2 readme details below
##########################################################################################

I noticed a slight issue with my P1 that did not come up in testing before.
In some cases at the end of file, I would get a current character value
of -1 in an int, used to check for EOF. That was getting through to my
fsa, and then the next state could come up as garbage data. While it was
always negative in P1 tests and did not have a visible effect, it did on one
of my first tests here. That issue has now been fixed with an extra check
on currentChar (in driver.cpp) before calling for the next state.

The makefile is enough to compile this project.
In order to clear files, use
make clean

Previous points of the readme from P1 stand, at the
bottom of the file and marked.

Following the examples given, I reworked my tree files slightly. I pushed
everything into node.cpp and node.h, allowing main to access just the root
node passed back from parser.cpp and printing it that way.

Parser.cpp and parser.h similarly followed the examples. I had to mess with
error checking on mStat a little - when I used "}" as a check to return
to block for finishing a block, my default was to go into stat and mStat.
But if the next token isn't the start of a statement, the programmer probably
wanted to close the block next, and if it was the start then the compiler wouldn't
be able to tell anyway. So I added an additional check for FIRST(stat) so I could
send an error related to the block if neither case were true.

For now, I do include structural tokens in the nodes. I'll probably take that
out for Project 3 depending on what's needed, but it's been a chaotic month
and I believe this is ready to turn in.

Errors mention the label of the node being worked on at the time of failure
(or one closely related if it's more useful, I believe, such as mStat and block),
the line number of the failed token, the expected tokens, and the
token string that it got.

###########################################################################################
#P1 readme details below
###########################################################################################

The program reads files from arguments and file
contents funneled to stdin perfectly fine from what
I have tried. I had trouble handling actual keyboard 
input, if that was required here, without writing
another temporary file for the contents.

I have tested the fsa with multiple types of identifiers,
every keyword, integers, and every operator. 

The tests included errors such as integer tokens with 
letters added, a single "=", multiple operators in a row,
whitespace immediately before and after a newline, and
lack of whitespace between tokens. There may be edge
cases I did not consider, but I expect any would be
a minority.

I separated the scanner, driver, and fsa into files
of the same name. 

  Scanner: given a file pointer, will handle reading 
           from the file; the other two files will 
           never see the file pointer.
  FSA: stores the 2d array and all related constance. 
       Other functions deal purely with reading and 
       setting the state. 
  Driver: filters input from the scanner, gets state
          information from the fsa, builds tokens, and 
          generates errors. 

I originally had filter functionality in the scanner, 
but had difficulty handling moving important information
where it was needed without extra complexity, lowering
readability.

I placed token structure and enum information into a
separate header (token.h) and included it in multiple others.
It was needed for multiple source files, but I expect 
it will rarely need to be modified.

Printing to user:

I added an extra line after every 10 tokens printed.
For larger number of tokens I believe this made it
easier to read and keep track of all of them.

For errors, I tracked the column number and text
around the error to make it easier to find.
Any text in the string before the error hit is retained.
As such, an error beginning with an operator will
start with the operator that cause the problem.

If the error occurred before the end of the string,
the error will include up to 6 characters after the error
or up to the first newline/carriage return, whichever
happens first. This is an attempt to help find the
error when the context would otherwise be a single character.
