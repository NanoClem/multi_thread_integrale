#include <iostream>
#include <fstream>
#include <stdexcept>

using namespace std;



void createFile(string filename)
{
  ofstream ofs(filename.c_str(), ios::out);
  if(ofs)
    ofs.close();
  else
    throw runtime_error("Erreur : impossible de creer le fichier !");
}



void readFileData(string filename)
{
  string data = "";
  string prog_param = "";
  ifstream ifs(filename.c_str(), ios::in);
  if(ifs)
  {
    while( ifs.good() )   //tant qu'on est pas à la fin du fichier
    {
      getline(ifs, data);
      prog_param = "./progOpenMP " + data;
      system(prog_param.c_str());
    }
    ifs.close();
  }
  else
    throw runtime_error("Erreur : impossible de lire dans le fichier ! ");
}



void writeFileData(string filename, const unsigned int NBTHREADS, const unsigned int ITERATION)
{
  ofstream ofs;
  ofs.open(filename.c_str(), ios::out | ios::app);          // on écrit à la fin du fichier avec "app" pour éviter l'overwrite
  if(ofs)
  {
    ofs << NBTHREADS
        << " "
        << ITERATION
        << "\n";

    ofs.close();
  }
  else
    throw runtime_error("Erreur : impossible d'ecrire dans le fichier !");
}


void eraseContent(string filename)
{
  ofstream ofs(filename.c_str(), ios::out);
  ofs.close();
}




int main()
{
  unsigned int const NBTHREADS[4]    = {1, 2, 4, 8};
  unsigned int const NBITERATION[6]  = {100, 1000, 10000, 100000, 1000000, 10000000};
  string filename = "temp_OpenMp.txt";

  // On fabrique l'éxécutable du programme de calcul en utilisant le makefile
  system("make progOpenMP");

  try {
    //création du fichier contenant les arguments à passer au programme
    createFile(filename);
    for(size_t i = 0; i < 4; i++)
    {
        for(size_t j = 0; j < 6; j++)
          writeFileData(filename, NBTHREADS[i], NBITERATION[j]);

        readFileData(filename);
        eraseContent(filename);
    }
  }
  catch(exception& e) {
    cout << e.what() << endl;
  }

  system("rm temp_OpenMp.txt");
  return 0;
}
