/* test.c - a simple module to test the dynamic module-loader */

#include <stdio.h>

/* Prototypes - we need to decide whether to use a separate header file for modules or not.
   Perhaps for multi-file modules. */
void
sum_numbers (int a, int b, int *ans);
int
mul_numbers (int a, int b);

/* Sum two integers, a and b, and store the result in ans */
void
sum_numbers (int a, int b, int *ans)
{
  *ans = a + b;
}

/* Multiply two integers, a and b, and return the result */
int
mul_numbers (int a, int b)
{
  return a * b;
}
