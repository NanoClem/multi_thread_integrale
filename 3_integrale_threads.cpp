#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <pthread.h>
#include <mutex>
#include <math.h>
#include <time.h>

using namespace std;


#define NBITERATION 100
#define NBTHREADS 10


//MUTEX : verrou pour les variables partagées et les zones critiques
mutex mtx;



//STRUCTURE DE DONNEES : fonction polynomiale du 2nd degré

typedef struct
{
  long double min, max;
  double coef[3];
  size_t size;
  long double * integral;
}Function;




//FONCTION THREAD : afficher le polynome

void printFunction(Function * p)
{
  mtx.lock();
  cerr << "THREAD(), on est dans le thread d'affichage \n";
  cout << "f(x) = " << p->coef[0] << "*x^2 + " << p->coef[1] << "*x + " << p->coef[2] << endl;
  mtx.unlock();
}


//Afficher les éléments contenu dans le tableau d'integrale

void printIntegral(const Function * _f) {
	mtx.lock();
	for (unsigned int i = 0; i<_f -> size; ++i)
		cout << _f->integral[i] << " / ";
	cout << endl;
	mtx.unlock();
}




//CALCUL DU POLYNOME

long double fn(long double x, Function * _p)
{
  long double fx = _p->coef[0]*(x*x) + _p->coef[1]*x + _p->coef[2];
  return fx;
}



//FONCTION THREAD : calcul de l'integrale de chaque trapèze et la stocke dans
void Ttrapeze(Function * _p) //void * _args)
{
  //Function * _p = (Function *) _args;

  //mtx.lock();
  cerr << "THREAD(), on est dans le thread du calcul de l'integrale \n";
  //mtx.unlock();

  long double T = 0.0f;
  long double xi, xj;
  long double a = _p->min;
  long double b = _p->max;


  for(size_t i=0; i<NBITERATION; ++i)
  {
      xi = a+(b-a)*i/(float)NBITERATION;
      xj = a+(b-a)*(i+1)/(float)NBITERATION;
      T = (xj-xi)/2.0f * (fn(xi,_p)+fn(xj,_p));

      if(T<0 || a>b)
        _p -> integral[i] = -T;

      _p -> integral[i] = T;
  }

  //return(NULL);
}




//FONCTION THREAD : somme des aires des trapèzes

void * partialSum(void * _args)
{
  Function * _p = (Function *) _args;
  //printIntegral(_p);

  long double * sum = new long double();
  (*sum) = 0.0f;

  for(unsigned int i=0; i<(_p -> size); ++i)
    (*sum) += _p -> integral[i];

  mtx.lock();
  cerr << "integral_sum = " << (*sum) << "\n";
  mtx.unlock();

  return(sum);
}





//FONCTION MAIN

int main()
{
  pthread_t th1;
  pthread_t thtab[NBTHREADS];
  clock_t begin, end;

  long double result = 0.0f;
  unsigned int partSize = NBITERATION / NBTHREADS;

  Function * myFunctions = new Function[NBTHREADS];

//Calcul des intégrales de chaque trapèze
  Function * _f = new Function();
  _f -> integral = new long double[NBITERATION];
  Ttrapeze(_f);
  //pthread_create(&th1, NULL, Ttrapeze, (void *) _f);
  //pthread_join(th1, NULL);

  cout << "On va decouper en " << NBTHREADS << " morceaux de " << partSize << " elements" << endl;


  for(size_t i=0; i<NBTHREADS; ++i)
  {
    myFunctions[i] . size = partSize;
    myFunctions[i] . integral = new long double[partSize];
    myFunctions[i] . min = 0;
    myFunctions[i] . max = 3*M_PI/2;
    myFunctions[i] . coef[0] = 5;
    myFunctions[i] . coef[1] = 2;
    myFunctions[i] . coef[2] = 20;

    for(size_t j=0; j<partSize; ++j)
      myFunctions[i] . integral[j] = _f -> integral[partSize*i + j];

    printIntegral(&myFunctions[i]);
  }


  printf("\n");

  //Départ du compteur
  begin = clock();

  //Les threads
  cout << "MAIN(), Données operationnelles, depart des threads \n";

  for(size_t th=0; th<NBTHREADS; ++th)
  {
    mtx.lock();
    cerr << "lancement du thread " << th << endl;
    mtx.unlock();
    pthread_create(&thtab[th], NULL, partialSum, (void *) &myFunctions[th]);
  }


  cerr << "tous les thread sont lances \n";
  void *r;
  long double * res;


  for(size_t th=0; th<NBTHREADS; ++th)
  {
    pthread_join(thtab[th], &r);
    res = (long double *) r;

    result += (*res);

    mtx.lock();
    cerr << "arrivee du thread " << th << "; r= " << *res << "; result+ = " << result << endl;
    mtx.unlock();
  }

  cout << "MAIN(), Fin des threads \n";
  printf("L'aire finale est de %Lf uA \n", result);

  delete [] _f;
  delete [] myFunctions;

  //Fin du compteur
  end = clock();
  printf("Le programme s'est execute en %Lf secondes \n", (long double)(end-begin)/CLOCKS_PER_SEC);
  printf("\n");


  printf("\n");
  return(EXIT_SUCCESS);
}
