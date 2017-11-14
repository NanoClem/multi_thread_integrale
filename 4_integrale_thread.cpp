#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <pthread.h>
#include <mutex>
#include <math.h>
#include <time.h>

using namespace std;


#define NBITERATION 1000
#define NBTHREADS 10


//MUTEX : verrou pour les variables partagées et les zones critiques
mutex mtx;


//VARIABLES GLOBALES
long double temp_integral = 0;					//Valeur finale de l'intégrale
float partSize = NBITERATION  / NBTHREADS;		//Nombre de trapèzes calculés par thread



//STRUCTURE DE DONNEES :
//Fonction polynomiale du 2nd degré

typedef struct
{
  long double min, max;		//Bornes minorante et majorante de la fonction
  double coef[3];			//Coefficients du polynôme
}Function;




//FONCTION THREAD : afficher le polynome

void printFunction(Function * p)
{
  cerr << "On est dans la fonction d'affichage de la fonction \n";
  cout << "f(x) = " << p->coef[0] << "*x^2 + " << p->coef[1] << "*x + " << p->coef[2] << endl;
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




//FONCTION THREAD : calcul de l'integrale de chaque trapèze et additionne le résultat avec
// la variable globale temp_integrale pour chaque itération
void * Ttrapeze(void * _args)
{
  Function * _p = (Function *) _args;

  long double T = 0.0f;
  long double localSum = 0.0f;
  long double xi, xj;
  long double a = _p->min;
  long double b = _p->max;


  for(size_t i=0; i < size_t(partSize); ++i)
  {
      xi = a+(b-a)*i / double(NBITERATION);
      xj = a+(b-a)*(i+1) / double(NBITERATION);
      T = (xj-xi)/2.0f * (fn(xi,_p)+fn(xj,_p));

      if(T<0 || a>b)
        T = -T;

      localSum += T;
  }
  printf("\n");
  printf("%Lf \n", localSum);
  printf("%Lf \n", temp_integral);
  IntegralSum(localSum);
  printf("%Lf \n", temp_integral);
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

  printf("\n");
  cout << "On va decouper le calcul en " << NBTHREADS << " thread(s) de " << partSize << " trapèzes chacun" << endl;
  printf("\n");

  for(size_t i=0; i<NBTHREADS; ++i)
  {
    myFunctions[i] . min = 0;
    myFunctions[i] . max = 5;
    myFunctions[i] . coef[0] = 5;
    myFunctions[i] . coef[1] = 2;
    myFunctions[i] . coef[2] = 20;
  }



  //LA FONCTION POLYNOMIALE

  printf("\n");
  printFunction(myFunctions);



  //DEPART DU COMPTEUR
  begin = clock();



  //DEPART DES THREADS
  cout << "MAIN(), Données operationnelles, depart des threads \n \n";

  for(size_t th=0; th<NBTHREADS; ++th)
  {
    cerr << "lancement du thread " << th << endl;
    pthread_create(&thtab[th], NULL, Ttrapeze, (void *) &myFunctions[th]);
  }

  printf("\n");
  cerr << "tous les thread sont lances \n";
  printf("\n");


  for(size_t th=0; th<NBTHREADS; ++th)
  {
    pthread_join(thtab[th], NULL);
	printf("arrivée du thread %zu \n", th);
  }


  delete [] myFunctions;


  printf("\n");
  cout << "MAIN(), Fin des threads \n";
  printf("\n");


  //FIN DU COMPTEUR
  end = clock();
  printf("                                               ---------------------RESULTATS--------------------- \n");
  printf("\n");
  printf("                                                 L'aire finale est de %Lf uA pour %d itération(s) \n", temp_integral, NBITERATION);
  printf("                                                 Le programme s'est exécuté en %Lf secondes \n", (long double)(end-begin)/CLOCKS_PER_SEC);
  printf("\n");
  printf("                                               --------------------------------------------------- \n");
  printf("\n");


  return(EXIT_SUCCESS);
}
