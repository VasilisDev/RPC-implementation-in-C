/*
2018  ©  GEORGE MANGIOROS/ BILL TSAKIRIS
mg.geomag@gmail.com
vatsakiris@gmail.com
ALL RIGHTS RESERVED
THIS CODE IS FOR EDUCATIONAL USE
FOR COMMERCIAL USE REQUEST LICENCE FROM THE AUTHORS

    
 */
#include "tcp_calculate_app.h"

data_out *minmax_1_svc(data_in *argp, struct svc_req *rqstp)
{
	static data_out  result;
	puts("\n  RPC Server: calculate min/max for a given array\n");
	result.array_min_max[0] = argp->array.array_val[0];
	result.array_min_max[1] = argp->array.array_val[0];
	for(int i=1;i<argp->array.array_len;i++)
		if(result.array_min_max[0] < argp->array.array_val[i])
	   	result.array_min_max[0] = argp->array.array_val[i];
    else if(result.array_min_max[1] > argp->array.array_val[i])
	    result.array_min_max[1] = argp->array.array_val[i];
	return &result;
}

double *avg_1_svc(data_in *argp, struct svc_req *rqstp)
{
	static double  result;
	int sum=0;
	puts("\n  RPC Server: calculate the average for a given array\n");
	printf("result = %lf \n",result );
	for(int i=0;i<argp->array.array_len;i++)
	{
		printf("array[%d] = %d \n",i,  argp->array.array_val[i] );
	  sum += argp->array.array_val[i];
	}
	printf("array len %d \n", argp->array.array_len );
	result=sum/(double)argp->array.array_len;
	printf("average %lf \n", result );

	return &result;
}

data_out *multiply_1_svc(data_in *argp, struct svc_req *rqstp)
{
	static data_out  result;
	result.multi_array.multi_array_val=(double *)malloc(sizeof(double)*argp->array.array_len);
	result.multi_array.multi_array_len=argp->array.array_len;
	puts("\n  RPC Server: Calculate the multiplication of a given array with a given number\n");
	for(int i=0;i<argp->array.array_len;i++)
		{
		 result.multi_array.multi_array_val[i]=argp->array.array_val[i]*argp->x_multiplier;
		}
	return &result;
}
