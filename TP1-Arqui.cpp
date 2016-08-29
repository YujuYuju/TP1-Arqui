#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

void printVector(int row[], int n)
{
    for(int i = 0; i < n; i++)
        printf("%d  ", row[i]);
    printf("\n");
}

bool esPrimo(int n)
{
    switch(n)
    {
    case 1:
    case 2:
    case 3:
    case 5:
    case 7:
        return true;
        break;
    default:
        return false;
    }
}


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
	int sendcounts_B[numprocs];  /* nuevo para Scatterv  para n procesos */
    int displs_B[numprocs];

    if (myid == 0)
    {
        startwtime = MPI_Wtime();
        while (n<=0)   //se ve si n es múltiplo de cantidad de procesos
        {
            printf("Digite el numero n (debe ser un multiplo de la cantidad de procesos):\n");
            fflush(stdout);
            scanf("%d",&n);
            if (n >= numprocs && (n % numprocs) == 0)
                break;
            else
                n=0;
        }
        printf("%d ", n);
        printf("\n");
    }

    int la_matriz[n][n];
    int el_vector_Q[n];
    int el_vector_P[n];
    int el_vector_V[n];	//todos los procesos tienen al vector V.
    int primosGlobales = 0;
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
        //printf("La Vector V es: ");
        //printf("\n");
        for (int i=0; i<n; i++)
        {
            //printf("%d  ", el_vector_V[i]);
        }
        //printf("\n");
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

    int parcial_de_M[n*n/numprocs];
	int parcial_de_M_para_B[n*n];
	for (int i = 0; i < numprocs; i++)
    {
        sendcounts_B[i]= n * n/numprocs+n;
		if (i==0){
			displs_B[i] = i*n*n/numprocs;
		} else {
			displs_B[i] = i*n*n/numprocs-n;
		}
    }
	
    int Q_parcial[n/numprocs];
    int P_parcial[n/numprocs];
    int primosLocales = 0;
	
	
    MPI_Scatterv(&la_matriz[0][0], sendcounts,displs, MPI_INT, parcial_de_M, n*n/numprocs, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatterv(&la_matriz[0][0], sendcounts_B,displs_B, MPI_INT, parcial_de_M_para_B, n*n, MPI_INT, 0, MPI_COMM_WORLD);

    printf("Las filas de M es para el proceso %d son: ", myid);
    printf("\n");
    for (int i=0; i<n * n; i++)
    {
        if (i%n==0 && i!=0)
        {
            printf("\n");
        }
        printf("%d  ", parcial_de_M_para_B[i]);
    }
    printf("\n");


    //Calculo de Q
    Q_parcial[0] = 0;
    P_parcial[0] = 0;
    int index = 0;
    for (int i=0; i<n * n/numprocs; i++)
    {
        if (i%n==0 && i!=0)
        {
            index += 1;
            Q_parcial[index] = 0;
            P_parcial[index] = 0;
        }
        Q_parcial[index] += el_vector_V[i%n] * parcial_de_M[i];
        if(esPrimo(parcial_de_M[i]))
        {
            P_parcial[index] ++;
            primosLocales++;
        }

    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(Q_parcial, n/numprocs, MPI_INT, el_vector_Q, n/numprocs, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(P_parcial, n/numprocs, MPI_INT, el_vector_P, n/numprocs, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Reduce(&primosLocales,&primosGlobales,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    if (myid == 0)
    {
        //printf("La Vector Q es: ");
        //printf("\n");
        //printVector(el_vector_Q, n);
        //printf("La Vector P es: ");
        //printf("\n");
        //printVector(el_vector_P, n);
        //printf("Total de primos: ");
        //printf("\n");
        //printf("%d \n", primosGlobales);
        endwtime = MPI_Wtime();
        //printf("Tiempo de ejecución = %f\n", endwtime-startwtime);
        //fflush( stdout );
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}


