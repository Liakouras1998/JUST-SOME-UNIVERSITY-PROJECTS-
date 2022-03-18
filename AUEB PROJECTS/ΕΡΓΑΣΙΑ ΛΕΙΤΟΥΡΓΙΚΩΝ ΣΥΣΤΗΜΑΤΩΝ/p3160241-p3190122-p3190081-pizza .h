#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

const int N_tel = 3; // maximum telephone operators
const int N_cook = 2; // maximum cooks
const int N_oven = 10; // maximum ovens
const int N_deliverer = 7; // maximum deliverers
const int N_packer = 1; // maximum packers

const int T_order_low = 1; // minimum time for another customer to call
const int T_order_high = 5; // maximum time for another customer to call

const int N_order_low = 1; // minimum pizzas in each order
const int N_order_high = 5; // maximum pizzas in each order 

const int T_payment_low = 1; // minimum time for the payment
const int T_payment_high = 2; // maximum time for the payment

const int C_pizza = 10; // cost of a pizza

const float P_fail = 5; // payment failure probability

const int T_prep = 1; // preparation time for each pizza

const int T_bake = 10; // baking time for each pizza

const int T_pack = 2; // packing time for each pizza

const int T_del_low = 5; // minimum time for an order to be delivered
const int T_del_high = 15; // maximum time for an order to be delivered

// Declaration of variables
int available_telephone_operators;
int available_cooks; 
int available_ovens; 
int available_deliverers; 
int available_packer;
double preparation_time;
double delivery_time; 
double total_income;
double successful_orders;
double failed_orders;
double total_wait_time;
double max_wait_time;
double total_service_time; 
double max_service_time; 
double total_cold_time; 
double max_cold_time; 
unsigned int seed;

// Declaration of mutexes
pthread_mutex_t mutex_available_telephone_operators = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_available_cooks = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_available_ovens = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_available_deliverers = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_available_packer = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_total_income = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_successful_orders = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_failed_orders = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_total_wait_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_max_wait_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_total_service_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_max_service_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_total_cold_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_max_cold_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lock_screen = PTHREAD_MUTEX_INITIALIZER;

// Declaration of conditions
pthread_cond_t cond_available_telephone_operators = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_available_cooks = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_available_ovens = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_available_deliverers = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_available_packer = PTHREAD_COND_INITIALIZER;

// Messages

void fail_message(int oid) // message when an order gets canceled
{
	printf("\nOrder with id %d failed.\n\n", oid);
}

void success_message(int oid) // message when an order gets registered
{
	printf("\nOrder with id %d registered.\n\n", oid);
}

void prepared_message(int oid, double preparation_time) // message when the pizza is prepared.Shows the preparation time
{
	printf("\nOrder with id %d was prepared in %f minutes.\n\n", oid, preparation_time);
}

void delivered_message(int oid, double delivery_time) // message when the pizza is delivered.Shows the delivery time
{
	printf("\nOrder with id %d was delivered in %f minutes.\n\n", oid, delivery_time);
}

// final message.Shows information about the orders
void final_message(int N_cust, double total_income, double successful_orders, double failed_orders, double total_wait_time
, double max_wait_time, double total_service_time, double max_service_time, double total_cold_time, double max_cold_time) 
{
	printf("\nTotal Income: %f.\nSuccessful Orders: %f.\nFailed Orders: %f.\nAverage Wait Time: %f.\nMaximum Wait Time: %f. \
\nAverage Service Time: %f. \nMaximum Service Time: %f.\nAverage Cold Time: %f.\nMaximum Cold Time: %f.\n\n"
	, total_income, successful_orders, failed_orders, total_wait_time/N_cust, max_wait_time
	, total_service_time/N_cust, max_service_time, total_cold_time/N_cust, max_cold_time);
}
