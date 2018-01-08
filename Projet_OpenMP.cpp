#include <omp.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdexcept>
#include <stdio.h>
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


//STRUCTURE DE DONNEES :
//Fonction polynomiale du 2nd degré

typedef struct
{
  long double min, max;		//Bornes minorante et majorante de la fonction
  double coef[3];         //Coefficients du polynôme
  float bmin, bmax;		    //Borne minimal et maximal recalculées à chaque trapèze
  long double Sum;	      //Somme final de l'intégrale
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

int main(int argc, char * argv[])
{
  printf("\n");

  // PARAMETRES DE CONSOLE
  if(argc != 3)                                                         // argv[0] contient le nom de l'exécutable
  {
    cout << "Nombre d'arguments invalide, veuillez en renseigner le nombre de threads PUIS le nombre d'iteration \n";
    cout << "\n";
    exit(0);
  }

  const unsigned int NBTHREADS = atoi(argv[1]);                    // Nombre de threads, atoi permet de convettir argv[] en entier
  const int NBITERATION        = atoi(argv[2]);                    // Nombre d'iteration
  float partSize               = NBITERATION  / NBTHREADS;		     // Nombre de trapèzes calculés par thread



  // CREATION D'UNE STRUCTURE POUR LA FONCTION
  Function * myFunction = new Function;

  //LA FONCTION POLYNOMIALE
  printf("\n");
  printFunction(myFunction);

  printf("\n");
  cout << "On va decouper le calcul en " << NBTHREADS << " thread(s) de " << NBITERATION/NBTHREADS << " trapèzes chacun" << endl;
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
  chrono::steady_clock::time_point begin = chrono::steady_clock::now();

  //LANCEMENT DES THREADS
  #pragma omp parallel sections num_threads(NBTHREADS)
  {

	#pragma omp section
	for(size_t i=0; i < size_t(NBITERATION); ++i)
	{
	  myFunction->bmin = (NBITERATION/NBTHREADS)*i;
      myFunction->bmax = (NBITERATION/NBTHREADS)*(i+1);
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

  //FIN DU COMPTEUR
  chrono::steady_clock::time_point end = chrono::steady_clock::now();
  chrono::duration<long double> time_span = chrono::duration_cast<chrono::duration<long double>>(end-begin);
  long double exec_time = time_span.count();

  delete [] myFunction;


  printf("                                               ---------------------RESULTATS--------------------- \n");
  printf("\n");
  printf("                                                 L'aire finale est de %Lf uA pour %d itération(s) \n", myFunction->Sum, NBITERATION);
  printf("                                                 Le programme s'est exécuté en %Lf secondes \n", exec_time);
  printf("\n");
  printf("                                               --------------------------------------------------- \n");
  printf("\n");


  //ECRITURE DES RESULTATS DANS UN FICHIER TXT
  string filename = "resultats_OpenMP.txt";

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
