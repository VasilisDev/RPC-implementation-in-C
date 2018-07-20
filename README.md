# RPC-implementation-in-C
An application in C who calculate 2 mathematical expressions and find the minimum and maximum value in an array.It uses RPC and TCP/IP protocols for the communication and a stream of Sockets for security purposes.The server is concurrent server and he can serve multiple clients at the same time.
Follow the bellow commands to Run the app:

1 Compile the TCP-client side : gcc my_client.c -pthread -o myclient
2 Compile the RPC client-TCP server :  gcc -o TCPserverRPCclient tcp_calculate_app_client.c tcp_calculate_app_clnt.c tcp_calculate_app_xdr.c -lpthread -lnsl
3 Compile the RPC server:  gcc -o RPCserver tcp_calculate_app_server.c  tcp_calculate_app_svc.c tcp_calculate_app_xdr.c -lpthread -lnsl

That's it!You are ready to choose the mathematical expression that you want the server calculates.


