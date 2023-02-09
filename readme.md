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

#### Pour le dockerfile : 
- docker build -t mpi .
- docker run -it mpi
- Ou via l'interface Clion : clic droit sur le dockerfile > run

## Exécution
Pour executer le programme il y a une config clion inclue, sinon vous pouvez le compiler via cmake et executer le résultat avec :
- "mpiexec -n 4 ./main.exe" pour windows
- "mpirun -n 4 ./main.exe" pour linux

