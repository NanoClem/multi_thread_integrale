#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <math.h>
#include <time.h>

using namespace std;


#define NBITERATION
#define NBTHREADS 10


//MUTEX : verrou pour les variables partagées et les zones critiques
mutex mtx;



//STRUCTURE DE DONNEES : fonction polynomiale du 2nd degré

typedef struct
{
  long double min, max;
  double coef[3];
  long double * integrale;
  unsigned int n;
}Function;




//FONCTION THREAD : afficher le polynome

void thread_printFunction(Function * p)
{
  mtx.lock();
  cerr << "THREAD(), on est dans le thread d'affichage \n";
  cout << "f(x) = " << p->coef[0] << "*x^2 + " << p->coef[1] << "*x + " << p->coef[2] << endl;
  mtx.unlock();
}


//Afficher les éléments contenu dans le tableau d'integrale

void printIntegral(const struct Function * _f) {
	mtx.lock();
	for (unsigned int i = 0; i<_f -> size; ++i)
		cout << _f->tab[i] << " / ";
	cout << endl;
	mtx.unlock();
}




//CALCUL DU POLYNOME

long double fn(long double x, Function * p)
{
  long double fx = p->coef[0]*(x*x) + p->coef[1]*x + p->coef[2];
  return fx;
}



//FONCTION THREAD : calcul de l'integrale de chaque trapèze et la stocke dans 

void thread_Ttrapeze(Function * p)
{
  mtx.lock();
  cerr << "THREAD(), on est dans le thread du calcul de l'integrale \n";
  mtx.unlock();

  long double * T = new long double();
  (*T) = 0.f;

  long double xi, xj;
  long double a = p->min;
  long double b = p->max;


  for(int i=0; i<(p -> n); ++i)
  {
      xi = a+(b-a)*i/(float)n;
      xj = a+(b-a)*(i+1)/(float)n;
      (*T) = (xj-xi)/2.0f * (fn(xi,p)+fn(xj,p));

      if(T<0 || a>b)
        p -> integral[i] = -(*T);

      p -> integral[i] = (*T);
  }
}




//FONCTION THREAD : somme






//FONCTION MAIN

int main()
{
  clock_t begin, end;

  Function * f = new Function();
  f->rep = 100000000;
  f->min = 0;
  f->max = 3*M_PI/2;
  f->coef[0] = 5;
  f->coef[1] = 2;
  f->coef[2] = 20;

  printf("\n");

  //Départ du compteur
  begin = clock();

  //Les threads
  cout << "MAIN(), Structure operationnelle, depart des threads \n";
  thread t1(thread_printFunction, f);
  thread t2(thread_Ttrapeze, f);

  t1.join();
  t2.join();
  cout << "MAIN(), Fin des threads \n";

  delete [] f;

  //Fin du compteur
  end = clock();
  printf("Le programme s'est execute en %Lf secondes \n", (long double)(end-begin)/CLOCKS_PER_SEC);
  printf("\n");


  printf("\n");
  return(EXIT_SUCCESS);
}
