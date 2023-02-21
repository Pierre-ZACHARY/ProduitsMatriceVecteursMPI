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

## Temps d'exécution
- Tests effectué sur une machine avec un i78700k (6/12) @5ghz
- Temps moyen sur 10 runs

| 25% de zeros        | 256       | 512      | 1024    |
|---------------------|-----------|----------|---------|
| Sequential          | 0,147166  | 1,13563  | 11,9933 |
| Sequential + OpenMP | 0,115065  | 0,314042 | 1,95231 |
| MPI1                | 0,0326211 | 0,35955  | 2,55332 |
| MPI1 + OpenMP       | 0,0254049 | 0,22047  | 2,3115  |
| MPI2(RMA) + OpenMP  | 0,0559645 | 0,283473 | 2,96623 |
| LeaderFollower      | /         | /        | /       |

| 50% de zeros        | 256       | 512      | 1024    |
|---------------------|-----------|----------|---------|
| Sequential          | 0,188065  | 1,12755  | 11,2917 |
| Sequential + OpenMP | 0,0334624 | 0,21485  | 2,27652 |
| MPI1                | 0,0342452 | 0,272957 | 2,44741 |
| MPI1 + OpenMP       | 0,0324383 | 0,19225  | 2,24284 |
| MPI2(RMA) + OpenMP  | 0,0363033 | 0,284174 | 2,95035 |
| LeaderFollower      | /         | /        | /       |

#### Remarques: 
- On peut voir que sur cette machine la version séquentielle +OpenMp est plus rapide ; J'expliquerai cela par le fait qu'il n'y a pas de temps d'échanges de données. Sur un cluster où l'échange de données serait obligatoire, la version MPI serait sans doute plus rapide.
- De même pour la version LeaderFollower, ici Master occupe 1/6 de la puissance de calcul, et les workers 5/6, ce qui n'est pas optimal. Sur un cluster avec plusieurs centaines de cœurs, cette contraite ne se pose pas ( master ne prendra 1/100 de la puissance de calcul... ), et donc cette version devrait être plus intéressante sur un cluster, notamment si la puissance de calcul des différents coeurs est variable. ( Ce n'est pas le cas sur ma machine à priori, et donc il n'y a pas vraiment d'intérêt à répartir les lignes de cette façon car chaque worker va à peu près aussi vite ).

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

#### Version Leader/Follower avec système de job
- voir un exemple de run dans le fichier leader_follower.txt
- LeaderFollowerTemplate contient les commandes nécessaire pour spawn un nombre x de worker et merge leur communicateur avec le Master
- LeaderFollowerJobSystem étend le template pour y ajouter un système de tasks, il est fait pour pouvoir y customiser les tasks à envoyer / recevoir ( voir les fonctions virtual )
- LeaderFollowerMultMat étend justement le JobSystem pour 
  1) forger les messages ( voir les fonctions serialize / deserialize ) à envoyer du master vers les workers ( génère les deux matrices, Bcast la matrice B, puis répartis les lignes en tasks )
  2) Les tasks généré par getInitialData sont automatiquement envoyés aux workers par le JobSystem ( voir sender / listener )
  3) traiter les messages reçus par les worker ( faire le calcul d'une ou plusieurs lignes )
  4) récupérer les résultats des worker et les stocker dans la matrice finale 
