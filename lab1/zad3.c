#include <stdio.h>
#include <gsl/gsl_ieee_utils.h>



int main (void)
{
  float f = 1.0/3.0;
  
  

  for(int i=0; i<165;i++){
    gsl_ieee_printf_float(&f);
    f =f/2.0;
    printf("\n");
    
  }
  // 
  // x = znak * 1,mantysa * 2^(wykladnik -127)
  // mantysa przstaje byc znormalizowana, gdy x osiagnal postac znak *1,mantysa * 2^(-126)
  // czyli gdy: wykladnik - 127 = -126 
  // czyli wykladnik = 1 
  // wynika to z tego, ze wykladnik moze miec wartosci: 1,2,...,254 
  // zminiejszajac ponownie liczbe tracimy dokladnosc mantysy

  return 0;
}

