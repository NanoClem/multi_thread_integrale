
/*
  J'ai utilisé 2 threads dans le programme mais il faut régler les problèmes de partage de
  données (zones d'exclusion mutuelles par exemple).
  Les résultats sont bon, c'est juste à l'affichage des messages intra-thread que les problèmes surviennent
*/



#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

using namespace std;



//Strcuture de données de la fonction

typedef struct
{
  long double min, max;
  double coef[3];
  long double integrale;
  int rep;
}Function;



//Afficher la fonction polynome

void * thread_printFunction(void * _args)
{
  cerr << "TREAD(), on est dans le thread d'affichage \n";

  const Function * p = (const Function *)_args;
  cout << "f(x) = " << p->coef[0] << "*x^2 + " << p->coef[1] << "*x + " << p->coef[2] << endl;

  return(NULL);
}



//Future onction thread : calcul du polynôme pour un x donné

long double fn(long double x, Function * p)
{
  return p->coef[0]*(x*x) + p->coef[1]*x + p->coef[2];
}



//fonction thread : calcul de l'intégrale

void * thread_Ttrapeze(void * _args)
{
  cerr << "TREAD(), on est dans le thread du calcul de l'integrale \n";

  Function * p  = (Function *)_args;
  long double T = 0;
  long double xi, xj;
  int n = p->rep;
  long double a = p->min;
  long double b = p->max;


  for(int i=0; i<n; i++)
  {
      xi = a+(b-a)*i/(float)n;
      xj = a+(b-a)*(i+1)/(float)n;
      T += (xj-xi)/2.0f * (fn(xi,p)+fn(xj,p));
  }

  if(T<0 || a>b)
    p->integrale = -T;

  p->integrale = T;

  cout << "l'aire T = " << T << " pour " << n << " répétitions \n";
  return(NULL);
}



//MAIN

int main()
{
  clock_t begin, end;
  pthread_t th1, th2;

  Function * arg = new Function();
  arg->rep = 100000;
  arg->min = 0;
  arg->max = 3*M_PI/2;
  arg->coef[0] = 5;
  arg->coef[1] = 2;
  arg->coef[2] = 20;

  printf("\n");

  //Départ du compteur
  begin = clock();


  //Les threads
  cout << "MAIN(), Structure operationnelle, depart des threads \n";
  pthread_create(&th1, NULL, thread_printFunction, (void *)arg);
  pthread_create(&th2, NULL, thread_Ttrapeze, (void *)arg);

  pthread_join(th1,NULL);
  pthread_join(th2, NULL);
  cout << "MAIN(), Fin des threads \n";

  delete [] arg;

  //Fin du compteur
  end = clock();
  printf("Le programme s'est execute en %f secondes \n", (double)(end-begin)/CLOCKS_PER_SEC);
  printf("\n");


  return(EXIT_SUCCESS);
}
