Pierre ZACHARY

[![CppUnit Tests](https://github.com/Pierre-ZACHARY/ProduitsMatriceVecteursMPI/actions/workflows/test.yml/badge.svg)](https://github.com/Pierre-ZACHARY/ProduitsMatriceVecteursMPI/actions/workflows/test.yml)

## Pré-requis 

### Le programme a été testé sur windows, et sur ubuntu via le dockerfile.
#### Pour windows :
Vous devriez pouvoir installer les dépendances windows avec **install_mpi.bat**, il est possible que le bat se fasse bloquer par votre antivirus, dans ce cas vous pouvez juste copier coller les commandes dans un powershell admin :
- bitsadmin /transfer myDownloadJob /download /priority foreground http://download.microsoft.com/download/A/E/0/AE002626-9D9D-448D-8197-1EA510E297CE/msmpisetup.exe 
- C:\Users\Public\Downloads\msmpisetup.exe -unattend -full
- bitsadmin /transfer myDownloadJob /download /priority foreground https://download.microsoft.com/download/A/E/0/AE002626-9D9D-448D-8197-1EA510E297CE/msmpisdk.msi
- C:\Users\Public\Downloads\msmpisdk.msi /quiet
- **Vous devrez probablement rouvrir CLION après l'installation**

#### Pour macos : 
- brew install open-mpi
- brew install cmake
- brew install libomp
- il y a une config clion pour macos, vous pouvez trouver le chemin vers l'executable mpi avec "where mpirun"
- le chemin vers openmp est spécifié dans le cmake, il est possible que vous deviez le changer ( pour le trouver : find /usr/local -name "omp.h" )

#### Pour le dockerfile : 
- docker build -t mpi .
- docker run -it mpi
- Ou via l'interface Clion : clic droit sur le dockerfile > run

## Exécution
Pour executer le programme il y a une config clion inclue, sinon vous pouvez le compiler via cmake et executer le résultat avec :
- "mpiexec -n 4 ./main.exe" pour windows
- "mpirun -n 4 ./main.exe" pour linux

### Explication du code : 
- MPITemplate réalise le init / finalize / comm rank / comm size 
- Matrix est une classe template permettant de stocker différents types de variables ( int, float, double, etc... ), de les afficher / multiplier entre elles
- les différentes classes MPI Impl contiennent les différentes versions ci-dessous

#### Version MPI1
- initialisation des deux matrices sur root
- Scatterv, calcul du résultat locale, puis Gatherv 

#### Version MPI2
- Chaque processus calculs le nombre de lignes qu'il doit traiter et sa ligne de départ ( autant de lignes chacun +-1 )
- Initialisation des deux matrices sur root
- Ouverture d'une fenetre sur m1 et m2 sur Root uniquement ( null pour les autres )
- Ouverture d'une fenetre sur m3 local sur chaque processus, de la taille du nombre de lignes à traiter
- Chaque processus fait un Rget ( Get async, pour pouvoir en faire plusieurs en même temps ) de m2
- Chaque processus fait un Rget des lignes à traiter de m1 
- Chaque processsus calcul son m3_local, qui fait la taille du nombre de lignes à traiter
- Root va faire un Rget de m3_local de chaque processus pour récupérer le résultat final sur m3