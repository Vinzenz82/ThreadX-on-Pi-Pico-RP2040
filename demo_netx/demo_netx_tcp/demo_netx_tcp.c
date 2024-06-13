/* This is a small ping demo of the high-performance NetX Duo TCP/IP stack.  */

#include "tx_api.h"
#include "nx_api.h"
#include "nxd_dhcp_client.h"


/* Define the ThreadX and NetX object control blocks...  */

NX_PACKET_POOL    pool_0;
NX_IP             ip_0;  
#ifdef NX_ENABLE_DHCP
NX_DHCP           dhcp_client;
UCHAR             ip_address[4];
UCHAR             network_mask[4];
TX_THREAD         thread_0;
UCHAR             thread_0_stack[2048];
#endif

/////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// tcp server ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
#define MULTI_SOCKET

TX_THREAD         thread_1;

UCHAR             thread_1_stack[2048];
#ifndef MULTI_SOCKET
    NX_TCP_SOCKET server_socket;
#else
    NX_TCP_SOCKET server_socket[3];
    static ULONG g_not_listening = TX_FALSE;
#endif

#define		SOCKET_PORT		15555
ULONG thread_1_counter;
/////////////////////////////////////////////////////////////////////////////////


/* Define the IP thread's stack area.  */

ULONG ip_thread_stack[2 * 1024 / sizeof(ULONG)];


/* Define packet pool for the demonstration.  */

#define NX_PACKET_POOL_SIZE ((1536 + sizeof(NX_PACKET)) * 50)

ULONG             packet_pool_area[NX_PACKET_POOL_SIZE/4 + 4];

/* Define the ARP cache area.  */

ULONG             arp_space_area[512 / sizeof(ULONG)];

                                                           
/* Define an error counter.  */

ULONG             error_counter;

#ifdef NX_ENABLE_DHCP
VOID    thread_0_entry(ULONG thread_input);
#endif

void thread_1_entry(ULONG thread_input);
void thread_1_connect_received(NX_TCP_SOCKET *server_socket, UINT port);
void thread_1_disconnect_received(NX_TCP_SOCKET *server_socket);
void g_tcp_sck_receive_cb(NX_TCP_SOCKET *p_server_socketsck);

VOID nx_driver_pico_w(NX_IP_DRIVER *driver_req_ptr);


/* Define main entry point.  */

void demo_threadx(void)
{

    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
}


/* Define what the initial system looks like.  */

void    tx_application_define(void *first_unused_memory)
{

UINT  status;
    
     
    /* Initialize the NetX system.  */
    nx_system_initialize();
    
    /* Create a packet pool.  */
    status =  nx_packet_pool_create(&pool_0, "NetX Main Packet Pool", 1536,  (ULONG*)(((int)packet_pool_area + 15) & ~15) , NX_PACKET_POOL_SIZE);

    /* Check for pool creation error.  */
    if (status)
    {
        printf("nx_packet_pool_create failed: %d \n", status);
        error_counter++;
    }
    /* Create an IP instance.  */
    status = nx_ip_create(&ip_0, 
                          "NetX IP Instance 0", 
#ifdef NX_ENABLE_DHCP
                          IP_ADDRESS(0,0,0,0),
                          IP_ADDRESS(0,0,0,0), 
#else
                          IP_ADDRESS(192, 168, 1, 139), 
                          0xFFFFFF00UL, 
#endif
                          &pool_0, nx_driver_pico_w,
                          (UCHAR*)ip_thread_stack,
                          sizeof(ip_thread_stack),
                          1);
    
    /* Check for IP create errors.  */
    if (status)
    {
        printf("nx_ip_create failed: %d \n", status);
        printf("NX_IP App: %d \n", (UINT)sizeof(NX_IP));
        error_counter++;
    }
        
    /* Enable ARP and supply ARP cache memory for IP Instance 0.  */
    status =  nx_arp_enable(&ip_0, (void *)arp_space_area, sizeof(arp_space_area));

    /* Check for ARP enable errors.  */
    if (status)
        error_counter++;

    /* Enable TCP traffic.  */
    status =  nx_tcp_enable(&ip_0);
    
    /* Check for TCP enable errors.  */
    if (status)
        error_counter++;
    
    /* Enable UDP traffic.  */
    status =  nx_udp_enable(&ip_0);
    
    /* Check for UDP enable errors.  */
    if (status)
        error_counter++;

    /* Enable ICMP.  */
    status =  nx_icmp_enable(&ip_0);
   
    /* Check for errors.  */
    if (status)
        error_counter++;   

    printf("Error count: %d \n", error_counter);

#ifdef NX_ENABLE_DHCP
    /* Create the main thread.  */
    tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
                     thread_0_stack, sizeof(thread_0_stack), 
                     4, 4, TX_NO_TIME_SLICE, TX_AUTO_START); 
#endif
}

#ifdef NX_ENABLE_DHCP

/* Define the test threads.  */
void    thread_0_entry(ULONG thread_input)
{
UINT    status;
ULONG   actual_status;
ULONG   temp;


    /* Create the DHCP instance.  */
    printf("DHCP In Progress...\n");

    nx_dhcp_create(&dhcp_client, &ip_0, "dhcp_client");

    /* Start the DHCP Client.  */
    nx_dhcp_start(&dhcp_client);
    
    /* Wait util address is solved. */
    status = nx_ip_status_check(&ip_0, NX_IP_ADDRESS_RESOLVED, &actual_status, 1000);
    
    if (status)
    {
        
        /* DHCP Failed...  no IP address! */
        printf("Can't resolve address\n");
    }
    else
    {
        
        /* Get IP address. */
        nx_ip_address_get(&ip_0, (ULONG *) &ip_address[0], (ULONG *) &network_mask[0]);

        /* Convert IP address & network mask from little endian.  */
        temp =  *((ULONG *) &ip_address[0]);
        NX_CHANGE_ULONG_ENDIAN(temp);
        *((ULONG *) &ip_address[0]) =  temp;
        
        temp =  *((ULONG *) &network_mask[0]);
        NX_CHANGE_ULONG_ENDIAN(temp);
        *((ULONG *) &network_mask[0]) =  temp;

        /* Output IP address. */
        printf("IP address: %d.%d.%d.%d\nMask: %d.%d.%d.%d\n", 
               (UINT) (ip_address[0]),
               (UINT) (ip_address[1]),
               (UINT) (ip_address[2]),
               (UINT) (ip_address[3]),               
               (UINT) (network_mask[0]),
               (UINT) (network_mask[1]),
               (UINT) (network_mask[2]),
               (UINT) (network_mask[3]));


                /**************************************************************
                 * 	Update from the original file
                 *
                 *************************************************************/
                tx_thread_create(&thread_1, "thread server", thread_1_entry, 0,
                                 thread_1_stack, sizeof(thread_1_stack),
                                3, 3, TX_NO_TIME_SLICE, TX_AUTO_START);
                //******************************************

   
    }
}

/**************************************************************
 * 	Update from the original file
 *
 *************************************************************/


void    thread_1_entry(ULONG thread_input)
{

UINT       status;
NX_PACKET *packet_ptr;
ULONG      actual_status;

    NX_PARAMETER_NOT_USED(thread_input);

    /* Wait 1 second for the IP thread to finish its initilization. */
    tx_thread_sleep(NX_IP_PERIODIC_RATE);

    /* Ensure the IP instance has been initialized.  */
    status =  nx_ip_status_check(&ip_0, NX_IP_INITIALIZE_DONE, &actual_status, NX_IP_PERIODIC_RATE);

    /* Check status...  */
    if (status != NX_SUCCESS)
    {
        printf("IP status not initialized, status: %d\n",status);
        error_counter++;
        return;
    }

    #ifndef MULTI_SOCKET
    /* Create a socket.  */
    status =  nx_tcp_socket_create(&ip_0, &server_socket, "Server Socket",
                                   NX_IP_NORMAL, NX_DONT_FRAGMENT, NX_IP_TIME_TO_LIVE, 100,
                                   NX_NULL, thread_1_disconnect_received);
    #else
        /* Create all sockets */
        for (INT i = 0; i < 3; i++)
        {
            status = nx_tcp_socket_create(&ip_0, &server_socket[i], "Multi Socket",
                                        NX_IP_NORMAL, NX_DONT_FRAGMENT,
                                        NX_IP_TIME_TO_LIVE, 512, NX_NULL,
                                        thread_1_disconnect_received);
        }
    #endif

    /* Check for error.  */
    if (status)
    {
    	printf("Socket not created, status: %d\n",status);
        error_counter++;
    }

    /* Setup this thread to listen.  */
    #ifndef MULTI_SOCKET
        status =  nx_tcp_server_socket_listen(&ip_0, SOCKET_PORT, &server_socket, 5, thread_1_connect_received);
    #else
        /* Start listening on the first socket */
        status =  nx_tcp_server_socket_listen(&ip_0, SOCKET_PORT, &server_socket[0], 0, thread_1_connect_received);
    #endif
    /* Check for error.  */
    if (status)
    {
    	printf("Error set socket to listen on port %d, status: %d\n",SOCKET_PORT, status);
        error_counter++;
    }

    /* Loop to create and establish server connections.  */
    while (1)
    {

        /* Increment thread 1's counter.  */
        thread_1_counter++;
        printf("Server waiting on port: %d \n", SOCKET_PORT);

        #ifdef MULTI_SOCKET
        /* Everything else is handled from TCP callbacks dispatched from
         * IP instance thread */
        tx_thread_suspend(tx_thread_identify());
        #else

        /* Accept a client socket connection.  */
        status =  nx_tcp_server_socket_accept(&server_socket, NX_WAIT_FOREVER);

        /* Check for error.  */
        if (status)
        {
        	printf("Error on server socket accept, status: %d\n",status);
            error_counter++;
        }
        printf("Server socket connected\n");

        while(!status)
        {
			/* Receive a TCP message from the socket.  */
			status =  nx_tcp_socket_receive(&server_socket, &packet_ptr, NX_WAIT_FOREVER);

			/* Check for error.  */
			if (status)
			{
				printf("Error on received packet, status: %d\n",status);
				error_counter++;
			}
			else
			{
				/* Release the packet.  */
//				PRINTF("Server socket received a packet\n");

				/* Send the packet out!  */
				status =  nx_tcp_socket_send(&server_socket, packet_ptr, NX_WAIT_FOREVER);

				nx_packet_release(packet_ptr);
			}
        }
        /* Disconnect the server socket.  */
        status =  nx_tcp_socket_disconnect(&server_socket, NX_IP_PERIODIC_RATE);

        /* Check for error.  */
        if (status)
        {
        	printf("Error on socket disconection, status: %d\n",status);
            error_counter++;
        }
        printf("Server socket disconnected\n");

        /* Unaccept the server socket.  */
        status =  nx_tcp_server_socket_unaccept(&server_socket);

        /* Check for error.  */
        if (status)
        {
        	printf("Error on socket unaccept, status: %d\n",status);
            error_counter++;
        }
        printf("Server socket unaccept\n");
        /* Setup server socket for listening again.  */
        status =  nx_tcp_server_socket_relisten(&ip_0, SOCKET_PORT, &server_socket);

        /* Check for error.  */
        if (status)
        {
        	printf("Error on socket relisten, status: %d\n",status);
            error_counter++;
        }
        #endif
    }
}


void  thread_1_connect_received(NX_TCP_SOCKET *socket_ptr, UINT port)
{
    #ifndef MULTI_SOCKET
    /* Check for the proper socket and port.  */
    if ((socket_ptr != &server_socket) || (port != SOCKET_PORT))
    {

        error_counter++;
    }
    else
    {
    	printf("packet received on port %d\n",port);
    }
    #else
    /* Incoming connection, accept and queueing new requests */
    printf("Accpet connection on public port: %d bind to local port: %d\n", port, socket_ptr->nx_tcp_socket_connect_port );
    nx_tcp_server_socket_accept(socket_ptr, NX_NO_WAIT);
    nx_tcp_server_socket_unlisten(&ip_0, port);
    nx_tcp_socket_receive_notify(socket_ptr, g_tcp_sck_receive_cb);

        /* Attempt to find another idle socket to start listening on */
    ULONG state = 0;

    for (INT i = 0; i < 3; i++)
    {
        /* Get socket state value */
        nx_tcp_socket_info_get(&server_socket[i],
                               0, 0, 0, 0, 0, 0, 0, &state, 0, 0, 0);

        /* Start lisnening if socket is idle */
        if (NX_TCP_CLOSED == state)
        {
            nx_tcp_server_socket_listen(&ip_0, port, &server_socket[i], 0,
                                        thread_1_connect_received);

            break;
        }
    }

    /* Ran out of sockets, set appropriate flag to let next socket to
     * disconnect know that it should start listening right away. */
    if (NX_TCP_CLOSED != state)
    {
        g_not_listening = TX_TRUE;
    }

    #endif
}

void g_tcp_sck_receive_cb(NX_TCP_SOCKET * socket_ptr)
{
    NX_PACKET * p_packet;
    UINT       status;
    ULONG length_rx = 0u;

    /* This callback is invoked when data is already received. Retrieving
     * packet with no suspension. */
    nx_tcp_socket_receive(socket_ptr, &p_packet, NX_NO_WAIT);

    length_rx = p_packet->nx_packet_length;

    char data[length_rx];

    memcpy(&data, p_packet->nx_packet_prepend_ptr, p_packet->nx_packet_length);
    nx_packet_release(p_packet);

    printf("Received %d data bytes on local port: %d\n", p_packet->nx_packet_length , socket_ptr->nx_tcp_socket_connect_port);
    printf("Received %s\n", data);

    /* Allocate a packet for client response. */
    status =  nx_packet_allocate(&pool_0, &p_packet, NX_TCP_PACKET, NX_NO_WAIT);

    if (status != NX_SUCCESS)
    {
        error_counter++;
        return;
    }

    /* append data */
    nx_packet_data_append(p_packet, "Response: ", 10, &pool_0, NX_NO_WAIT);
    nx_packet_data_append(p_packet, data, length_rx, &pool_0, NX_NO_WAIT);

    /* Send packet back on the same TCP socket */
    nx_tcp_socket_send(socket_ptr, p_packet, NX_NO_WAIT);

}

void  thread_1_disconnect_received(NX_TCP_SOCKET *socket)
{
    #ifndef MULTI_SOCKET
    /* Check for proper disconnected socket.  */
    if (socket != &server_socket)
    {
        error_counter++;
    }
    #else
        nx_tcp_server_socket_unaccept(socket);

        /* If all sockets are busy, start listening again */
        if (TX_TRUE == g_not_listening)
        {
            nx_tcp_server_socket_listen(&ip_0, SOCKET_PORT, socket, 0,
                                        thread_1_connect_received);

            g_not_listening = TX_FALSE;
        }
    #endif
}
#endif