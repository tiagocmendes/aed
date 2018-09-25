//
// Tomás Oliveira e Silva, AED, September 2014, September 2017
//
// best rational approximation (with time measurement)
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "elapsed_time.h"


#ifndef DEBUG
#  define DEBUG 0  // use a positive value to see partial results
                   // use a negative value to count loop iteratrions
#endif

typedef unsigned int uint;

typedef struct
{
  uint num;
  uint den;
}
fraction;

//
// brute force approach
//

#if DEBUG < 0
int count_slow;
#endif

fraction best_rational_approximation_slow(double x,uint max_den)
{
  double num,den,e,best_e;
  fraction best;

  best.num = 0u;
  best.den = 1u;
  best_e = x;
#if DEBUG < 0
  count_slow = 0;
#endif
  for(den = 1.0;den <= (double)max_den;den += 1.0)
  {
    num = round(den * x);
    e = fabs(num / den - x);
#if DEBUG < 0
    count_slow++;
#endif
    if(e < best_e)
    {
      best.num = (uint)num;
      best.den = (uint)den;
      best_e = e;
#if DEBUG > 0
      printf("  %u/%u (%.15f)\n",best.num,best.den,best_e);
#endif
    }
  }
  return best;
}

//
// see section 4.5 of "Concrete Mathematics", by Graham, Knuth and Patashnik
//

#if DEBUG < 0
int count_fast;
#endif

fraction best_rational_approximation_fast(double x,uint max_den)
{
  fraction best,f0,f1,f2; // f0=low, f1=mediant, f2=high
  double a,e,best_e;

  best.num = f0.num = 0u;
  best.den = f0.den = 1u;
  best_e = x;
  f2.num = 1u;
  f2.den = 0u;
#if DEBUG < 0
  count_fast = 0;
#endif
  for(;;)
  {
    f1.num = f0.num + f2.num;
    f1.den = f0.den + f2.den;
    if(f1.den > max_den)
      return best;
    a = (double)f1.num / (double)f1.den;
    e = fabs(a - x);
#if DEBUG < 0
    count_fast++;
#endif
    if(e < best_e)
    {
      best = f1;
      best_e = e;
#if DEBUG > 0
      printf("  %u/%u (%.15f)\n",best.num,best.den,best_e);
#endif
    }
    if(a <= x)
      f0 = f1;
    else
      f2 = f1;
  }
}


//
// main program
//

int main(void)
{
  uint max_den;
  fraction f;
  double x,dt;

  x = M_PI;
  printf("Best rational approximations to %18.16f\n",x);
  //
  // to avoid arithmetic overflows the loop
  //   for(max_den = 10u;max_den <= 1000000000u;max_den *= 10u)
  // was modified so that the termination test is done just before the multiplication by 10
  //
  // for 32-bit unsigned integers:
  //     10000000 * 10 =  100000000
  //    100000000 * 10 = 1000000000
  //   1000000000 * 10 = 1410065408 (and not 10000000000)
  //
  max_den = 10u;
  while(1)
  {
    printf("  maximum denominator: %u\n",max_den);
    (void)elapsed_time();
    f = best_rational_approximation_slow(x,max_den);
    dt = elapsed_time();
#if DEBUG < 0
    printf("    slow: %u/%u (%.6e) in %.9f seconds [%d comparisons]\n",f.num,f.den,(double)f.num / (double)f.den - x,dt,count_slow);
#else
    printf("    slow: %u/%u (%.6e) in %.9f seconds\n",f.num,f.den,(double)f.num / (double)f.den - x,dt);
#endif
    (void)elapsed_time();
    f = best_rational_approximation_fast(x,max_den);
    dt = elapsed_time();
#if DEBUG < 0
    printf("    fast: %u/%u (%.6e) in %.9f seconds [%d comparisons]\n",f.num,f.den,(double)f.num / (double)f.den - x,dt,count_fast);
#else
    printf("    fast: %u/%u (%.6e) in %.9f seconds\n",f.num,f.den,(double)f.num / (double)f.den - x,dt);
#endif
    if(max_den == 1000000000u)
      break; // done!
    max_den *= 10u;
  }
  return 0;
}
