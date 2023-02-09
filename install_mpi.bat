rem Download Microsoft MPI (executable installer)
bitsadmin /transfer myDownloadJob /download /priority foreground http://download.microsoft.com/download/A/E/0/AE002626-9D9D-448D-8197-1EA510E297CE/msmpisetup.exe C:\Users\Public\Downloads\msmpisetup.exe
C:\Users\Public\Downloads\msmpisetup.exe -unattend -full

rem Download Microsoft MPI SDK (headers and libraries)
bitsadmin /transfer myDownloadJob /download /priority foreground https://download.microsoft.com/download/A/E/0/AE002626-9D9D-448D-8197-1EA510E297CE/msmpisdk.msi C:\Users\Public\Downloads\msmpisdk.msi
C:\Users\Public\Downloads\msmpisdk.msi /quiet
[Environment]::SetEnvironmentVariable("MPI_INCLUDE_PATH", "C:\Program Files (x86)\Microsoft SDKs\MPI\Include", "Machine")


