#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdexcept>
#include <cstdlib>
#include <pthread.h>
#include <mutex>
#include <math.h>
#include <chrono>

using namespace std;


// Afficher les infos relatives à la mémoire
#define PRINT_MEM "\
#!/bin/ \n\
free -t\
"


// TEST DE L'EXISTENCE DU FICHIER CONTENANT LES RESULTATS DU CALCUL
bool doExists(string filename)
{
  ifstream ifs(filename.c_str(), ios::in);
  if(ifs)
    return true;
  else
    return false;
}


// CREATION DU FICHIER DES RESULTATS
void createFile(string filename)
{
  ofstream ofs(filename.c_str(), ios::out);
  if(ofs)
  {
    cout << "fichier cree avec succes \n";
    ofs.close();
  }
  else
    throw runtime_error("Erreur : impossible de creer le fichier !");
}


// ECRIRE LES DONNEES DANS LE FICHIER DES RESULTATS
void writeFileData(string filename, const unsigned int ITERATION, long double execTime)
{
  ofstream ofs;
  ofs.open(filename.c_str(), ios::out | ios::app);          // on écrit à la fin du fichier avec "app" pour éviter l'overwrite
  if(ofs)
  {
    ofs << ITERATION
        << "   "
        << execTime
        << "\n";

    ofs.close();
  }
  else
    throw runtime_error("Erreur : impossible d'ecrire dans le fichier !");
}


//MUTEX : verrou pour les variables partagées et les zones critiques
mutex mtx;


//VARIABLES GLOBALES
long double final_integral = 0;					      // Valeur finale de l'intégrale




//STRUCTURE DE DONNEES :
//Fonction polynomiale du 2nd degré

typedef struct
{
  long double min, max;		          // Bornes minorante et majorante de la fonction
  double coef[3];			              // Coefficients du polynôme
  float bmin, bmax;                 // Intervalle sur lequel le calcul de l'intégrale est effectué
  int ITERATION;                    // Nombre d'itérations de la fonction
}Function;




//FONCTION THREAD : afficher le polynome

void printFunction(Function * p)
{
  cerr << "FONCTION POLYNOME \n";
  cout << "f(x) = " << p->coef[0] << "*x^2 + " << p->coef[1] << "*x + " << p->coef[2] << endl;
}



//CALCUL DU POLYNOME : fonction polynome

long double fn(long double x, Function * _p)
{
  long double fx = _p->coef[0]*(x*x) + _p->coef[1]*x + _p->coef[2];
  return fx;
}




//SOMME DES INTEGRALES : avec la variable globale final_integral
// La variable final_integral étant globale, elle est partagée entre tous les threads
// on a donc une zone d'exclusion mutuelle avec un mutex
void IntegralSum(long double localDataSum)
{
  mtx.lock();
  final_integral += localDataSum;
  mtx.unlock();
}




//FONCTION THREAD : calcul de l'integrale de chaque trapèze et additionne le résultat avec
// la variable globale final_integrale pour chaque itération
void * Ttrapeze(void * _args)
{
  Function * _p = (Function *) _args;

  long double T = 0.0f;
  long double localSum = 0.0f;
  long double xi, xj;
  long double a = _p->min;
  long double b = _p->max;
  const int N = _p->ITERATION;


  for(size_t i=_p->bmin; i < size_t(_p->bmax); ++i)
  {
      xi = a+(b-a)*i / double(N);               // On recalcule les bornes actuelles
      xj = a+(b-a)*(i+1) / double(N);           // pour faire le calcul jusqu'à bmax
      T  = (xj-xi)/2.0f * (fn(xi,_p)+fn(xj,_p));

      if(T<0 || a>b)
        T = -T;

      localSum += T;
  }

  IntegralSum(localSum);    // Ajout du résultat local à la somme globale des intégrtales
  return(NULL);
}






//FONCTION MAIN

int main(int argc, char * argv[])
{
  printf("\n");

  // PARAMETRES DE CONSOLE
  if(argc != 3)                                                         // argv[0] contient le nom de l'exécutable
  {
    cout << "Nombre d'arguments invalide, veuillez renseigner le nombre de threads PUIS le nombre d'iteration \n";
    cout << "\n";
    exit(0);
  }

  const unsigned int NBTHREADS = atoi(argv[1]);                    // Nombre de threads, atoi permet de convettir argv[] en entier
  const int NBITERATION        = atoi(argv[2]);                    // Nombre d'iteration
  float partSize               = NBITERATION  / NBTHREADS;		     // Nombre de trapèzes calculés par thread



  // MEMOIRE AVANT LANCEMENT
  printf("MEMOIRE AVANT LE LANCEMENT DES THREADS \n");
  system(PRINT_MEM);


  // VARIABLES
  pthread_t thtab[NBTHREADS];
  Function * myFunctions = new Function[NBTHREADS];


  // DECOUPAGE
  printf("\n");
  cout << "On va decouper le calcul en " << NBTHREADS << " thread(s) de " << partSize << " trapèzes chacun" << endl;
  printf("\n");

  for(size_t i=0; i<NBTHREADS; ++i)
  {
    myFunctions[i] . min       = 0;                  // intervalle global de la fonction
    myFunctions[i] . max       = 5;                  // commun à tous les threads
    myFunctions[i] . bmin      = partSize*i;         // on repousse les bornes min et max locales de chaque structure pour avoir le bon intervalle de calcul
    myFunctions[i] . bmax      = partSize*(i+1);     // pour chaque thread et ainsi éviter de calculer sur le même intervalle
    myFunctions[i] . coef[0]   = 5;
    myFunctions[i] . coef[1]   = 2;
    myFunctions[i] . coef[2]   = 20;
    myFunctions[i] . ITERATION = NBITERATION;
  }


  //LA FONCTION POLYNOMIALE
  printf("\n");
  printFunction(myFunctions);
  printf("\n");


  //DEPART DU COMPTEUR
  chrono::steady_clock::time_point begin = chrono::steady_clock::now();


  //DEPART DES THREADS
  cout << "MAIN(), Données operationnelles, depart des threads \n \n";

  for(size_t th=0; th<NBTHREADS; ++th)
  {
    cerr << "lancement du thread " << th << endl;
    pthread_create(&thtab[th], NULL, Ttrapeze, (void *) &myFunctions[th]);
  }

  printf("tous les thread sont lances \n");
  printf("\n");
  printf("\n");


  // MEMOIRE PENDANT LE PROGRAMME
  printf(" MEMOIRE PENDANT LES THREADS \n");
  system(PRINT_MEM);

  printf("\n");
  printf("\n");


  for(size_t th=0; th<NBTHREADS; ++th)
  {
    pthread_join(thtab[th], NULL);
	  printf("arrivée du thread %zu \n", th);
  }


  //FIN DU COMPTEUR
  chrono::steady_clock::time_point end = chrono::steady_clock::now();
  chrono::duration<long double> time_span = chrono::duration_cast<chrono::duration<long double>>(end-begin);
  long double exec_time = time_span.count();


  delete [] myFunctions;


  printf("\n");
  cout << "MAIN(), Fin des threads \n";
  printf("\n");


  printf("                                               ---------------------RESULTATS--------------------- \n");
  printf("\n");
  printf("                                                 L'aire finale est de %Lf uA pour %d itération(s) \n", final_integral, NBITERATION);
  printf("                                                 Le programme s'est exécuté en %Lf secondes \n", exec_time);
  printf("\n");
  printf("                                               --------------------------------------------------- \n");
  printf("\n");


  //ECRITURE DES RESULTATS DANS UN FICHIER TXT
  string filename = "resultats.txt";

  if( doExists(filename) )
    writeFileData(filename, NBITERATION, exec_time);
  else
  {
    try {
      createFile(filename);
      writeFileData(filename, NBITERATION, exec_time);
    }
    catch(exception& e) {
      cout << e.what() << endl;
    }
  }


  return(EXIT_SUCCESS);
}
