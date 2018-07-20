#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<stdlib.h>
#include<unistd.h>    //write


struct array_to_rpc{
  int *array;
  double x;
  int N;
};
struct results_from_rpc{
  double *marray;
  int minmax_arrray[2];
  double avg;
  int N;

};
struct array_to_rpc *toRPC  ;
struct results_from_rpc *fromRPC  ;

void initialize_data();

int main(int argc , char *argv[])
{
    toRPC= (struct array_to_rpc *)malloc(sizeof(struct array_to_rpc));
 	  fromRPC= (struct results_from_rpc *)malloc(sizeof(struct results_from_rpc));
    int sock;
    int readC;
    struct sockaddr_in server;
    int user_s;
    //action is the message sent to server
    // so he can decode what to calculate
    char action[1] ;


    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        puts("Could not create socket");
    }
    puts("Socket created");
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        exit(1);
    }
    puts("Connected");

    //call initialize_data function
    initialize_data();
    //keep alive tcp connection
    while(1)
    {
      do{
    		printf("\n  Menu:\n    1.Calculate min max (RPC)\n    2.Calculate average (RPC)\n    3.Calculate array multiplication (RPC)\n    4.Edit user data\n    0.EXIT\n");
    		scanf("%d",&user_s);
        switch (user_s) {
          case 1:
            //send identifier to server
            strncpy(action, "1", 1);
            write(sock , action , strlen(action));
            write(sock,&toRPC->N,sizeof(toRPC->N));
            write(sock,toRPC->array,sizeof(int)*toRPC->N);
            write(sock,&toRPC->x,sizeof(toRPC->x));
            readC = recv(sock , &fromRPC->minmax_arrray , sizeof(int)*2 , 0);
            printf("\n Max is %d \n Min is %d\n", fromRPC->minmax_arrray[0],fromRPC->minmax_arrray[1]);
            break;
          case 2:
            strncpy(action, "2", 1);
            write(sock , action , strlen(action));
            write(sock,&toRPC->N,sizeof(toRPC->N));
            write(sock,toRPC->array,sizeof(int)*toRPC->N);
            write(sock,&toRPC->x,sizeof(toRPC->x));
            readC = recv(sock , &fromRPC->avg , sizeof(double) , 0);
            printf("\nThe average is %.2lf \n",fromRPC->avg);
            break;
          case 3:
            strncpy(action, "3", 1);
            write(sock , action , strlen(action));
            write(sock,&toRPC->N,sizeof(toRPC->N));
            write(sock,toRPC->array,sizeof(int)*toRPC->N);
            write(sock,&toRPC->x,sizeof(toRPC->x));
            free(fromRPC->marray);
            readC = recv(sock , &fromRPC->N , sizeof(int) , 0);
            fromRPC->marray = (double *)malloc(fromRPC->N *sizeof(double));
            readC = recv(sock , fromRPC->marray , sizeof(double)*fromRPC->N , 0);
            for(int i=0;i<fromRPC->N;i++)
              printf("\nArray[%d] = %.2lf\n",i,fromRPC->marray[i]);
            break;
          case 4:
            initialize_data();
            break;
          case 0:
            return 0;
            break;
        }
    	}while(user_s != 0);
    }
    return 0;
}

void initialize_data(){
  int temp;
  free(toRPC->array);
  free(fromRPC->marray);
  puts("  Give length of the array:  ");
  scanf("%d",&temp);
  toRPC->N = temp;
  /*---- Allocate size for struct array ----*/
  toRPC->array = (int *)malloc(toRPC->N *sizeof(int));
  printf("\n  Give %d integers to fill the array:\n\t",temp); /* For collecting the list of elements from USER*/
  for(int i=0;i<temp;i++)
  scanf("%d",&toRPC->array[i]);
  puts("\n  Give a double number to multiply the array: ");
  scanf("%lf",&toRPC->x);
}
