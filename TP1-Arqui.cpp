#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <iostream>

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
    srand (time(NULL));

    double startwtime, endwtime;

    MPI_Init(&argc,&argv); 						/*  Inicia el trabajo con MPI */
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);	/*  numprocs almacena número de procesos que puso usuario*/
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);		/*  MPI almacena en myid la identificación del proceso actual */
    MPI_Status status;


    int* sendcounts;
    int* displs;
    int* sendcounts_B;
    int* displs_B;

    if (myid == 0)	//indagación por el N
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

	int* la_matriz;
	int* la_matriz_B;
    int* el_vector_Q;
    int* el_vector_P;
    int el_vector_V[n];
    int primosGlobales = 0;


    if (myid == 0)	//asignación de memoria y lleno de la_matriz y el_vector_V, y de inf. para scatterv
    {
		la_matriz = new int[n*n];
		la_matriz_B = new int[n*n];
		el_vector_Q = new int[n];
		el_vector_P = new int[n];

        //Matriz M-----------------------------------------------------------------
        for (int i=0; i<n; i++)
            for (int j=0; j<n; j++)
                la_matriz[i*n+j] = rand() % 10;

        //Se despliega la matriz M
        printf("La Matriz M es: ");
        printf("\n");
        for (int i=0; i<n; i+=1)
        {
            for (int j=0; j<n; j+=1)
            {
                printf("%d  ", la_matriz[i*n+j]);
            }
            printf("\n");
        }

        //Vector V-----------------------------------------------------------------
        for (int i=0; i<n; i++)
        {
            el_vector_V[i] = rand() % 10;
        }
        //Se despliega el vector V
        printf("La Vector V es: \n");
        printVector(el_vector_V, n);
        printf("\n");


		//Asignación para procesos, de parciales de la matriz----------------------
		//Asignación del scatterv
		sendcounts = new int[numprocs];
		displs = new int[numprocs];
		sendcounts_B = new int[numprocs];
		displs_B = new int[numprocs];

		for (int i = 0; i < numprocs; i++)
		{
			sendcounts[i]= n * n/numprocs;
			displs[i] = i*n*n/numprocs;
		}
		//Para B
		sendcounts_B[0]= n * n/numprocs + n;
		displs_B[0] = 0;
		for (int i = 1; i < numprocs; i++)
		{
			sendcounts_B[i]= n * n/numprocs + 2*n;
			displs_B[i] = i*n*n/numprocs-n;
		}
		sendcounts_B[n-1]-=n;
    }




    //Todos los procesos hacen lo siguiente------------------------------------------------------------
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(el_vector_V, n, MPI_INT, 0, MPI_COMM_WORLD);

    int parcial_de_M[n*n/numprocs];
	int parcial_de_M_para_B[n * n/numprocs + 2*n];


    int Q_parcial[n/numprocs];
    int P_parcial[n];
    int B_parcial[n/numprocs*n];
    int primosLocales = 0;


    MPI_Scatterv(la_matriz, sendcounts,displs, MPI_INT, parcial_de_M, n*n/numprocs, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatterv(la_matriz, sendcounts_B,displs_B, MPI_INT, parcial_de_M_para_B, n*n, MPI_INT, 0, MPI_COMM_WORLD);


    /*printf("Las filas de M, para el proceso %d son: ", myid);
    printf("\n");
    for (int i=0; i<n*n/numprocs; i++)
    {
        if (i%n==0 && i!=0)
        {
            printf("\n");
        }
        printf("%d  ", parcial_de_M[i]);
    }
    printf("\n");*/

	printf("Las filas de M para B para el proceso %d son: ", myid);
    printf("\n");
	if (myid==0 || myid==numprocs-1)
	{
		for (int i=0; i<n * n/numprocs + n; i++)
		{
			if (i%n==0 && i!=0)
			{
				printf("\n");
			}
			printf("%d  ", parcial_de_M_para_B[i]);
		}
		printf("\n");
	}
	else
	{
		for (int i=0; i<n * n/numprocs + 2*n; i++)
		{
			if (i%n==0 && i!=0)
			{
				printf("\n");
			}
			printf("%d  ", parcial_de_M_para_B[i]);
		}
		printf("\n");
	}



    //Calculo de Q-----------------------------------------------------------------
    Q_parcial[0] = 0;
    P_parcial[0] = 0;
	for (int i=0; i<n; i++)
    {
        P_parcial[i] = 0;
    }
    int index = 0;
	int indexP = 0;
	int k = 0;
	if(myid != 0)
        k = n;
    for (int i=k; (i-k)<n * n/numprocs; i++)
    {
        if (i%n==0 && i!=k)
        {
            index += 1;
            Q_parcial[index] = 0;
            indexP = 0;
        }

        Q_parcial[index] += el_vector_V[i%n] * parcial_de_M_para_B[i];
        if(esPrimo(parcial_de_M_para_B[i]))
        {
            primosLocales++;
			P_parcial[indexP]++;
        }
		indexP++;
    }





	//Calculo de B-----------------------------------------------------------------
	bool medio = false;
    if(myid == 0)
    {
        for (int i=0; i<n * n/numprocs; i++)
        {
            if (i%n==0 && i!=0)
                medio = true;
            if(medio)
            {
                if(i >= (n*n)-n)//ultima fila
                {
					if(i%n == 0)
                    {
                        B_parcial[i] = parcial_de_M[i-n] + parcial_de_M[i] + parcial_de_M[i+1];
                    }
                    else if(i%n == (n-1))
                    {
                        B_parcial[i] = parcial_de_M[i-n] + parcial_de_M[i] + parcial_de_M[i-1];
                    }
                    else
                    {
                        B_parcial[i] = parcial_de_M[i-n] + parcial_de_M[i] + parcial_de_M[i-1] + parcial_de_M[i+1];
                    }
                }
                else//medio
                {
					if(i%n == 0)
                    {
                        B_parcial[i] = parcial_de_M_para_B[i-n] + parcial_de_M_para_B[i+n] + parcial_de_M[i] + parcial_de_M[i+1];
                    }
                    else if(i%n == (n-1))
                    {
                        B_parcial[i] = parcial_de_M_para_B[i-n] + parcial_de_M_para_B[i+n] + parcial_de_M[i] + parcial_de_M[i-1];
                    }
                    else
                    {
                        B_parcial[i] = parcial_de_M_para_B[i-n] + parcial_de_M_para_B[i+n] + parcial_de_M[i] + parcial_de_M[i-1] + parcial_de_M[i+1];
                    }
                }
            }
            else//primera
            {
				if(i%n == 0)
                {
                    B_parcial[i] = parcial_de_M[i] + parcial_de_M[i+1] +parcial_de_M_para_B[n];
                }
                else if(i%n == (n-1))
                {
                    B_parcial[i] = parcial_de_M[i]+ parcial_de_M[i-1] + parcial_de_M_para_B[n+i];
                }
                else
                {
                    B_parcial[i] = parcial_de_M[i-1] + parcial_de_M[i] + parcial_de_M[i+1] + parcial_de_M_para_B[i+n];
                }
            }
        }
    }
    else if (myid < numprocs-1)
    {
        for (int i=0; i<n * n/numprocs; i++)//medio
        {
			if(i%n == 0)
            {
                B_parcial[i] = parcial_de_M_para_B[i] + parcial_de_M_para_B[i+2*n] + parcial_de_M[i] + parcial_de_M[i+1];
            }
            else if(i%n == (n-1))
            {
                B_parcial[i] = parcial_de_M_para_B[i] + parcial_de_M_para_B[i+2*n] + parcial_de_M[i] + parcial_de_M[i-1];
            }
            else
            {
                B_parcial[i] = parcial_de_M_para_B[i] + parcial_de_M_para_B[i+2*n] + parcial_de_M[i] + parcial_de_M[i-1] + parcial_de_M[i+1];
            }
        }
    }
    else
    {
        int cantidadFilasRestantes = n/numprocs;
        for (int i=0; i<n * n/numprocs; i++)
        {
            if (i%n==0 && i!=0)
                cantidadFilasRestantes--;
            if(cantidadFilasRestantes == 1)//ultima fila
            {
				if(i%n == 0)
                {
                    B_parcial[i] = parcial_de_M_para_B[i] + parcial_de_M[i] + parcial_de_M[i+1];
                }
                else if(i%n == (n-1))
                {
                    B_parcial[i] = parcial_de_M_para_B[i] + parcial_de_M[i] + parcial_de_M[i-1];
                }
                else
                {
                    B_parcial[i] = parcial_de_M_para_B[i] + parcial_de_M[i] + parcial_de_M[i-1] + parcial_de_M[i+1];
                }
            }
            else//medio
            {
				if(i%n == 0)
                {
                    B_parcial[i] = parcial_de_M_para_B[i] + parcial_de_M_para_B[i+2*n] + parcial_de_M[i] + parcial_de_M[i+1];
                }
                else if(i%n == (n-1))
                {
                    B_parcial[i] = parcial_de_M_para_B[i] + parcial_de_M_para_B[i+2*n] + parcial_de_M[i] + parcial_de_M[i-1];
                }
                else
                {
                    B_parcial[i] = parcial_de_M_para_B[i] + parcial_de_M_para_B[i+2*n] + parcial_de_M[i] + parcial_de_M[i-1] + parcial_de_M[i+1];
                }
            }
        }
    }



    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(Q_parcial, n/numprocs, MPI_INT, el_vector_Q, n/numprocs, MPI_INT, 0, MPI_COMM_WORLD);
	for (int i=0; i<n; i++){
		MPI_Reduce(&P_parcial[i],&el_vector_P[i],1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
	}
	MPI_Gather(B_parcial, n*n/numprocs, MPI_INT, la_matriz_B, n*n/numprocs, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Reduce(&primosLocales,&primosGlobales,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    if (myid == 0)
    {
		//Se despliega la matriz B
        printf("La Matriz B es: ");
        printf("\n");
        for (int i=0; i<n; i+=1)
        {
            for (int j=0; j<n; j+=1)
            {
                printf("%d  ", la_matriz_B[i*n+j]);
            }
            printf("\n");
        }

        printf("La Vector Q es:\n"); printVector(el_vector_Q, n);
        printf("La Vector P es:\n"); printVector(el_vector_P, n);
        printf("Total de primos: %d\n", primosGlobales);


		//Matriz M
		std::ofstream matriz_m;
		matriz_m.open ("M.txt");
		for (int i=0; i<n; i+=1)
        {
            for (int j=0; j<n; j+=1)
            {
				matriz_m <<  la_matriz[i*n+j];
				matriz_m <<  " ";
            }
            matriz_m <<  "\n";
        }
		matriz_m.close();
		//Vector V
		std::ofstream vector_v;
		vector_v.open ("V.txt");
		for (int i=0; i<n; i+=1)
        {
			vector_v <<  el_vector_V[i];
			vector_v <<  " ";
		}
		vector_v.close();
		//Vector Q
		std::ofstream vector_Q;
		vector_Q.open ("Q.txt");
		for (int i=0; i<n; i+=1)
        {
			vector_Q <<  el_vector_Q[i];
			vector_Q <<  " ";
		}
		vector_Q.close();
		//Vector P
		std::ofstream vector_P;
		vector_P.open ("P.txt");
		for (int i=0; i<n; i+=1)
        {
			vector_P <<  el_vector_P[i];
			vector_P <<  " ";
		}
		vector_P.close();
		//Matriz B
		std::ofstream matriz_b;
		matriz_b.open ("B.txt");
		for (int i=0; i<n; i+=1)
        {
            for (int j=0; j<n; j+=1)
            {
				matriz_b <<  la_matriz_B[i*n+j];
				matriz_b <<  " ";
            }
            matriz_b <<  "\n";
        }
		matriz_b.close();
		endwtime = MPI_Wtime();
        //printf("Tiempo de ejecución = %f\n", endwtime-startwtime);
        //fflush( stdout );

    }
    MPI_Finalize();
    return 0;
}


