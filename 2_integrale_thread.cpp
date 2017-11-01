#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <math.h>
#include <time.h>

using namespace std;



//MUTEX : verrou pour les variables partagées et les zones critiques
mutex mtx;



//STRUCTURE DE DONNEES : fonction polynomiale du 2nd degré

typedef struct
{
  long double min, max;
  double coef[3];
  long double integrale;
  int rep;
}Function;




//FONCTION THREAD : afficher le polynome

void thread_printFunction(Function * p)
{
  mtx.lock();
  cerr << "THREAD(), on est dans le thread d'affichage \n";
  cout << "f(x) = " << p->coef[0] << "*x^2 + " << p->coef[1] << "*x + " << p->coef[2] << endl;
  mtx.unlock();
}




//CALCUL DU POLYNOME

long double fn(long double x, Function * p)
{
  long double fx = p->coef[0]*(x*x) + p->coef[1]*x + p->coef[2];
  return fx;
}





void thread_Ttrapeze(Function * p)
{
  mtx.lock();
  cerr << "THREAD(), on est dans le thread du calcul de l'integrale \n";
  mtx.unlock();

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

  mtx.lock();
  printf("L'aire T = %Lf uA  pour %d repetitions \n", T, n);
  mtx.unlock();
}



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
  printf("Le programme s'est execute en %f secondes \n", (double)(end-begin)/CLOCKS_PER_SEC);
  printf("\n");


  printf("\n");
  return(EXIT_SUCCESS);
}
