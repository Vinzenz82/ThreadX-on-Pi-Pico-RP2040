#include <stdio.h>
#include "tx_api.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"

//include SD lib begin
#include "f_util.h"
#include "ff.h"
#include "rtc.h"
#include "hw_config.h"
//include SD lib end


#if LIB_PICO_CYW43_ARCH == 1
#include "pico/cyw43_arch.h"
#else
#include "hardware/gpio.h"
#endif

/*
   This example application sets up an RTU server and handles modbus requests

   This server supports the following function codes:
   FC 01 (0x01) Read Coils
   FC 03 (0x03) Read Holding Registers
   FC 15 (0x0F) Write Multiple Coils
   FC 16 (0x10) Write Multiple registers
*/
#include "nanomodbus.h"

// The data model of this sever will support coils addresses 0 to 100 and registers addresses from 0 to 32
#define COILS_ADDR_MAX 100
#define REGS_ADDR_MAX 32

// Our RTU address
#define RTU_SERVER_ADDRESS 1

// A single nmbs_bitfield variable can keep 2000 coils
nmbs_bitfield server_coils = {0};
uint16_t server_registers[REGS_ADDR_MAX] = {0};

//--------------------------------------------------------------------------------------------------------

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
TX_THREAD               thread_2;
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
ULONG                   thread_2_counter;
// ULONG                   thread_2_messages_received;
// ULONG                   thread_3_counter;
// ULONG                   thread_4_counter;
// ULONG                   thread_5_counter;
// ULONG                   thread_6_counter;
// ULONG                   thread_7_counter;
ULONG                   modbus_read_call;
ULONG                   modbus_write_call;



/* Define thread prototypes.  */

void    thread_monitor_entry(ULONG thread_input);
void    thread_1_entry(ULONG thread_input);
void    thread_2_entry(ULONG thread_input);
// void    thread_3_and_4_entry(ULONG thread_input);
// void    thread_5_entry(ULONG thread_input);
// void    thread_6_and_7_entry(ULONG thread_input);
void callback_uart1_irq(void);

int32_t read_serial(uint8_t* buf, uint16_t count, int32_t byte_timeout_ms, void* arg);
int32_t write_serial(const uint8_t* buf, uint16_t count, int32_t byte_timeout_ms, void* arg);
nmbs_error handle_read_coils(uint16_t address, uint16_t quantity, nmbs_bitfield coils_out, uint8_t unit_id, void *arg);
nmbs_error handle_write_multiple_coils(uint16_t address, uint16_t quantity, const nmbs_bitfield coils, uint8_t unit_id, void *arg);
nmbs_error handler_read_holding_registers(uint16_t address, uint16_t quantity, uint16_t* registers_out, uint8_t unit_id, void *arg);
nmbs_error handle_write_multiple_registers(uint16_t address, uint16_t quantity, const uint16_t* registers, uint8_t unit_id, void *arg);

/* Define main entry point.  */

void demo_threadx(void)
{
    time_init();

    /* Enter the ThreadX kernel.  */    
    tx_kernel_enter();
}

/* Define what the initial system looks like.  */

void    tx_application_define(void *first_unused_memory)
{

CHAR    *pointer;


    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /********************************************* Initialize UART1 ************************************/
    uart_init(uart1, 115200);
    gpio_set_function(UART1_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART1_RX_PIN, GPIO_FUNC_UART);
    
    //irq_add_shared_handler (21, callback_uart1_irq, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
    uart_set_fifo_enabled(uart1, true);
    //irq_set_exclusive_handler(UART1_IRQ, callback_uart1_irq);
    //irq_set_enabled (UART1_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    //uart_set_irq_enables(uart1, true, false);
    ////////////////////////////////////////////////////////////////////////////////////////////////////

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


        // See FatFs - Generic FAT Filesystem Module, "Application Interface",




    // /* Allocate the stack for thread 2.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
             pointer, DEMO_STACK_SIZE, 
             16, 16, 4, TX_AUTO_START);

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

    gpio_init(16);
    gpio_set_dir(16, GPIO_OUT);
    gpio_put(16, 0);
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
        printf("           thread 1 messages sent:        %lu\n", thread_1_counter);
        printf("           thread 2 messages received:    %lu\n", thread_2_counter);
        // printf("           thread 3 obtained semaphore:   %lu\n", thread_3_counter);
        // printf("           thread 4 obtained semaphore:   %lu\n", thread_4_counter);
        // printf("           thread 5 events received:      %lu\n", thread_5_counter);
        // printf("           thread 6 mutex obtained:       %lu\n", thread_6_counter);
        // printf("           thread 7 mutex obtained:       %lu\n\n", thread_7_counter);
        printf("           modbus_read_call:       %lu\n", modbus_read_call);
        printf("           modbus_write_call:       %lu\n\n", modbus_write_call);

        /* Sleep for 100 ticks.  */
        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND * 10);

        // /* Set event flag 0 to wakeup thread 5.  */
        // status =  tx_event_flags_set(&event_flags_0, 0x1, TX_OR);

        // /* Check status.  */
        // if (status != TX_SUCCESS)
        //     break;

        server_registers[0] = thread_0_counter & 0x00FF;
        server_registers[1] = (thread_0_counter & 0xFF00) >> 16;


        server_registers[2] = thread_1_counter & 0x00FF;
        server_registers[3] = (thread_1_counter & 0xFF00) >> 16;

    }
}


void    thread_1_entry(ULONG thread_input)
{
    UINT    status;
    // ULONG   actual_flags;

    nmbs_platform_conf platform_conf;
    platform_conf.transport = NMBS_TRANSPORT_RTU;
    platform_conf.read = read_serial;
    platform_conf.write = write_serial;
    platform_conf.arg = NULL;

    nmbs_callbacks callbacks = {0};
    callbacks.read_coils = handle_read_coils;
    callbacks.write_multiple_coils = handle_write_multiple_coils;
    callbacks.read_holding_registers = handler_read_holding_registers;
    callbacks.write_multiple_registers = handle_write_multiple_registers;

    // Create the modbus server
    nmbs_t nmbs;
    nmbs_error err = nmbs_server_create(&nmbs, RTU_SERVER_ADDRESS, &platform_conf, &callbacks);
    if (err != NMBS_ERROR_NONE) {
        printf("Error: Can not create Modbus server. \n");
    }

    nmbs_set_read_timeout(&nmbs, 1000);
    nmbs_set_byte_timeout(&nmbs, 100);

    /* This thread simply sends messages to a queue shared by thread 2.  */
    while(1)
    {

        /* Increment the thread counter.  */
        thread_1_counter++;

        if (thread_1_counter % 2 == 0) {
            gpio_put(16, 1);
        } else {
            gpio_put(16, 0);
        }

        nmbs_server_poll(&nmbs);

        /* Sleep for 100 ticks.  */
        tx_thread_sleep(100);

        // /* Wait for event flag 0.  */
        // status =  tx_event_flags_get(&event_flags_0, 0x1, TX_OR_CLEAR, 
        //                                         &actual_flags, TX_WAIT_FOREVER);

        // /* Check status.  */
        // if ((status != TX_SUCCESS) || (actual_flags != 0x1))
        //     break;

        
    }
}


void    thread_2_entry(ULONG thread_input)
{

ULONG   received_message;
UINT    status;

    /* This thread retrieves messages placed on the queue by thread 1.  */
    while(1)
    {

        /* Increment the thread counter.  */
        thread_2_counter++;


     // See FatFs - Generic FAT Filesystem Module, "Application Interface",
    // http://elm-chan.org/fsw/ff/00index_e.html
    sd_card_t *pSD = sd_get_by_num(0);
    FRESULT fr = f_mount(&pSD->fatfs, pSD->pcName, 1);
    if (FR_OK != fr) panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
    FIL fil;
    const char* const filename = "filename.txt";
    fr = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);
    if (FR_OK != fr && FR_EXIST != fr)
        panic("f_open(%s) error: %s (%d)\n", filename, FRESULT_str(fr), fr);
    
    if (f_printf(&fil, "Hello, world!\n") < 0) {
        printf("f_printf failed\n");
    }
    fr = f_close(&fil);
    if (FR_OK != fr) {
        printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    }
    f_unmount(pSD->pcName);

    printf("File writing done.\n");

        /* Sleep for 5 seconds.  */
        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND * 5);

        // /* Retrieve a message from the queue.  */
        // status = tx_queue_receive(&queue_0, &received_message, TX_WAIT_FOREVER);

        // /* Check completion status and make sure the message is what we 
        //    expected.  */
        // if ((status != TX_SUCCESS) || (received_message != thread_2_messages_received))
        //     break;
        
        // /* Otherwise, all is okay.  Increment the received message count.  */
        // thread_2_messages_received++;
    }
}


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

int32_t read_serial(uint8_t* buf, uint16_t count, int32_t byte_timeout_ms, void* arg) {
    uint32_t timeout = byte_timeout_ms * 1000u;
    uint16_t bytes_read = 0;
    
    modbus_read_call++;

    if( uart_is_readable_within_us(uart1, timeout) )
    {
        uart_read_blocking(uart1, buf, count);
        bytes_read = count;
    }

    return bytes_read;
}


int32_t write_serial(const uint8_t* buf, uint16_t count, int32_t byte_timeout_ms, void* arg) {
    uint32_t timeout = byte_timeout_ms * 1000u;
    uint16_t bytes_left = count;

    modbus_write_call++;
    
    uart_write_blocking(uart1, buf, count);

    return count;
}

nmbs_error handle_read_coils(uint16_t address, uint16_t quantity, nmbs_bitfield coils_out, uint8_t unit_id, void *arg) {
  if (address + quantity > COILS_ADDR_MAX + 1)
    return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

  // Read our coils values into coils_out
  for (int i = 0; i < quantity; i++) {
    bool value = nmbs_bitfield_read(server_coils, address + i);
    nmbs_bitfield_write(coils_out, i, value);
  }

  return NMBS_ERROR_NONE;
}


nmbs_error handle_write_multiple_coils(uint16_t address, uint16_t quantity, const nmbs_bitfield coils, uint8_t unit_id, void *arg) {
  if (address + quantity > COILS_ADDR_MAX + 1)
    return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

  // Write coils values to our server_coils
  for (int i = 0; i < quantity; i++) {
    nmbs_bitfield_write(server_coils, address + i, nmbs_bitfield_read(coils, i));
  }

  return NMBS_ERROR_NONE;
}


nmbs_error handler_read_holding_registers(uint16_t address, uint16_t quantity, uint16_t* registers_out, uint8_t unit_id, void *arg) {
  if (address + quantity > REGS_ADDR_MAX + 1)
    return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

  // Read our registers values into registers_out
  for (int i = 0; i < quantity; i++)
    registers_out[i] = server_registers[address + i];

  return NMBS_ERROR_NONE;
}


nmbs_error handle_write_multiple_registers(uint16_t address, uint16_t quantity, const uint16_t* registers, uint8_t unit_id, void *arg) {
  if (address + quantity > REGS_ADDR_MAX + 1)
    return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

  // Write registers values to our server_registers
  for (int i = 0; i < quantity; i++)
    server_registers[address + i] = registers[i];

  return NMBS_ERROR_NONE;
}