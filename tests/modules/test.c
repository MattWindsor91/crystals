/* test.c - a simple module to test the dynamic module-loader */

#include <stdio.h>

/* Prototypes - we need to decide whether to use a separate header file for modules or not.
   Perhaps for multi-file modules. */
void
init (void);
void
term (void);
void
hello (void);
void
sum_numbers (int a, int b, int *ans);

/* Print a message announcing the module has been loaded */
void
init (void)
{
  printf ("test init: Module test has been loaded!\n");
}

/* Print a message announcing the module has been unloaded */
void
term (void)
{
  printf ("test term: Module test has been unloaded!\n");
}

/* Print a hello world message, the standard test function. */
void
hello (void)
{
  printf ("Hello, world.");
}

/* Sum two integers, a and b, and store the result in ans */
void
sum_numbers (int a, int b, int *ans)
{
  *ans = a + b;
}
