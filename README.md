# multi_thread_integrale


#CONSIGNES D'UTILISATION :

Le projet est fourni avec un makefile qui se chargera de fabriquer les exécutables nécessaires
Il est possible que ce dernier doive être adapté, ne marche que sous les systèmes linux/unix (debian, ubuntu...)
Une version de Gnuplot est necéssaire pour le programme plot_curve_thread.cpp

Les résultats des temps d'exécution sont écris dans des fichiers .txt pour les programmes integrale_thread.cpp et Projet_OpenMP.cpp


#TRAITEMENT AUTOMATIQUE DES RESULTATS
Pour les programmes integrale_thread.cpp et Projet_OpenMP.cpp
il est possible de fabriquer les exécutables :

   - plotCurvExec : compile et lance l'exécutable de        integrale_thread.cpp pour 1, 2, 4 et 8 threads, avec 100, 1000, 10000, 100000, 1000000, 10000000 itérations pour chaque nombre de threads. Le programme fabrique également une image illustrant les temps d'exécution pour les différents paramètres (avec gnuplot en utilisant le fichier config_courbe.dat)

   - fill_OpenMP_result : même chose mais cette fois pour Projet_OpenMP.cpp et aucun traçage de courbe.


#LANCEMENT MANUEL
 - Fabriquer l'exécutable prog_sequentiel / prog_thread / progOpenMP avec le makefile

 - Au moment de lancer l'exécutable :
   - Pour les version thread et OpenMp : renseigner dans l'ordre le nombre de threads et le nombre d'itérations
   - Pour la version séquentiel, renseigner uniquement le nombre d'itérations
