#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <pthread.h>
#include <mutex>
#include <math.h>
#include <time.h>

using namespace std;


#define NBITERATION 1000000
#define NBTHREADS 10


//MUTEX : verrou pour les variables partagées et les zones critiques
mutex mtx;


//VARIABLES GLOBALES

long double temp_integral = 0;
float partSize = NBITERATION  / NBTHREADS;



//STRUCTURE DE DONNEES : fonction polynomiale du 2nd degré

typedef struct
{
  long double min, max;
  double coef[3];
}Function;




//FONCTION THREAD : afficher le polynome

void printFunction(Function * p)
{
  mtx.lock();
  cerr << "THREAD(), on est dans le thread d'affichage \n";
  cout << "f(x) = " << p->coef[0] << "*x^2 + " << p->coef[1] << "*x + " << p->coef[2] << endl;
  mtx.unlock();
}



//CALCUL DU POLYNOME

long double fn(long double x, Function * _p)
{
  long double fx = _p->coef[0]*(x*x) + _p->coef[1]*x + _p->coef[2];
  return fx;
}




//SOMME DES INTEGRALES : avec la variable globale

void IntegralSum(long double localDataSum)
{
  mtx.lock();
  temp_integral += localDataSum;
  mtx.unlock();

  //printf("Somme des intégrales = %Lf \n", temp_integral);
}




//FONCTION THREAD : calcul de l'integrale de chaque trapèze et la stocke dans
void * Ttrapeze(void * _args)
{
  Function * _p = (Function *) _args;

  long double T = 0.0f;
  long double localSum = 0.0f;
  long double xi, xj;
  long double a = _p->min;
  long double b = _p->max;


  for(size_t i=0; i<partSize; ++i)
  {
      xi = a+(b-a)*i/(float)NBITERATION;
      xj = a+(b-a)*(i+1)/(float)NBITERATION;
      T = (xj-xi)/2.0f * (fn(xi,_p)+fn(xj,_p));

      if(T<0 || a>b)
        T = -T;

      localSum += T;
      IntegralSum(localSum);
  }
  return(NULL);
}






//FONCTION MAIN

int main()
{

  // VARIABLES

  pthread_t thtab[NBTHREADS];
  clock_t begin, end;
  Function * myFunctions = new Function[NBTHREADS];


  // DECOUPAGE

  cout << "On va decouper en " << NBTHREADS << " morceaux de " << partSize << " elements" << endl;
  printf("\n");

  for(size_t i=0; i<NBTHREADS; ++i)
  {
    myFunctions[i] . min = 0;
    myFunctions[i] . max = 3*M_PI/2;
    myFunctions[i] . coef[0] = 5;
    myFunctions[i] . coef[1] = 2;
    myFunctions[i] . coef[2] = 20;
  }


  printf("\n");


  //DEPART DU COMPTEUR
  begin = clock();


  //DEPART DES THREADS
  cout << "MAIN(), Données operationnelles, depart des threads \n";

  for(size_t th=0; th<NBTHREADS; ++th)
  {
    cerr << "lancement du thread " << th << endl;
    pthread_create(&thtab[th], NULL, Ttrapeze, (void *) &myFunctions[th]);
  }


  cerr << "tous les thread sont lances \n";


  for(size_t th=0; th<NBTHREADS; ++th)
  {
    pthread_join(thtab[th], NULL);
  }

  printf("\n");
  cout << "MAIN(), Fin des threads \n";
  printf("\n");
  printf("L'aire finale est de %Lf uA \n", temp_integral);

  delete [] myFunctions;

  //Fin du compteur
  end = clock();
  printf("Le programme s'est execute en %Lf secondes \n", (long double)(end-begin)/CLOCKS_PER_SEC);
  printf("\n");


  printf("\n");
  return(EXIT_SUCCESS);
}
