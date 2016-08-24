
#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

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
     /* initialize random seed: */
	srand (time(NULL));
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
            printf("Digite el numero n: \n (debe ser un multiplo de la cantidad de procesos que definio)");
            fflush(stdout);
            
            scanf("%d",&n);
        
        int** la_matriz;
        la_matriz = new int* [n];
		int* el_vector;
		el_vector = new int [n];
        int* el_vector_Q;
		el_vector_Q = new int [n];
		
		//Matriz M
		//Se crean las filas de la matriz M
        for (int i=0; i<n; i++){
            la_matriz[i] = crear_Fila_de_matriz(n);
        }
		//Se llenan con random las filas de la matriz M
        for (int j=0; j<n; j++){
            int* la_fila = la_matriz[j];
            for (int i=0; i<n; i++){
                la_fila[i] = rand() % 10;
            }
        }
		//Se despliega la matriz M
		printf("La Matriz M es: ");
		printf("\n");
        for (int j=0; j<n; j++){
            int* la_fila = la_matriz[j];
            for (int i=0; i<n; i++){
                printf("%d  ", la_fila[i]);
            }
            printf("\n");
        }
		//Vector V
		//Se crean la fila del vector V
		el_vector = crear_Fila_de_matriz(n);
		//Se llenan con random la fila del vector V
		for (int i=0; i<n; i++){
                el_vector[i] = rand() % 10;
            }
		
		//Se despliega el vector V
		printf("La Vector V es: ");
		printf("\n");
            for (int i=0; i<n; i++){
                printf("%d  ", el_vector[i]);
            }

		//Vector V
		//Se crean la fila del vector Q
		el_vector_Q = crear_Fila_de_matriz(n);		
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
