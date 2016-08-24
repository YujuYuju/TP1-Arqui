
#include <mpi.h>
#include <stdio.h>
#include <math.h>

int*  crear_Fila_de_matriz(int numero_celdas) {
    int* fila = new int[numero_celdas];
    if (fila == NULL) {
        printf("Error al asignar memoria.");
        return NULL;
    } else {
        return fila;
    }
}
int main(int argc,char **argv)
{
    int n = 0, myid, numprocs, i;
    
    double mypi, pi, h, sum, x;
    double startwtime, endwtime;
    int  namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    
    MPI_Init(&argc,&argv); /*  Se inicia el trabajo con MPI */
    
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs); /*  MPI almacena en numprocs el número total de procesos que se pusieron a correr */
    
    MPI_Comm_rank(MPI_COMM_WORLD,&myid); /*  MPI almacena en myid la identificación del proceso actual */
    
    MPI_Get_processor_name(processor_name,&namelen);
    
    fprintf(stdout,"Proceso %d de %d en %s\n", myid, numprocs, processor_name); /*  Cada proceso despliega su identificación y el nombre de la computadora en la que corre*/
    
    MPI_Barrier(MPI_COMM_WORLD); /* Barrera de sincronizacion. Hasta que todos los procesos  alcancen este llamado ninguno puede proseguir.*/
    
    
    
    
    
    if (myid == 0){
        startwtime = MPI_Wtime();
        while (n <= 0){
            printf("Digite el numero n:  ");
            fflush(stdout);
            
            scanf("%d",&n);
        }
        int** la_matriz;
        la_matriz = new int* [n];
        
        for (int i=0; i<n; i++){
            la_matriz[i] = crear_Fila_de_matriz(n);
        }
        for (int j=0; j<n; j++){
            int* la_fila = la_matriz[j];
            for (int i=0; i<n; i++){
                la_fila[i] = 22;
            }
        }
        for (int j=0; j<n; j++){
            int* la_fila = la_matriz[j];
            for (int i=0; i<n; i++){
                printf("%d  ", la_fila[i]);
            }
            printf("\n");
        }
    }
    
    if (myid == 0){
        endwtime = MPI_Wtime(); /* Se toma el tiempo actual, para luego calcular la duración del cálculo por 
                                 diferencia con el tiempo inicial*/
        printf("wall clock time = %f\n", endwtime-startwtime);
        fflush( stdout );
    }
    
    MPI_Finalize();
    return 0;
}
