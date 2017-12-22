#include <omp.h>
#include <iostream>
#include <cstdlib>
#include <time.h>

using namespace std;

//VARIABLES GLOBALES

#define NBITERATION 1000000


//STRUCTURE DE DONNEES :
//Fonction polynomiale du 2nd degré

typedef struct
{
  long double min, max;		//Bornes minorante et majorante de la fonction
  double coef[3];           //Coefficients du polynôme
  float bmin, bmax;		    //Borne minimal et maximal recalculées à chaque trapèze
  long double Sum;	        //Somme final de l'intégrale
}Function;


//FONCTION : afficher le polynome
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

//FONCTION MAIN

int main()
{

  clock_t begin, end;
  Function * myFunction = new Function;
  int nthread;
  
  //DEMANDE DU NOMBRE DE THREADS
  cout << "Combien voulez vous de threads? > ";
  cin  >> nthread;

  //LA FONCTION POLYNOMIALE
  printf("\n");
  printFunction(myFunction);
  
  printf("\n");
  cout << "On va decouper le calcul en " << nthread << " thread(s) de " << NBITERATION/nthread << " trapèzes chacun" << endl;
  printf("\n");

  myFunction->min = 0;
  myFunction->max = 5;
  myFunction->coef[0] = 5;
  myFunction->coef[1] = 2;
  myFunction->coef[2] = 20;
  myFunction->Sum = 0.0f;
  
  long double T = 0.0f;
  long double xi, xj;
  long double a = 0;
  long double b = 5;

  //DEPART DU COMPTEUR
  
  begin=clock();
  
  #pragma omp parallel sections num_threads(nthread)
  {

	#pragma omp section
	for(size_t i=0; i < size_t(NBITERATION); ++i)
	{
	  myFunction->bmin = (NBITERATION/nthread)*i;
      myFunction->bmax = (NBITERATION/nthread)*(i+1);
      xi = a+(b-a)*i / double(NBITERATION);
      xj = a+(b-a)*(i+1) / double(NBITERATION);
	  long double fxi = myFunction->coef[0]*(xi*xi) + myFunction->coef[1]*xi + myFunction->coef[2];
	  long double fxj = myFunction->coef[0]*(xj*xj) + myFunction->coef[1]*xj + myFunction->coef[2];
      T = (xj-xi)/2.0f * (fxi+fxj);
      
      if(T<0 || a>b)
        T = -T;
      myFunction->Sum += T;
	}
  }
  
  end = clock();
  printf("                                               ---------------------RESULTATS--------------------- \n");
  printf("\n");
  printf("                                                 L'aire finale est de %Lf uA pour %d itération(s) \n", myFunction->Sum, NBITERATION);
  printf("                                                 Le programme s'est exécuté en %Lf secondes \n", (long double)(end-begin)/CLOCKS_PER_SEC);
  printf("\n");
  printf("                                               --------------------------------------------------- \n");
  printf("\n");
  
  delete myFunction;
  return 0;
}
