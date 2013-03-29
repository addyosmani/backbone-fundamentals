/*===
cexcept: example2.c 2.0.0 (2001-Mar-21-Wed)
Adam M. Costello <amc@cs.berkeley.edu>

An example application that demonstrates how to use version 2.0.* of the
cexcept.h interface to provide polymorphic exceptions, while avoiding
the use of global variables.  It also illustrates the use of dynamically
nested Try blocks.

See README for copyright information.

See example.c for a simpler example.

===*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* The following declarations would normally go in a separate .h file: */

#include "cexcept.h"

enum exception_flavor { okay, oops, screwup, barf };

struct exception {
  enum exception_flavor flavor;
  const char *msg;
  union {
    int oops;
    long screwup;
    char barf[8];
  } info;
};

define_exception_type(struct exception);

/* End of separate .h file. */


struct thread_state {
  int blah;
  struct exception_context ec[1];
  unsigned long junk;
};


void demo_throw(struct exception_context *the_exception_context)
{
  static int count = 0;
  struct exception e;

  fprintf(stderr, "enter demo_throw\n");

  ++count;

  if (count == 2) {
    e.flavor = oops;
    e.msg = "demo oops message";
    e.info.oops = 17;
    Throw e;
  }
  else if (count == 3) {
    e.flavor = barf;
    e.msg = "demo barf message";
    strcpy(e.info.barf, "ABCDEFG");
    Throw e;
  }
  else if (count == 4) {
    e.flavor = screwup;
    e.msg = "demo screwup message";
    e.info.screwup = 987654321;
    Throw e;
  }

  fprintf(stderr, "return from demo_throw\n");
}


void foo(struct thread_state *state)
{
  fprintf(stderr, "enter foo\n");
  demo_throw(state->ec);
  fprintf(stderr, "return from foo\n");
}


void bar(struct thread_state *state)
{
  struct exception_context *the_exception_context = state->ec;
  struct exception e;

  fprintf(stderr, "enter bar\n");

  Try foo(state);
  Catch (e) {
    switch (e.flavor) {
      case okay: break;
      case oops: fprintf(stderr, "bar caught oops (info == %d): %s\n",
                         e.info.oops, e.msg);
                 break;
        default: Throw e;
    }
  }

  fprintf(stderr, "return from bar\n");
}


int main()
{
  struct thread_state state[1];
  struct exception_context *the_exception_context = state->ec;
  struct exception e;

  init_exception_context(the_exception_context);

  Try {
    bar(state);  /* no exceptions */
    bar(state);  /* exception will be caught by bar(), looks okay to us */
    bar(state);  /* bar() will rethrow the exception */
    fprintf(stderr, "we won't get here\n");
  }
  Catch (e) {
    switch (e.flavor) {
         case okay: break;
         case barf: fprintf(stderr, "main caught barf (info == %s): %s\n",
                            e.info.barf, e.msg);
                    break;
      case screwup: fprintf(stderr, "main caught screwup (info == %ld): %s\n",
                            e.info.screwup, e.msg);
           default: fprintf(stderr, "main caught unknown exception\n");
    }
  }

  return EXIT_SUCCESS;
}
