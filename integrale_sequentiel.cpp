#include <iostream>
#include <cstdlib>
#include <stdio.h>
// #include <unistd.h>
// #include <pthread.h>
#include <math.h>
#include <time.h>

using namespace std;



//Strcuture de données de la fonction

typedef struct
{
  long double borne_min, borne_max;
  int coef[3];
  long double integrale;
}Function;



//Afficher la fonction polynome

void printFunction(const Function * p)
{
  cout << p->coef[0] << "*x^2 + " << p->coef[1] << "*x + " << p->coef[2] << endl;
}



//Future onction thread : calcul du polynôme

long double fn(long double x, Function * p)
{
  return p->coef[0]*(x*x) + p->coef[1]*x + p->coef[2];
}



//Future fonction thread

void Ttrapeze(int n, Function * p)
{
  long double T = 0;
  long double xi, xj;
  long double a = p->borne_min;
  long double b = p->borne_max;


  for(int i=0; i<n; i++)
  {
      xi = a+(b-a)*i/(float)n;
      xj = a+(b-a)*(i+1)/(float)n;
      T += (xj-xi)/2.0f * (fn(xi,p)+fn(xj,p));
  }

  if(T<0 || a>b)
    p->integrale = -T;

  p->integrale = T;
}



//MAIN

int main()
{
  clock_t temps;

  Function * f = new Function();
  long int k = 1000;
  f->borne_min = 0;
  f->borne_max = 3*M_PI/2;
  f->coef[0] = 5;
  f->coef[1] = 2;
  f->coef[2] = 20;

  Ttrapeze(k,f);
  long double aireT = f->integrale;

  cout << "l'aire T = " << aireT << " pour " << k << " répétitions \n";

  delete [] f;

  temps = clock();
  printf("Le programme s'est execute en %f secondes \n", (double)temps/CLOCKS_PER_SEC);

  return 0;
}
