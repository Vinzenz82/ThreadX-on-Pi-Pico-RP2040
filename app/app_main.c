#include <stdio.h>
#include "tx_api.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"

#if LIB_PICO_CYW43_ARCH == 1
#include "pico/cyw43_arch.h"
#else
#include "hardware/gpio.h"
#endif

//--------------------------------------------------------------------------------------------------------
//------------------------------------------------------------ DEFINES -----------------------------------
//--------------------------------------------------------------------------------------------------------

#define UART1_TX_PIN 4
#define UART1_RX_PIN 5

#define DEMO_STACK_SIZE         1024
#define DEMO_BYTE_POOL_SIZE     9120
#define DEMO_BLOCK_POOL_SIZE    100
#define DEMO_QUEUE_SIZE         100


//--------------------------------------------------------------------------------------------------------
//------------------------------------------------------------ CONTROL BLOCKS ----------------------------
//--------------------------------------------------------------------------------------------------------

/* Define the ThreadX object control blocks...  */

TX_THREAD               thread_monitor;
TX_THREAD               thread_1;
// TX_THREAD               thread_2;
// TX_THREAD               thread_3;
// TX_THREAD               thread_4;
// TX_THREAD               thread_5;
// TX_THREAD               thread_6;
// TX_THREAD               thread_7;
// TX_QUEUE                queue_0;
// TX_SEMAPHORE            semaphore_0;
// TX_MUTEX                mutex_0;
TX_EVENT_FLAGS_GROUP    event_flags_0;
TX_BYTE_POOL            byte_pool_0;
TX_BLOCK_POOL           block_pool_0;
UCHAR                   memory_area[DEMO_BYTE_POOL_SIZE];


/* Define the counters used in the demo application...  */

ULONG                   thread_0_counter;
ULONG                   thread_1_counter;
ULONG                   thread_1_messages_sent;
// ULONG                   thread_2_counter;
// ULONG                   thread_2_messages_received;
// ULONG                   thread_3_counter;
// ULONG                   thread_4_counter;
// ULONG                   thread_5_counter;
// ULONG                   thread_6_counter;
// ULONG                   thread_7_counter;


/* Define thread prototypes.  */

void    thread_monitor_entry(ULONG thread_input);
void    thread_1_entry(ULONG thread_input);
// void    thread_2_entry(ULONG thread_input);
// void    thread_3_and_4_entry(ULONG thread_input);
// void    thread_5_entry(ULONG thread_input);
// void    thread_6_and_7_entry(ULONG thread_input);
void callback_uart1_irq(void);



/* Define main entry point.  */

void demo_threadx(void)
{

    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
}


/* Define what the initial system looks like.  */

void    tx_application_define(void *first_unused_memory)
{

CHAR    *pointer;


    /* Initialize UART1 */
    uart_init(uart1, 115200);
    gpio_set_function(UART1_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART1_RX_PIN, GPIO_FUNC_UART);
    
    //irq_add_shared_handler (21, callback_uart1_irq, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
    uart_set_fifo_enabled(uart1, false);
    irq_set_exclusive_handler(UART1_IRQ, callback_uart1_irq);
    irq_set_enabled (UART1_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(uart1, true, false);

    /* Create a byte memory pool from which to allocate the thread stacks.  */
    tx_byte_pool_create(&byte_pool_0, "byte pool 0", memory_area, DEMO_BYTE_POOL_SIZE);

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    /* Allocate the stack for thread 0.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create the main thread.  */
    tx_thread_create(&thread_monitor, "thread monitor", thread_monitor_entry, 0,  
            pointer, DEMO_STACK_SIZE, 
            1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);


    /* Allocate the stack for thread 1.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create threads 1 and 2. These threads pass information through a ThreadX 
       message queue.  It is also interesting to note that these threads have a time
       slice.  */
    tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, DEMO_STACK_SIZE, 
            16, 16, 4, TX_AUTO_START);

    // /* Allocate the stack for thread 2.  */
    // tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    // tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
    //         pointer, DEMO_STACK_SIZE, 
    //         16, 16, 4, TX_AUTO_START);

    // /* Allocate the stack for thread 3.  */
    // tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    // /* Create threads 3 and 4.  These threads compete for a ThreadX counting semaphore.  
    //    An interesting thing here is that both threads share the same instruction area.  */
    // tx_thread_create(&thread_3, "thread 3", thread_3_and_4_entry, 3,  
    //         pointer, DEMO_STACK_SIZE, 
    //         8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

    // /* Allocate the stack for thread 4.  */
    // tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    // tx_thread_create(&thread_4, "thread 4", thread_3_and_4_entry, 4,  
    //         pointer, DEMO_STACK_SIZE, 
    //         8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

    // /* Allocate the stack for thread 5.  */
    // tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    // /* Create thread 5.  This thread simply pends on an event flag which will be set
    //    by thread_0.  */
    // tx_thread_create(&thread_5, "thread 5", thread_5_entry, 5,  
    //         pointer, DEMO_STACK_SIZE, 
    //         4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);

    // /* Allocate the stack for thread 6.  */
    // tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    // /* Create threads 6 and 7.  These threads compete for a ThreadX mutex.  */
    // tx_thread_create(&thread_6, "thread 6", thread_6_and_7_entry, 6,  
    //         pointer, DEMO_STACK_SIZE, 
    //         8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

    // /* Allocate the stack for thread 7.  */
    // tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    // tx_thread_create(&thread_7, "thread 7", thread_6_and_7_entry, 7,  
    //         pointer, DEMO_STACK_SIZE, 
    //         8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

    // /* Allocate the message queue.  */
    // tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_QUEUE_SIZE*sizeof(ULONG), TX_NO_WAIT);

    // /* Create the message queue shared by threads 1 and 2.  */
    // tx_queue_create(&queue_0, "queue 0", TX_1_ULONG, pointer, DEMO_QUEUE_SIZE*sizeof(ULONG));

    // /* Create the semaphore used by threads 3 and 4.  */
    // tx_semaphore_create(&semaphore_0, "semaphore 0", 1);

    /* Create the event flags group used by threads 1 and 5.  */
    tx_event_flags_create(&event_flags_0, "event flags 0");

    // /* Create the mutex used by thread 6 and 7 without priority inheritance.  */
    // tx_mutex_create(&mutex_0, "mutex 0", TX_NO_INHERIT);

    // /* Allocate the memory for a small block pool.  */
    // tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_BLOCK_POOL_SIZE, TX_NO_WAIT);

    // /* Create a block memory pool to allocate a message buffer from.  */
    // tx_block_pool_create(&block_pool_0, "block pool 0", sizeof(ULONG), pointer, DEMO_BLOCK_POOL_SIZE);

    // /* Allocate a block and release the block memory.  */
    // tx_block_allocate(&block_pool_0, (VOID **) &pointer, TX_NO_WAIT);

    /* Release the block back to the pool.  */
    tx_block_release(pointer);
}



/* Define the test threads.  */

void    thread_monitor_entry(ULONG thread_input)
{

UINT    status;

    // LED OFF
#if LIB_PICO_CYW43_ARCH == 1
    cyw43_arch_init();
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
#else
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
#endif

    /* This thread simply sits in while-forever-sleep loop.  */
    while(1)
    {

        /* Increment the thread counter.  */
        thread_0_counter++;

        if (thread_0_counter % 2 == 0) {
#if LIB_PICO_CYW43_ARCH == 1
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
#else
            gpio_put(PICO_DEFAULT_LED_PIN, 1);
#endif
        } else {
#if LIB_PICO_CYW43_ARCH == 1
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
#else
            gpio_put(PICO_DEFAULT_LED_PIN, 0);
#endif
        }

        /* Print results.  */
        printf("**** ThreadX Demonstration on Raspberry Pi Pico **** \n\n");
        printf("           thread 0 events sent:          %lu\n", thread_0_counter);
        printf("           thread 1 messages sent:        %lu\n\n", thread_1_counter);
        // printf("           thread 2 messages received:    %lu\n", thread_2_counter);
        // printf("           thread 3 obtained semaphore:   %lu\n", thread_3_counter);
        // printf("           thread 4 obtained semaphore:   %lu\n", thread_4_counter);
        // printf("           thread 5 events received:      %lu\n", thread_5_counter);
        // printf("           thread 6 mutex obtained:       %lu\n", thread_6_counter);
        // printf("           thread 7 mutex obtained:       %lu\n\n", thread_7_counter);

        /* Sleep for 50 ticks.  */
        tx_thread_sleep(100);

        // /* Set event flag 0 to wakeup thread 5.  */
        // status =  tx_event_flags_set(&event_flags_0, 0x1, TX_OR);

        // /* Check status.  */
        // if (status != TX_SUCCESS)
        //     break;
    }
}


void    thread_1_entry(ULONG thread_input)
{

    UINT    status;
    ULONG   actual_flags;

    /* This thread simply sends messages to a queue shared by thread 2.  */
    while(1)
    {

        /* Increment the thread counter.  */
        thread_1_counter++;

        /* Wait for event flag 0.  */
        status =  tx_event_flags_get(&event_flags_0, 0x1, TX_OR_CLEAR, 
                                                &actual_flags, TX_WAIT_FOREVER);

        /* Check status.  */
        if ((status != TX_SUCCESS) || (actual_flags != 0x1))
            break;

        uart_putc(uart1, 'B');
    }
}


// void    thread_2_entry(ULONG thread_input)
// {

// ULONG   received_message;
// UINT    status;

//     /* This thread retrieves messages placed on the queue by thread 1.  */
//     while(1)
//     {

//         /* Increment the thread counter.  */
//         thread_2_counter++;

//         /* Retrieve a message from the queue.  */
//         status = tx_queue_receive(&queue_0, &received_message, TX_WAIT_FOREVER);

//         /* Check completion status and make sure the message is what we 
//            expected.  */
//         if ((status != TX_SUCCESS) || (received_message != thread_2_messages_received))
//             break;
        
//         /* Otherwise, all is okay.  Increment the received message count.  */
//         thread_2_messages_received++;
//     }
// }


// void    thread_3_and_4_entry(ULONG thread_input)
// {

// UINT    status;


//     /* This function is executed from thread 3 and thread 4.  As the loop
//        below shows, these function compete for ownership of semaphore_0.  */
//     while(1)
//     {

//         /* Increment the thread counter.  */
//         if (thread_input == 3)
//             thread_3_counter++;
//         else
//             thread_4_counter++;

//         /* Get the semaphore with suspension.  */
//         status =  tx_semaphore_get(&semaphore_0, TX_WAIT_FOREVER);

//         /* Check status.  */
//         if (status != TX_SUCCESS)
//             break;

//         /* Sleep for 2 ticks to hold the semaphore.  */
//         tx_thread_sleep(2);

//         /* Release the semaphore.  */
//         status =  tx_semaphore_put(&semaphore_0);

//         /* Check status.  */
//         if (status != TX_SUCCESS)
//             break;
//     }
// }


// void    thread_5_entry(ULONG thread_input)
// {

// UINT    status;
// ULONG   actual_flags;


//     /* This thread simply waits for an event in a forever loop.  */
//     while(1)
//     {

//         /* Increment the thread counter.  */
//         thread_5_counter++;

//         /* Wait for event flag 0.  */
//         status =  tx_event_flags_get(&event_flags_0, 0x1, TX_OR_CLEAR, 
//                                                 &actual_flags, TX_WAIT_FOREVER);

//         /* Check status.  */
//         if ((status != TX_SUCCESS) || (actual_flags != 0x1))
//             break;
//     }
// }


// void    thread_6_and_7_entry(ULONG thread_input)
// {

// UINT    status;


//     /* This function is executed from thread 6 and thread 7.  As the loop
//        below shows, these function compete for ownership of mutex_0.  */
//     while(1)
//     {

//         /* Increment the thread counter.  */
//         if (thread_input == 6)
//             thread_6_counter++;
//         else
//             thread_7_counter++;

//         /* Get the mutex with suspension.  */
//         status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);

//         /* Check status.  */
//         if (status != TX_SUCCESS)
//             break;

//         /* Get the mutex again with suspension.  This shows
//            that an owning thread may retrieve the mutex it
//            owns multiple times.  */
//         status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);

//         /* Check status.  */
//         if (status != TX_SUCCESS)
//             break;

//         /* Sleep for 2 ticks to hold the mutex.  */
//         tx_thread_sleep(2);

//         /* Release the mutex.  */
//         status =  tx_mutex_put(&mutex_0);

//         /* Check status.  */
//         if (status != TX_SUCCESS)
//             break;

//         /* Release the mutex again.  This will actually 
//            release ownership since it was obtained twice.  */
//         status =  tx_mutex_put(&mutex_0);

//         /* Check status.  */
//         if (status != TX_SUCCESS)
//             break;
//     }
// }

void callback_uart1_irq(void)
{
    tx_event_flags_set(&event_flags_0, 0x1, TX_OR);
    printf("IRQ");

    while (uart_is_readable(uart1)) {
        uint8_t ch = uart_getc(uart1);
        // Can we send it back?
        if (uart_is_writable(uart1)) {
            // Change it slightly first!
            ch++;
            uart_putc(uart1, ch);
        }
        //chars_rxed++;
    }
}