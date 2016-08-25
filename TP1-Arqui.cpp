#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */


int main(int argc,char **argv)
{
    int n = 0, myid, numprocs;
	srand (time(NULL)); /* initialize random seed: */
	
    double startwtime, endwtime;
	
	int la_matriz[n][n];
	int el_vector_Q[n];
	int el_vector_V[n];	//todos los procesos tienen al vector V.
	int parcial_de_M[n];
	
    
    MPI_Init(&argc,&argv); 						/*  Inicia el trabajo con MPI */
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);	/*  numprocs almacena número de procesos que puso usuario*/
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);		/*  MPI almacena en myid la identificación del proceso actual */
	MPI_Status status;
	int sendcounts[numprocs];  /* nuevo para Scatterv  para n procesos */
    int displs[numprocs]; 
	
	

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
		

		//Matriz M-----------------------------------------------------------------
		for (int i=0; i<n; i++){
			for (int j=0; j<n; j++){
				la_matriz[i][j] = rand() % 10;
			}
		}
		//Se despliega la matriz M
		printf("La Matriz M es: ");
		printf("\n");
		for (int i=0; i<n; i++){
			for (int j=0; j<n; j++){
				printf("%d  ", la_matriz[i][j]);
			}
			printf("\n");
		}
		
		//Vector V-----------------------------------------------------------------
		for (int i=0; i<n; i++){
			el_vector_V[i] = rand() % 10;
		}
		//Se despliega el vector V
		printf("La Vector V es: ");
		printf("\n");
		for (int i=0; i<n; i++){
			printf("%d  ", el_vector_V[i]);
		}
		printf("\n");
	}
	 
	//Todos los procesos hacen lo siguiente------------------------------------------------------------
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(el_vector_V, n, MPI_INT, 0, MPI_COMM_WORLD);	
	
	//Vector Q
	for (int i = 0; i < numprocs; i++){
		sendcounts[i]= n + i;
		displs[i] = i*n - i;
	}

	int Q_parcial[n];
	MPI_Scatterv(la_matriz, sendcounts,displs, MPI_INT, parcial_de_M, sendcounts[myid], MPI_INT, 0, MPI_COMM_WORLD);
	

	printf("La fila de M es: ");
	printf("\n");
	for (int i=0; i<n; i++){
		printf("%d  ", parcial_de_M[i]);
	}
	printf("\n");
	
	
	//Calculo de Q
	for (int i=0; i<n; i++){
		for (int j=0; j<n; j++){
			Q_parcial[i] += parcial_de_M[j]*el_vector_V[j];
		}
	}
	
	
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Gather(Q_parcial, n, MPI_INT, el_vector_Q, n, MPI_INT, 0, MPI_COMM_WORLD);
	
    
    if (myid == 0){
/* 		printf("La Vector Q es: ");
		printf("\n");
		for (int i=0; i<n; i++){
			printf("%d  ", el_vector_Q[i]);
		}
		printf("\n"); */
	
        endwtime = MPI_Wtime();
        printf("Tiempo de ejecución = %f\n", endwtime-startwtime);
        fflush( stdout );
    }
    
    MPI_Finalize();
    return 0;
}
