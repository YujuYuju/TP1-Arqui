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

    MPI_Init(&argc,&argv); 						/*  Inicia el trabajo con MPI */
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);	/*  numprocs almacena número de procesos que puso usuario*/
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);		/*  MPI almacena en myid la identificación del proceso actual */
    MPI_Status status;
    int sendcounts[numprocs];  /* nuevo para Scatterv  para n procesos */
    int displs[numprocs];

    if (myid == 0)
    {
        startwtime = MPI_Wtime();
        while (n<=0)   //se ve si n es múltiplo de cantidad de procesos
        {
            printf("Digite el numero n (debe ser un multiplo de la cantidad de procesos):\n");
            fflush(stdout);
            scanf("%d",&n);
            if ((n % numprocs) == 0)
            {
                break;
            }
            else
            {
                n=0;
            }
        }
        printf("%d ", n);
        printf("\n");
    }

    int la_matriz[n][n];
	int el_vector_Q[n];
    int el_vector_V[n];	//todos los procesos tienen al vector V.
    int parcial_de_M[n];
    if (myid == 0)
    {
		//Matriz M-----------------------------------------------------------------
        for (int i=0; i<n; i++)
            for (int j=0; j<n; j++)
                la_matriz[i][j] = rand() % 10;

        //Se despliega la matriz M
        printf("La Matriz M es: ");
        printf("\n");
        for (int i=0; i<n; i+=1)
        {
            for (int j=0; j<n; j+=1)
            {
                printf("%d  ", la_matriz[i][j]);
            }
            printf("\n");
        }

        //Vector V-----------------------------------------------------------------
        for (int i=0; i<n; i++)
        {
            el_vector_V[i] = rand() % 10;
        }
        //Se despliega el vector V
        printf("La Vector V es: ");
        printf("\n");
        for (int i=0; i<n; i++)
        {
            printf("%d  ", el_vector_V[i]);
        }
        printf("\n");
    }

    //Todos los procesos hacen lo siguiente------------------------------------------------------------
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(el_vector_V, n, MPI_INT, 0, MPI_COMM_WORLD);

    //Vector Q
    for (int i = 0; i < numprocs; i++)
    {
        sendcounts[i]= n * n/numprocs;
        displs[i] = i*n*n/numprocs;
    }

    int Q_parcial[n * n/numprocs];
	MPI_Scatterv(&la_matriz[0][0], sendcounts,displs, MPI_INT, parcial_de_M, n*n, MPI_INT, 0, MPI_COMM_WORLD);
    
    printf("Las filas de M es para el proceso %d son: ", myid);
    printf("\n");
    for (int i=0; i<n * n/numprocs; i++)
    {
		if (i%n==0 && i!=0){
			printf("\n");
		}
		printf("%d  ", parcial_de_M[i]);
    }
    printf("\n");


    //Calculo de Q
    for (int i=0; i<n/numprocs; i++)
    {
        for (int j=0; j<n; j++)
        {
            //Q_parcial[i] += parcial_de_M[j+k]*el_vector_V[j];
			//printf("%d *", parcial_de_M[j]);
			//printf("%d \n", el_vector_V[j]);
        }
    }


    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(Q_parcial, n, MPI_INT, el_vector_Q, n, MPI_INT, 0, MPI_COMM_WORLD);


    if (myid == 0)
    {
		printf("La Vector Q es: ");
		printf("\n");
		for (int i=0; i<n; i++){
			printf("%d  ", el_vector_Q[i]);
		}
		printf("\n");

        endwtime = MPI_Wtime();
        //printf("Tiempo de ejecución = %f\n", endwtime-startwtime);
        //fflush( stdout );
    }

    MPI_Finalize();
    return 0;
}
