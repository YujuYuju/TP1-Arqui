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
    int n = 0, myid, numprocs;
	srand (time(NULL)); /* initialize random seed: */
	
    double startwtime, endwtime;
	int* el_vector_V;	//todos los procesos tienen al vector V. Lo necesitan para calcular Q.
	int* el_vector_Q;
	
	int* parcial_de_M;
	int Q_sub_i = 0;
	int indice_columna = 0;
    
    MPI_Init(&argc,&argv); 						/*  Inicia el trabajo con MPI */
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);	/*  numprocs almacena número de procesos que puso usuario*/
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);		/*  MPI almacena en myid la identificación del proceso actual */
	MPI_Status status;
	MPI_Request req;

    if (myid == 0){
        startwtime = MPI_Wtime();
		while (n<=0) { //se ve si n es múltiplo de cantidad de procesos
			printf("Digite el numero n (debe ser un multiplo de la cantidad de procesos):\n");
            fflush(stdout);
            scanf("%d",&n);
			if ((n % numprocs) == 0){
				break;
			} else {
				n=0;
			}
		}
		
		int** la_matriz;
		la_matriz = new int* [n];
		el_vector_V = new int [n];
		el_vector_Q = new int [n];
		
		//Matriz M-----------------------------------------------------------------
		for (int i=0; i<n; i++){
			la_matriz[i] = crear_Fila_de_matriz(n);
		}
		//Se llenan con random las filas de la matriz M
		for (int j=0; j<n; j++){
			int* la_fila = la_matriz[j];
			for (int i=0; i<n; i++){
				la_fila[i] = rand() % 10;
			}
			la_matriz[j]=la_fila;
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
		
		
		//Vector V-----------------------------------------------------------------
		el_vector_V = crear_Fila_de_matriz(n);
		//Se llenan con random la fila del vector V
		for (int i=0; i<n; i++){
			el_vector_V[i] = rand() % 10;
		}
		//Se despliega el vector V
		printf("El Vector V es: ");
		printf("\n");
		for (int i=0; i<n; i++){
			printf("%d  ", el_vector_V[i]);
		}

		//Vector Q----------------------------------------------------------------
		//Se crean la fila del vector Q
		el_vector_Q = crear_Fila_de_matriz(n);
		
		//Asignación de filas para cada proceso
		for (int i=0; i<n; i++){
			for (int j=0; j<numprocs; j++){
				MPI_Send(la_matriz[i], n, MPI_INT, j, 55, MPI_COMM_WORLD);
				MPI_Send(&i, 1, MPI_INT, indice_columna, 56, MPI_COMM_WORLD);
			}
		}
	}
	//MPI_Barrier(MPI_COMM_WORLD); /* Barrera. */

	//Todos los procesos hacen lo siguiente------------------------------------------------------------
	MPI_Bcast(el_vector_V, n, MPI_INT, 0, MPI_COMM_WORLD);	//manda el contenido de el_vector_V a todos

	
	//Calculo de Q
	MPI_Recv(parcial_de_M, n, MPI_INT, 0, 55, MPI_COMM_WORLD, &status);
	MPI_Recv(&indice_columna, 1, MPI_INT, 0, 56, MPI_COMM_WORLD, &status);
	
	
	/* for (int i=0; i<n; i++){
		Q_sub_i += parcial_de_M[i]*el_vector_V[i];
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Reduce(&Q_sub_i, &el_vector_Q[indice_columna], 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); */
    
    if (myid == 0){
        endwtime = MPI_Wtime();
        printf("Tiempo de ejecución = %f\n", endwtime-startwtime);
        fflush( stdout );
    }
    
    MPI_Finalize();
    return 0;
}
