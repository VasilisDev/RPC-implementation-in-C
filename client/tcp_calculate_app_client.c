#include "tcp_calculate_app.h"
#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include <stdbool.h>
//#include <sys/types.h>
//#include <sys/syscall.h>



struct array_to_rpc{
  int *array;
  double x;
  int N;
};
struct results_from_rpc{
  double *marray;
  int minmax_arrray[2];
  double average;
  int N;
};



//the thread function
void *connection_handler(void *);
void initialize_data();

pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rpc_mutex = PTHREAD_MUTEX_INITIALIZER;

void prog_1(char *host,int user_case, void *listen_sock, struct array_to_rpc *toRPC,   struct results_from_rpc *fromRPC)
{
  int sock = *(int*)listen_sock;
	CLIENT *clnt;
	data_out  *result_1;
	double  *result_2;
	data_out  *result_3;

	//data_in  minmax_1_arg;
	//data_in  avg_1_arg;
	//data_in  multiply_1_arg;
	data_in in;
	in.array.array_val = (int *)malloc (sizeof(int)*toRPC->N);
  in.array.array_val = toRPC->array;
  in.array.array_len = toRPC->N;
	in.x_multiplier = toRPC->x;

#ifndef	DEBUG
	clnt = clnt_create (host, PROG, VER1, "tcp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */



	switch (user_case) {
		case 1:
  //  pthread_mutex_lock(&rpc_mutex);
		result_1 = minmax_1(&in, clnt);
  //  pthread_mutex_unlock(&rpc_mutex);
		if (result_1 == (data_out *) NULL) {
			clnt_perror (clnt, "call failed");
		}
    else{
      fromRPC->minmax_arrray[0] = result_1->array_min_max[0];
      fromRPC->minmax_arrray[1] = result_1->array_min_max[1];
    //  pthread_mutex_lock(&write_mutex);
      write(sock,&fromRPC->minmax_arrray,sizeof(int)*2);
    //  pthread_mutex_unlock(&write_mutex);

  }
    //Debug
		//printf("\n Max is %d \n Min is %d\n", fromRPC->minmax_arrray[0],fromRPC->minmax_arrray[1]);
    // END OF DEBUGGING
		break;
		case 2:
    //pthread_mutex_lock(&rpc_mutex);
		result_2 = avg_1(&in, clnt);
    //pthread_mutex_unlock(&rpc_mutex);
		if (result_2 == (double *) NULL) {
			clnt_perror (clnt, "call failed");
		}
    else{
      fromRPC->average = *result_2;
      //pthread_mutex_lock(&write_mutex);
      write(sock,&fromRPC->average,sizeof(double));
      //pthread_mutex_unlock(&write_mutex);
    }
    //Debug
    //printf("\nThe average is %.2lf \n",fromRPC->average);
    // END OF DEBUGGING
		break;
		case 3:
    //pthread_mutex_lock(&rpc_mutex);
		result_3 = multiply_1(&in, clnt);
    //pthread_mutex_unlock(&rpc_mutex);
		if (result_3 == (data_out *) NULL) {
			clnt_perror (clnt, "call failed");
		}
    else{
        fromRPC->N = result_3->multi_array.multi_array_len;
        fromRPC->marray = (double *)malloc(fromRPC->N *sizeof(double));
      	for(int i=0;i<result_3->multi_array.multi_array_len;i++)
      		fromRPC->marray[i]  = result_3->multi_array.multi_array_val[i];
      //    pthread_mutex_lock(&write_mutex);
        write(sock,&fromRPC->N,sizeof(int));
        write(sock,fromRPC->marray,sizeof(double)*fromRPC->N);
      //  pthread_mutex_unlock(&write_mutex);

    }
    //Debug
	   //	for(int i=0;i<fromRPC->N;i++)
     //		printf("\nArray[%d] = .2lf\n",i,fromRPC->marray[i]);
    // END OF DEBUGGING

		break;
	}
	#ifndef	DEBUG
		clnt_destroy (clnt);
	#endif	 /* DEBUG */
}




int main (int argc, char *argv[])
{
	//TCP SERVER AF inet

	int server_socket , client_sock , sizeof_s;
	struct sockaddr_in server , client;
	//Create socket
	server_socket = socket(AF_INET , SOCK_STREAM , 0);
	if (server_socket == -1)
	{
		puts("\nError: Could not create socket\n");
    exit(1);
	}
	puts("\nTCP Socket created\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );

	//Bind
	if( bind(server_socket,(struct sockaddr *)&server , sizeof(server))  != 0)
	{
		//print the error message
		perror("\nError: Binding unsuccessful\n");
		exit(1);
	}
	puts("\nBind successful\n");
	//Listen
	listen(server_socket , 128);
	if(listen(server_socket,128) != 0) {
		perror("\nListening unsuccessful\n");
		exit(1);
	}
	//Accept and incoming connection
	puts("\nWaiting for incoming connections...\n");
	sizeof_s = sizeof(struct sockaddr_in);
	pthread_t thread_id;
	while( (client_sock = accept(server_socket, (struct sockaddr *)&client, (socklen_t*)&sizeof_s)) )
	{
		puts("\nNew client connected\n");
		pthread_mutex_lock(&mutex);
				if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
				{
					perror("\nError: Could not create thread\n");
					exit(1);
				}
		pthread_mutex_unlock(&mutex);

		//Now join the thread , so that we dont terminate before the thread
		//pthread_join( thread_id , NULL);
		puts("\nHandler assigned to thread\n");
	}
	//END OF TCP SERVER IF inet


	return 0;
}

/*
* This will handle connection for each client
* */
void *connection_handler(void *c_socket)
{
	//Get the socket descriptor
	int sock = *(int*)c_socket;
	int read_size;
	char *message , client_action[2000];
	char *host = "localhost";
  struct array_to_rpc *toRPC = (struct array_to_rpc *)malloc(sizeof(struct array_to_rpc));
  struct results_from_rpc *fromRPC= (struct results_from_rpc *)malloc(sizeof(struct results_from_rpc));

	//Send some messages to the client


/*
	write(sock , message , strlen(message));

	message = "Now type something and i shall repeat what you type \n";
	write(sock , message , strlen(message));
	pthread_mutex_unlock(&mutex);
*/
	//Receive a message from client

//pid_t th_id ;

  while((read_size = recv(sock , client_action , 1 , 0)) > 0 )
	{

		switch (client_action[0]) {
			case 1:
      pthread_mutex_lock(&rpc_mutex);
    	initialize_data((void*) &sock,toRPC,fromRPC) ;
      prog_1("localhost",1,(void*) &sock,toRPC,fromRPC);
      pthread_mutex_unlock(&rpc_mutex);
    //  th_id = syscall(__NR_gettid);
    //  printf("Thread %d requested to calculate min/max for a given array\n",th_id );
			break;
			case 2:
      pthread_mutex_lock(&rpc_mutex);
      initialize_data((void*) &sock,toRPC,fromRPC) ;
      prog_1("localhost",2,(void*) &sock,toRPC,fromRPC);
      pthread_mutex_unlock(&rpc_mutex);
      // th_id = syscall(__NR_gettid);
    //  printf("Thread %d requested to calculate average for a given array\n",th_id );
			break;
			case 3:
      pthread_mutex_lock(&rpc_mutex);
      free(fromRPC->marray);
      initialize_data((void*) &sock,toRPC,fromRPC) ;
      prog_1("localhost",3,(void*) &sock,toRPC,fromRPC);
      pthread_mutex_unlock(&rpc_mutex);
    //  th_id = syscall(__NR_gettid);
    //  printf("Thread %d requested to calculate the multiplication for a given array with a given number\n",th_id );
      break;

		}
		//clear the message buffer
		memset(client_action, 0, 1);
	}
	if(read_size == 0)
	{
		puts("\nClient disconnected\n");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("\nFail to receive data ");
		exit(1);
  }
}
void initialize_data(void *listen_sock,   struct array_to_rpc *toRPC,   struct results_from_rpc *fromRPC){
	int sock = *(int*)listen_sock;
	int readC;
  free(toRPC->array);
  //pthread_mutex_lock(&read_mutex);
	readC = recv(sock , &toRPC->N , sizeof(int) , 0);
//  pthread_mutex_unlock(&read_mutex);
	if(!(readC > 0))
	{
		return;
	}
  toRPC->array = (int *)malloc(toRPC->N *sizeof(int));
  //pthread_mutex_lock(&read_mutex);
	readC = recv(sock , toRPC->array , sizeof(int)*toRPC->N , 0);
  //pthread_mutex_unlock(&read_mutex);
if(!(readC > 0))
	{
		return;
	}
  //pthread_mutex_lock(&read_mutex);
	readC = recv(sock , &toRPC->x , sizeof(double) , 0);
  //pthread_mutex_unlock(&read_mutex);
if(!(readC > 0))
	{
		return;
	}
  //DEBUGGING
/*
  for(int i=0;i<toRPC->N;i++)
	printf("array: %d\n", toRPC->array[i]);
	printf("n: %d\n", toRPC->N);
	printf("x: %lf\n", toRPC->x);
*/
  //END OF DEBUGGING
}
