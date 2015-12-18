/*
 * Copyright (c) 2015, Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * \file test_queue.c
 *
 * OS abstraction / tests of message queues API
 *
 *
 *
 */

/******* Framework headers : */
#include "os/os.h"
#include "os/os_types.h" /* framework-specific types */
#include "utility.h"
#include "util/cunit_test.h"
#include "test_task.h"
#include "utility.h"

#include "test_stub.h"
#include "zephyr/os_config.h"

extern void framework_init_queue(void); /* TODO: remove when OS_Abstraction provides single init function for all modules  */

/* #define __TEST_VERBOSE_ON */

/**********************************************************
 ************** Extern variables ************************
 **********************************************************/

/**********************************************************
 ************** Local definitions ************************
 **********************************************************/
#define _T(x) ((uint8_t*)x)



#define MSG_DEFAULT_TIMEOUT    100 /* ms */

#if   defined (ZEPHYR_NANO)
#define MSG_TIME_CONV_ERR      10 /* ms  -- errors could happen converting ticks to ms */
#elif defined  ZEPHYR_MICRO
#define MSG_TIME_CONV_ERR      1 /* ms  -- errors could happen converting ticks to ms */
#endif

#define MSG_TIMEOUT_ERROR      3 /* ms  -- allowed error when measuring the timeout on message reading */

#define TEST_MSG_MAX_SIZE              16
typedef uint8_t T_TEST_MESSAGE [TEST_MSG_MAX_SIZE];
#define TEST_QUEUE_TASK_DELAY          50    /* ms */
#define FUNCTIONAL_TEST_QID_MAIN_TASK        4
#define FUNCTIONAL_TEST_QID_TASK1_TX         5
#define FUNCTIONAL_TEST_QID_TASK2_TX         6
#define TEST_QUEUE_MAX_ITERATIONS     500/*(QUEUE_NB_OF_ELEMENETS + 1)*/


#define TASK1_MSG_HDR    _T("Task1_")
#define TASK2_MSG_HDR    _T("Task2_")

typedef enum {
    E_NO_TEST,
    E_UNIT_TESTS,
    E_SEND_MESSAGE,
}T_QUEUE_TEST_SELECTION;

#define LAST_QUEUE_INDEX               ( QUEUE_POOL_SIZE )
#define AVERAGE_ELEMENTS_IN_ONE_QUEUE  ( QUEUE_ELEMENT_POOL_SIZE / QUEUE_POOL_SIZE )
#define NB_ELEMENTS_IN_LAST_QUEUE      ( AVERAGE_ELEMENTS_IN_ONE_QUEUE + ( QUEUE_ELEMENT_POOL_SIZE % QUEUE_POOL_SIZE ) )
#define NB_ELEMENTS_OVERFLOW           ( QUEUE_ELEMENT_POOL_SIZE + 1 )
#define NOMINAL_QUEUE_SIZE             ( AVERAGE_ELEMENTS_IN_ONE_QUEUE - 1 )



static T_TEST_MESSAGE  g_TxBuffer [QUEUE_ELEMENT_POOL_SIZE + 1];
static T_TEST_MESSAGE* g_RxBuffer [QUEUE_ELEMENT_POOL_SIZE + 1];



/**********************************************************
 ************** Private variables ************************
 **********************************************************/
volatile T_QUEUE g_Q[QUEUE_POOL_SIZE + 1];

static uint32_t elapsedTimes [TEST_QUEUE_MAX_ITERATIONS];
static uint32_t avgElapsedTime, minElapsedTime, maxElapsedTime;
static uint32_t minElapsedIdx, maxElapsedIdx;
static T_QUEUE_TEST_SELECTION test_queue_test_type;

volatile uint32_t g_InterruptData;
static T_TEST_MESSAGE g_IsrMsgBuffer;

/**********************************************************
 ************** Forward declarations **********************
 **********************************************************/
static void set_message (T_TEST_MESSAGE msg, uint8_t* szMsg, uint16_t decVal);
static void increment_message (T_TEST_MESSAGE msg);
static bool compare_messages ( T_TEST_MESSAGE msgA, T_TEST_MESSAGE msgB);



/**********************************************************
 ************** Private functions ************************
 **********************************************************/


/**
 * \brief fill a T_TEST_MESSAGE with a zero-terminated string
 *        and a decimal value
 *
 * \param msg message to fill
 * \param szMsg zero-terminated char string to put in msg
 * \param decVal decimal value to be appended to msg ( must be [0..9] )
 */
static void set_message (T_TEST_MESSAGE msg, uint8_t* szMsg, uint16_t decVal)
{
    uint32_t idx = 0;

    /* copy szMsg to msg */
    while  ( (idx < (TEST_MSG_MAX_SIZE -4)) && (szMsg[idx] != 0) )
    {
        msg[idx] = szMsg[idx] ;
        idx ++ ;
    }
    /* idx = position of the zero byte in msg and szMsg...
     *       or TEST_MSG_MAX_SIZE-4 */

    /* append the decimal value and terminating zero */
    msg[idx   ] = '0' + (decVal % 10 );;
    msg[idx +1] = 0;  /* append zero byte */

}

/**
 * \brief increment the last char of a string
 *
 * \param msg message to increment
 */
static void increment_message (T_TEST_MESSAGE msg)
{
    uint32_t idx = 0;
    uint16_t decVal;

    /* find terminating zero */
    while  ( (idx < (TEST_MSG_MAX_SIZE -4)) && (msg[idx] != 0) )
    {
        idx ++ ;
    }
    /* idx = position of the zero byte in msg ...
     *       or TEST_MSG_MAX_SIZE-4 */

    if ( idx >= 3 ) /* 3 digits */
    {
        /* read the decimal value */
        decVal = (msg[idx - 1] - '0') ;

        /* add one */
        decVal ++ ;

        /* put back the decimal value */
        msg[idx - 1] = '0' + (decVal %10) ;
    }
}



/**
 * \brief return TRUE if the contents of 2 messages are equal
 *
 *  Comparison is done until byte n, where msgA[n] = 0, or
 *  n = TEST_MSG_MAX_SIZE (whichever comes first).
 *
 * \param msgA message to compare
 * \param msgB message to compare
 * \return true if contents are equal (up until msgA[n] = 0)
 *         false if at least one byte differs
 *
 */
static bool compare_messages ( T_TEST_MESSAGE msgA, T_TEST_MESSAGE msgB)
{
    uint8_t ctr;
    bool isEq = true;

    for ( ctr = 0 ; (ctr < TEST_MSG_MAX_SIZE) && ( msgA[ctr] != 0 ); ctr ++ )
    {
        if ( msgA[ctr] != msgB[ctr] )
        {
            isEq = false;
        }
    }
    return (isEq);
}



/**********************************************************
 ************** Exported functions ************************
 **********************************************************/

/**
 * \brief Initialization function to be called before running
 *   tests on message queues
 */
void test_queue_init(void)
{
    framework_init_queue();
    test_queue_test_type = E_NO_TEST;
}



/**
 * \brief Check parameters verification
 *
 * Remark: at the end of this tests, all available queues
 *         from the pool shall be in use.
 *         => indexOfLastCreatedQ = nb queue available
 */
void test_queue_unit_testing (void)
{
    OS_ERR_TYPE osErr;
    T_QUEUE myQ;
    T_TEST_MESSAGE  msgBufferTx;
    T_TEST_MESSAGE* msgBufferRx;
    uint32_t startTime, endTime ;
    uint32_t indexOfLastCreatedQ = 0 ;

    /* -----------  queue_create */
    osErr = E_OS_ERR_UNKNOWN;
    g_Q[indexOfLastCreatedQ] = queue_create(0, &osErr);
    CU_ASSERT("Unit Test: queue_create, maxSize = 0", osErr == E_OS_ERR);
    g_Q[indexOfLastCreatedQ] = queue_create(0, NULL);
    CU_ASSERT("Test: queue_create, maxSize = 0, err = NULL - panic", did_panic() == true);
    clear_panic();

    osErr = E_OS_ERR_UNKNOWN;
    g_Q[indexOfLastCreatedQ] = queue_create(-1, &osErr);
    CU_ASSERT("Unit Test: queue_create, maxSize = -1", osErr == E_OS_ERR);
    g_Q[indexOfLastCreatedQ] = queue_create(-1, NULL);
    CU_ASSERT("Unit Test: queue_create, maxSize = -1, err = NULL - panic", did_panic() == true);
    clear_panic();

    osErr = E_OS_ERR_UNKNOWN;
    g_Q[indexOfLastCreatedQ] = queue_create(0x7FFFFFFF, &osErr);
    CU_ASSERT("Unit Test: queue_create, maxSize = 0x7FFFFFFF", osErr == E_OS_ERR);
    g_Q[indexOfLastCreatedQ] = queue_create(0x7FFFFFFF, NULL);
    CU_ASSERT("Unit Test: queue_create, maxSize = 0x7FFFFFFF, err = NULL - panic",  did_panic() == true);
    clear_panic();

    osErr = E_OS_ERR_UNKNOWN;
    g_Q[indexOfLastCreatedQ] =  queue_create(NOMINAL_QUEUE_SIZE, &osErr);  /* nominal call */
    CU_ASSERT("Unit Test: queue_create nominal", osErr == E_OS_OK);
    indexOfLastCreatedQ ++ ;

    g_Q[indexOfLastCreatedQ] =  queue_create(NOMINAL_QUEUE_SIZE, NULL);
    CU_ASSERT("Unit Test: queue_create nominal, err = NULL ", did_panic() == false);
    indexOfLastCreatedQ ++ ;
    for ( /* :) */ ; indexOfLastCreatedQ < LAST_QUEUE_INDEX; indexOfLastCreatedQ ++)
    {
        g_Q[indexOfLastCreatedQ] =  queue_create(NOMINAL_QUEUE_SIZE, &osErr);  /* nominal call */
        CU_ASSERT("Test: queue_create nominal", osErr == E_OS_OK);
    }

    osErr = E_OS_ERR_UNKNOWN;
    myQ = queue_create(NOMINAL_QUEUE_SIZE, &osErr);
    CU_ASSERT("Unit Test: queue_create all resources in use", osErr == E_OS_ERR);

    myQ =  queue_create(NOMINAL_QUEUE_SIZE, NULL);
    CU_ASSERT("Unit Test: queue_create all resources in use, err = NULL - panic", did_panic() == true);
    clear_panic();

    /* -----------  queue_get */
    osErr = E_OS_ERR_UNKNOWN;
    queue_get_message (NULL, (T_QUEUE_MESSAGE*) &msgBufferRx, OS_NO_WAIT, &osErr);
    CU_ASSERT("Unit Test: queue_get_message, queue = NULL", osErr == E_OS_ERR);
    queue_get_message (NULL, (T_QUEUE_MESSAGE*) &msgBufferRx, OS_NO_WAIT, NULL);
    CU_ASSERT("Unit Test: queue_get_message, queue = NULL, err = NULL - panic", did_panic() == true);
    clear_panic();

    osErr = E_OS_ERR_UNKNOWN;
    queue_get_message ((T_QUEUE)42, (T_QUEUE_MESSAGE*) &msgBufferRx, OS_NO_WAIT, &osErr);
    CU_ASSERT("Unit Test: queue_get_message, queue = invalid", osErr == E_OS_ERR);
    queue_get_message ((T_QUEUE)42, (T_QUEUE_MESSAGE*) &msgBufferRx, OS_NO_WAIT, NULL);
    CU_ASSERT("Unit Test: queue_get_message, queue = invalid, err = NULL - panic", did_panic() == true);
    clear_panic();

    osErr = E_OS_ERR_UNKNOWN;
    queue_get_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE*) NULL, OS_NO_WAIT, &osErr);
    CU_ASSERT("Unit Test: queue_get_message, message = NULL", osErr == E_OS_ERR);
    queue_get_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE*) NULL, OS_NO_WAIT, NULL);
    CU_ASSERT("Unit Test: queue_get_message, message = NULL, err = NULL - panic", did_panic() == true);
    clear_panic();

    osErr = E_OS_ERR_UNKNOWN;
    queue_get_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE*) &msgBufferRx, -46, &osErr);
    CU_ASSERT("Unit Test: queue_get_message, timeout = invalid", osErr == E_OS_ERR);
    queue_get_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE*) &msgBufferRx, -46, NULL);
    CU_ASSERT("Unit Test: queue_get_message, timeout = invalid, err = NULL - panic", did_panic() == true);
    clear_panic();

    osErr = E_OS_ERR_UNKNOWN;
    queue_get_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE*) &msgBufferRx, OS_NO_WAIT, &osErr);
    CU_ASSERT("Unit Test: queue_get_message on empty queue, NO_WAIT", osErr == E_OS_ERR_EMPTY);
    queue_get_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE*) &msgBufferRx, OS_NO_WAIT, NULL);
    CU_ASSERT("Unit Test: queue_get_message on empty queue, NO_WAIT, err = NULL - no panic", did_panic() == false);

    osErr = E_OS_ERR_UNKNOWN;
    startTime = get_time_ms();
    queue_get_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE*) &msgBufferRx, MSG_DEFAULT_TIMEOUT, &osErr);
    endTime = get_time_ms();
    CU_ASSERT("Unit Test: queue_get_message on empty queue, timeout = 100ms, error code", osErr == E_OS_ERR_TIMEOUT);
    CU_ASSERT("Unit Test: queue_get_message on empty queue, timeout = 100ms, elapsed time max", (endTime - startTime) >= (MSG_DEFAULT_TIMEOUT - MSG_TIME_CONV_ERR));
    CU_ASSERT("Unit Test: queue_get_message on empty queue, timeout = 100ms, elapsed time max", (endTime - startTime) <= (MSG_DEFAULT_TIMEOUT + MSG_TIMEOUT_ERROR));

    osErr = E_OS_ERR_UNKNOWN;
    queue_get_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE*) &msgBufferRx, -12, &osErr);
    CU_ASSERT("Unit Test: queue_get_message on empty queue, negative timeout, error code", osErr == E_OS_ERR);

    /* -----------  queue_send */
    osErr = E_OS_ERR_UNKNOWN;
    queue_send_message (NULL, (T_QUEUE_MESSAGE) msgBufferTx, &osErr);
    CU_ASSERT("Unit Test: queue_send_message with queue = NULL", osErr == E_OS_ERR);
    queue_send_message (NULL, (T_QUEUE_MESSAGE) msgBufferTx, NULL);
    CU_ASSERT("Unit Test: queue_send_message, queue = NULL, err = NULL - panic", did_panic() == true);
    clear_panic();

    osErr = E_OS_ERR_UNKNOWN;
    queue_send_message ((T_QUEUE)42, (T_QUEUE_MESSAGE) msgBufferTx, &osErr);
    CU_ASSERT("Unit Test: queue_send_message with queue = invalid", osErr == E_OS_ERR);
    queue_send_message ((T_QUEUE)42, (T_QUEUE_MESSAGE) msgBufferTx, NULL);
    CU_ASSERT("Unit Test: queue_send_message, queue = invalid, err = NULL - panic", did_panic() == true);
    clear_panic();

    osErr = E_OS_ERR_UNKNOWN;
    queue_send_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE) NULL, &osErr);
    CU_ASSERT("Unit Test: queue_send_message with message = NULL", osErr == E_OS_OK);

    /* -----------  queue_delete */
    osErr = E_OS_ERR_UNKNOWN;
    queue_delete ((T_QUEUE)NULL, &osErr);
    CU_ASSERT("Unit Test: queue_delete, queue = NULL", osErr == E_OS_ERR);

    queue_delete ((T_QUEUE)NULL, NULL);
    CU_ASSERT("Unit Test: queue_delete, queue = NULL - panic", did_panic() == true);
    clear_panic();

    osErr = E_OS_ERR_UNKNOWN;
    queue_delete ((T_QUEUE)42, &osErr);
    CU_ASSERT("Unit Test: queue_delete, queue = invalid", osErr == E_OS_ERR);

    queue_delete ((T_QUEUE)42, NULL);
    CU_ASSERT("Unit Test: queue_delete, queue = invalid - panic", did_panic() == true);
    clear_panic();

    osErr = E_OS_ERR_UNKNOWN;
    queue_send_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE) msgBufferTx, &osErr); /* send a message on the last created queue */
    CU_ASSERT("Unit Test: queue_send_message nominal call", osErr == E_OS_OK);

    osErr = E_OS_ERR_UNKNOWN;
    queue_delete (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], &osErr); /* delete last created queue */
    CU_ASSERT("Unit Test: queue_delete nominal call, queue is not empty, error code", osErr == E_OS_OK);

    osErr = E_OS_ERR_UNKNOWN;
    queue_get_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE*) &msgBufferRx, OS_NO_WAIT, &osErr); /* get message on deleted queue */
    CU_ASSERT("Unit Test: queue_get_message on deleted queue", osErr == E_OS_ERR);

    osErr = E_OS_ERR_UNKNOWN;
    queue_send_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE) msgBufferTx, &osErr); /* send message on deleted queue */
    CU_ASSERT("Unit Test: queue_send_message on deleted queue", osErr == E_OS_ERR);

    osErr = E_OS_ERR_UNKNOWN;
    queue_delete (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], &osErr);
    CU_ASSERT("Unit Test: queue_delete, queue already deleted ",  osErr == E_OS_ERR);

    queue_delete (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], NULL); /* delete a deleted queue */
    CU_ASSERT("Unit Test: queue_delete, queue already deleted - panic", did_panic() == true);
    clear_panic();

    /* clean-up */
    for ( indexOfLastCreatedQ = 0 ; indexOfLastCreatedQ < LAST_QUEUE_INDEX; indexOfLastCreatedQ ++)
    {
        queue_delete(g_Q[indexOfLastCreatedQ], &osErr);  /* nominal call */
    }
}


/**
 * \brief Check the messages are received in their sending order
 */
void test_queue_functional_testing_message_order(void)
{
    uint32_t msgCtr ;
    OS_ERR_TYPE osErr ;
    uint32_t startTime, endTime ;

    /* initialize the messages to send */
    for ( msgCtr = 0; msgCtr < NOMINAL_QUEUE_SIZE; msgCtr ++ )
    {
        set_message(g_TxBuffer[msgCtr], _T("msg_"), msgCtr);
        g_RxBuffer[msgCtr] = NULL;
    }

    osErr = E_OS_ERR;
    g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK] = queue_create(NOMINAL_QUEUE_SIZE, &osErr);
    if ( E_OS_OK != osErr ) cu_print("Test error: failed to create FUNCTIONAL_TEST_QID_MAIN_TASK\n");


    /* fill queue */
    for (msgCtr = 0; msgCtr < NOMINAL_QUEUE_SIZE; msgCtr ++ )
    {
        osErr = E_OS_ERR_UNKNOWN;
        queue_send_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE) g_TxBuffer[msgCtr], &osErr);
        CU_ASSERT("Functional Test - message order : queue_send_message nominal call", osErr == E_OS_OK);
    }

    /* read queue */
    for (msgCtr = 0; msgCtr < NOMINAL_QUEUE_SIZE; msgCtr ++ )
    {
        osErr = E_OS_ERR_UNKNOWN;
        startTime = get_time_us();
        queue_get_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE*) &g_RxBuffer[msgCtr], MSG_DEFAULT_TIMEOUT, &osErr);
        endTime = get_time_us();
        CU_ASSERT("Functional Test: queue_get_message nominal call, same task", osErr == E_OS_OK);
        CU_ASSERT("Functional Test: queue_get_message nominal call, same task, elapsed time min", ((endTime - startTime) / 1000) <= (MSG_TIMEOUT_ERROR + MSG_TIME_CONV_ERR));
        if(osErr == E_OS_OK)
            CU_ASSERT("Functional Test: queue_get_message nominal call, same task", compare_messages(*(g_RxBuffer[msgCtr]), g_TxBuffer[msgCtr]) ); /* rem: testing if *g_RxBuffer == g_TxBuffer would be implementation-dependent */
    }

    /* clean-up */
    queue_delete(g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], &osErr);  /* nominal call */

}

/**
 * \brief Check behavior on overflow on one queue
 */
void test_queue_functional_testing_overflow_one_queue (void)
{
    uint32_t msgCtr ;
    OS_ERR_TYPE osErr ;

    /* initialize the messages to send */
    for ( msgCtr = 0; msgCtr <= (QUEUE_ELEMENT_POOL_SIZE); msgCtr ++ )
    {
        set_message(g_TxBuffer[msgCtr], _T("msg_"), msgCtr);
        g_RxBuffer[msgCtr] = NULL;
    }

    osErr = E_OS_ERR;
    g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK] = queue_create(QUEUE_ELEMENT_POOL_SIZE, &osErr);
    if ( E_OS_OK != osErr ) cu_print("Test error: failed to create FUNCTIONAL_TEST_QID_MAIN_TASK\n");

    /* overflow test: fill 1 queue */
    for (msgCtr = 0; msgCtr < QUEUE_ELEMENT_POOL_SIZE; msgCtr ++ )
    {
        osErr = E_OS_ERR_UNKNOWN;
        queue_send_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE) g_TxBuffer[msgCtr], &osErr);
        CU_ASSERT("Functional Test: queue_send_message nominal call, overflow test", osErr == E_OS_OK);
    }
    /* overflow test: add one too many */
    osErr = E_OS_ERR_UNKNOWN;
    queue_send_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE) g_TxBuffer[QUEUE_ELEMENT_POOL_SIZE], &osErr); /* queue should be full */
    CU_ASSERT("Functional Test: queue_send_message overflow", osErr == E_OS_ERR_OVERFLOW);

    /* try again, but do not provide a status variable */
    queue_send_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE) g_TxBuffer[QUEUE_ELEMENT_POOL_SIZE], NULL); /* queue should be full */
    CU_ASSERT("Functional Test: queue_send_message overflow - panic", did_panic() == true);
    clear_panic();

    /* overflow test: dequeue all */
    for (msgCtr = 0; msgCtr < QUEUE_ELEMENT_POOL_SIZE; msgCtr ++ )
    {
        osErr = E_OS_ERR_UNKNOWN;
        queue_get_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE*) &g_RxBuffer[msgCtr], OS_NO_WAIT, &osErr); /* nominal call */
        CU_ASSERT("Functional Test: queue_get_message nominal call, overflow test", osErr == E_OS_OK);
    }
    /* overflow test: read one more */
    osErr = E_OS_ERR_UNKNOWN;
    queue_get_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE*) &g_RxBuffer[NOMINAL_QUEUE_SIZE], OS_NO_WAIT, &osErr); /* nominal call */
    CU_ASSERT("Functional Test: queue_get_message empty queue", osErr == E_OS_ERR_EMPTY);
    /* try again, but do not provide a status variable */
    queue_get_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE*) &g_RxBuffer[NOMINAL_QUEUE_SIZE], OS_NO_WAIT, NULL); /* nominal call */
    CU_ASSERT("Functional Test: queue_send_message empty queue - panic", did_panic() == false);
    clear_panic();


    /* clean-up */
    queue_delete(g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], &osErr);  /* nominal call */

}


    /* ----------- Overflow of all queues */


/**
 * \brief Check behavior on overflow on all queues
 */
void test_queue_functional_testing_overflow_all_queues (void)
{
    uint32_t qidx, eidx;
    uint32_t remaining_element;
    OS_ERR_TYPE osErr;

    /* init message buffers */
    for (eidx = 0; eidx <= QUEUE_ELEMENT_POOL_SIZE; eidx ++ )
    {
        set_message(g_TxBuffer[eidx], _T("msg_"), eidx);
        g_RxBuffer[eidx] = NULL;
    }

    /* fill (LAST_QUEUE_INDEX - 1) queues with the average number of elements */
    for (qidx = 0; qidx < LAST_QUEUE_INDEX - 1 ; qidx ++ )
    {
        osErr = E_OS_ERR_UNKNOWN;
        g_Q[qidx] = queue_create(AVERAGE_ELEMENTS_IN_ONE_QUEUE, &osErr);
        CU_ASSERT("Overflow Test: nominal queue_create", osErr == E_OS_OK);

        for (eidx = 0; eidx < AVERAGE_ELEMENTS_IN_ONE_QUEUE; eidx ++ )
        {
            osErr = E_OS_ERR_UNKNOWN;
            queue_send_message (g_Q[qidx], (T_QUEUE_MESSAGE) g_TxBuffer[eidx], &osErr);
            CU_ASSERT("Overflow Test: queue_send_message nominal call, filling the queues", osErr == E_OS_OK);
        }
    }

    /* allocate the last queue with one too many elements --> should not return OK */
    osErr = E_OS_ERR_UNKNOWN;
    g_Q[LAST_QUEUE_INDEX] = queue_create((NB_ELEMENTS_OVERFLOW), &osErr);
    CU_ASSERT("Overflow Test: queue_create one too many elements", osErr != E_OS_OK);

    /* allocate the last queue with max number of elements */
    osErr = E_OS_ERR_UNKNOWN;
    g_Q[LAST_QUEUE_INDEX] = queue_create(( QUEUE_ELEMENT_POOL_SIZE ), &osErr);
    CU_ASSERT("Overflow Test: nominal queue_create all elements allocated", osErr == E_OS_OK);

    /* fill the last queue with max number of elements remaining */
    /* compute the remaining element:
     * remaining_element = number max of element allowed - number of element already inserted in each queue
     * number of element already inserted in each queue = number of element in each queue * number of queue already filled */
    remaining_element = QUEUE_ELEMENT_POOL_SIZE - (AVERAGE_ELEMENTS_IN_ONE_QUEUE * (LAST_QUEUE_INDEX - 1));
    for ( eidx = 0; eidx < remaining_element; eidx ++ )
    {
        osErr = E_OS_ERR_UNKNOWN;
        queue_send_message (g_Q[LAST_QUEUE_INDEX], (T_QUEUE_MESSAGE) g_TxBuffer[eidx], &osErr);
        CU_ASSERT("Overflow Test: queue_send_message nominal call, overflow test", osErr == E_OS_OK);
        if ( E_OS_OK != osErr)
            cu_print("Queue index = %d, message: %d / %d\n", LAST_QUEUE_INDEX, eidx+1, remaining_element);
    }

    /* send one more */
    osErr = E_OS_ERR_UNKNOWN;
    clear_panic();
    queue_send_message (g_Q[LAST_QUEUE_INDEX], (T_QUEUE_MESSAGE) g_TxBuffer[eidx], &osErr);

    CU_ASSERT("Overflow Test: queue_send_message, one too many message - panic", did_panic() == true);
    clear_panic();


    /* read and compare all messages */
    for ( eidx = 0; eidx < remaining_element; eidx ++ )
    {
        osErr = E_OS_ERR_UNKNOWN;
        queue_get_message (g_Q[LAST_QUEUE_INDEX], (T_QUEUE_MESSAGE*) &g_RxBuffer[eidx], OS_NO_WAIT, &osErr);
        CU_ASSERT("Overflow Test: queue_get_message nominal call, error code", osErr == E_OS_OK);
        if(osErr == E_OS_OK)
            CU_ASSERT("Overflow Test: queue_get_message nominal call, compare message", compare_messages( *(g_RxBuffer[eidx]), g_TxBuffer[eidx]) );
    }

    /* read one more */
    osErr = E_OS_ERR_UNKNOWN;
    queue_get_message (g_Q[LAST_QUEUE_INDEX], (T_QUEUE_MESSAGE*) &g_RxBuffer[0], OS_NO_WAIT, &osErr);
    CU_ASSERT("Overflow Test: queue_get_message on empty queue, error code", osErr == E_OS_ERR_EMPTY);

    /* cleanup */
    for (qidx = 0; qidx < LAST_QUEUE_INDEX; qidx ++ )
    {
        queue_delete(g_Q[qidx], &osErr);
    }

}


/**
 * \brief Check inter-task messaging
 * Actual test is performed in other tasks,
 * by functions fct_task1_test_queue and
 * fct_task2_test_queue
 */
void test_queue_functional_testing_different_tasks (void)
{
    uint32_t iteration;
    OS_ERR_TYPE osErr;

    avgElapsedTime = 0;
    minElapsedTime = 0xFFFFFFFF;
    maxElapsedTime = 0;
    minElapsedIdx = 0xFFFFFFFF;
    maxElapsedIdx = 0xFFFFFFFF;

    osErr = E_OS_ERR;
    g_Q[FUNCTIONAL_TEST_QID_TASK1_TX] = queue_create(NOMINAL_QUEUE_SIZE, &osErr);
    if ( E_OS_OK != osErr ) cu_print("Test error: failed to create FUNCTIONAL_TEST_QID_TASK1_TX\n");
    g_Q[FUNCTIONAL_TEST_QID_TASK2_TX] = queue_create(NOMINAL_QUEUE_SIZE, &osErr);
    if ( E_OS_OK != osErr ) cu_print("Test error: failed to create FUNCTIONAL_TEST_QID_TASK2_TX\n");

    /* start task2, task1 is started by task2 */
    task2_fct_id = TK2_TEST_QUEUES;

    /* wait for end of tests */
    do
    {
        local_task_sleep_ms(TEST_QUEUE_TASK_DELAY);

    } while ( (TK1_IDLE != task1_fct_id) ||
               (TK2_IDLE != task2_fct_id));


    for ( iteration = 0; iteration < TEST_QUEUE_MAX_ITERATIONS; iteration ++)
    {
        if ( elapsedTimes[iteration] >= maxElapsedTime )
        {
            maxElapsedTime = elapsedTimes[iteration];
            maxElapsedIdx = iteration;
        }

        if ( elapsedTimes[iteration] <= minElapsedTime )
        {
            minElapsedTime = elapsedTimes[iteration];
            minElapsedIdx = iteration;
        }

        avgElapsedTime += elapsedTimes[iteration];
    }
    avgElapsedTime = avgElapsedTime / TEST_QUEUE_MAX_ITERATIONS;

    cu_print("Elapsed time while waiting for message on %d iterations:\n",
        TEST_QUEUE_MAX_ITERATIONS);
    cu_print(" AVG = %d ms\n", avgElapsedTime);
    cu_print(" MIN = %d ms at iteration %d\n", minElapsedTime, minElapsedIdx);
    cu_print(" MAX = %d ms at iteration %d\n", maxElapsedTime, maxElapsedIdx);

    /* clean-up */
    queue_delete( g_Q[FUNCTIONAL_TEST_QID_TASK1_TX], &osErr);
    queue_delete( g_Q[FUNCTIONAL_TEST_QID_TASK2_TX], &osErr);

}





/**
 * \brief Task1 entry point
 */
void fct_task1_test_queue (void)
{
    T_TEST_MESSAGE   txBuffer;
    T_TEST_MESSAGE*  rxBuffer;
    T_TEST_MESSAGE   expectedRxBuffer;
    OS_ERR_TYPE osErr;
    volatile uint32_t startTime, endTime ;
    uint32_t iteration ;


    for (iteration = 0; iteration < TEST_QUEUE_MAX_ITERATIONS; iteration ++)
        elapsedTimes[iteration] = 0xFFFFFFFF ;

    set_message(txBuffer, TASK1_MSG_HDR, 0);
    set_message(expectedRxBuffer, TASK2_MSG_HDR, 0);

    for ( iteration = 0; iteration < TEST_QUEUE_MAX_ITERATIONS; iteration ++)
    {
        /* send */
        osErr = E_OS_ERR_UNKNOWN;
        queue_send_message (g_Q[FUNCTIONAL_TEST_QID_TASK1_TX], txBuffer, &osErr);
        CU_ASSERT("Functional Test: queue_send_message nominal call, task1", osErr == E_OS_OK);

        /* receive */
        osErr = E_OS_ERR_UNKNOWN;
        startTime = get_time_us();
        queue_get_message (g_Q[FUNCTIONAL_TEST_QID_TASK2_TX], (T_QUEUE_MESSAGE*)&rxBuffer, MSG_DEFAULT_TIMEOUT, &osErr);
        endTime = get_time_us();
#ifdef __TEST_VERBOSE_ON
        cu_print("\t\t Iteration %d   -  Start: %d   -  End %d -- buffer read: %s\n",iteration,startTime,endTime, *rxBuffer);
#endif
        elapsedTimes[iteration] = ((endTime - startTime) / 1000);
        CU_ASSERT("Functional Test: queue_get_message nominal call, task1, error code", osErr == E_OS_OK);
        CU_ASSERT("Functional Test: queue_get_message nominal call, task1, elapsed time min", elapsedTimes[iteration]  <= (MSG_TIMEOUT_ERROR + MSG_TIME_CONV_ERR) );
        if(rxBuffer != NULL)
            CU_ASSERT("Functional Test: queue_get_message nominal call, task1, compare messages", compare_messages( *rxBuffer, expectedRxBuffer) ); /* rem: testing if g_RxBuffer == g_TxBuffer would be implementation-dependent */

        increment_message (txBuffer);
        increment_message (expectedRxBuffer);
    }
}



/**
 * \brief Task2 entry point
 */
void fct_task2_test_queue (void)
{
    T_TEST_MESSAGE   txBuffer;
    T_TEST_MESSAGE*  rxBuffer;
    T_TEST_MESSAGE   expectedRxBuffer;
    OS_ERR_TYPE osErr;
    uint32_t iteration;

    set_message(txBuffer, TASK2_MSG_HDR, 9);
    set_message(expectedRxBuffer, TASK1_MSG_HDR, 0);

    /* start Task 1 */
    task1_fct_id = TK1_TEST_QUEUES;

    for ( iteration = 0; iteration < TEST_QUEUE_MAX_ITERATIONS; iteration ++)
    {
        /* receive */
        osErr = E_OS_ERR_UNKNOWN;
        queue_get_message (g_Q[FUNCTIONAL_TEST_QID_TASK1_TX],  (T_QUEUE_MESSAGE*)&rxBuffer, OS_WAIT_FOREVER, &osErr);
        CU_ASSERT("Functional Test: queue_get_message nominal call, task2, error code", osErr == E_OS_OK);
        CU_ASSERT("Functional Test: queue_get_message nominal call, task2, compare messages", compare_messages( *rxBuffer, expectedRxBuffer) ); /* rem: testing if *g_RxBuffer == g_TxBuffer would be implementation-dependent */
#ifdef __TEST_VERBOSE_ON
        cu_print("Task2 receive message: %s\n", *rxBuffer);
#endif

        increment_message (txBuffer);
        increment_message (expectedRxBuffer);

        /* send */
        osErr = E_OS_ERR_UNKNOWN;
        queue_send_message (g_Q[FUNCTIONAL_TEST_QID_TASK2_TX], txBuffer, &osErr);
        CU_ASSERT("Functional Test: queue_send_message nominal call, task1", osErr == E_OS_OK);
#ifdef __TEST_VERBOSE_ON
        cu_print("Task2 send message: %s\n", txBuffer);
#endif

    }

    /* Wait last queue reception in task 1 */
    local_task_sleep_ms(50);
}
/**
 * \brief ISR connected to UNIT_TESTS_IRQ_LINE
 *
 * \param address of a shared uint32_t
 */
void isr_clbk_test_queue (void* isrData)
{
    uint32_t* results = (uint32_t*) isrData;
    OS_ERR_TYPE osErr;
    T_QUEUE myQ;
    *results = 0;

    switch ( test_queue_test_type )
    {
    case E_UNIT_TESTS:
        osErr = E_OS_OK;
        myQ =  queue_create(NOMINAL_QUEUE_SIZE,  &osErr);
        if ( E_OS_ERR_NOT_ALLOWED != osErr )
        {
            *results |= 1;
        }

        osErr = E_OS_OK;
        queue_get_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE*) &g_RxBuffer, OS_NO_WAIT, &osErr);
        if ( E_OS_ERR_NOT_ALLOWED != osErr )
        {
            *results |= 1 << 1;
        }

        osErr = E_OS_OK;
        queue_send_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE) g_IsrMsgBuffer, &osErr);
        if ( E_OS_OK != osErr )
        {
            *results |= 1 << 2;
        }

        osErr = E_OS_OK;
        queue_delete (myQ, &osErr);
        if ( E_OS_ERR_NOT_ALLOWED != osErr )
        {
            *results |= 1 << 3;
        }
        break;

    case E_SEND_MESSAGE:
        g_IsrMsgBuffer[0] = 'F';
        g_IsrMsgBuffer[1] = 'r';
        g_IsrMsgBuffer[2] = 'o';
        g_IsrMsgBuffer[3] = 'm';
        g_IsrMsgBuffer[4] = 'I';
        g_IsrMsgBuffer[5] = 'S';
        g_IsrMsgBuffer[6] = 'R';
        g_IsrMsgBuffer[7] = 0;

        osErr = E_OS_OK;
        queue_send_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE) g_IsrMsgBuffer, &osErr);
        if ( E_OS_OK != osErr )
        {
            *results |= 1 << 2;
        }


        break;

    default:
        break;
    }

}


/**
 * \brief Check message queue API in interrupt context
 */
void test_queue_interrupt (void)
{
    uint32_t idx;
    OS_ERR_TYPE osErr;
    T_TEST_MESSAGE*  rxBuffer;
    interrupt_param_t it_parm = {E_CALLBACK_QUEUE, (void *)g_InterruptData, false} ;

    osErr = E_OS_ERR;
    g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK] = queue_create(NOMINAL_QUEUE_SIZE, &osErr);
    if ( E_OS_OK != osErr ) cu_print("Test error: failed to create FUNCTIONAL_TEST_QID_MAIN_TASK\n");

    /* program unit tests from ISR */
    g_InterruptData = 0 ;
    test_queue_test_type = E_UNIT_TESTS;

    /* trigger ISR */
    trigger_mbx_isr(&it_parm);

    /* check results */
    CU_ASSERT("Tests from interrupt: queue_create allowed in ISR context",  ( g_InterruptData & 1 ) == 0);
    CU_ASSERT("Tests from interrupt: queue_get_message allowed in ISR context",  ( g_InterruptData & (1<<1) ) == 0);
    CU_ASSERT("Tests from interrupt: queue_send_message failed in ISR context",  ( g_InterruptData & (1<<2) ) == 0);
    CU_ASSERT("Tests from interrupt: queue_delete allowed in ISR context",  ( g_InterruptData & (1<<3) ) == 0);

    /* prepare next test: clear the data buffer */
    for (idx = 0; idx < TEST_MSG_MAX_SIZE; idx ++)
        g_IsrMsgBuffer[idx] = 0;

    /* prepare next test: configure ISR to send a message */
    test_queue_test_type = E_SEND_MESSAGE;

    /* trigger ISR */
    trigger_mbx_isr(&it_parm);

    /* read the message */
    osErr = E_OS_OK;
    queue_get_message (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], (T_QUEUE_MESSAGE*) &rxBuffer, OS_NO_WAIT, &osErr);

    /*check results */
    CU_ASSERT("Tests from interrupt: nominal queue_get_message from ISR context, reception",  osErr == E_OS_OK );
    CU_ASSERT("Tests from interrupt: nominal queue_get_message from ISR context, error status",  g_InterruptData == 0);
    if(osErr == E_OS_OK)
        CU_ASSERT("Tests from interrupt: nominal queue_get_message from ISR context, compare messages", compare_messages( *rxBuffer, g_IsrMsgBuffer) );

    /* clean-up */
    queue_delete (g_Q[FUNCTIONAL_TEST_QID_MAIN_TASK], &osErr);
}










