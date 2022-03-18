#include "p3160241-p3190122-p3190081-pizza.h"

void *order(void *order_id){
    int *oid;
    oid = (int *)order_id;
    unsigned int seedp = seed+(*oid); // initialize a seed unique for each thread
    int rc;
	struct timespec customer_appeared; //when the customer appeared
    struct timespec order_start; // when the customer talks to the telephone operator
    struct timespec order_prep; // when the order starts to be prepared
    struct timespec order_bake; // when the order is inserted in the oven
    struct timespec order_stop; // when the order is out of the oven 
    struct timespec order_packed; // when the order is packed
    struct timespec order_delivered; // when the order is delivered to the customer

    clock_gettime(CLOCK_REALTIME, &customer_appeared); // get the time the customer appeared
	 
	 // try to find an available telephone operator
	rc = pthread_mutex_lock(&mutex_available_telephone_operators);
    if(rc != 0){
        printf("Mutex: Error in locking with code %d\n", rc);
        pthread_exit(&rc);
    }
    while(available_telephone_operators<=0){
        rc = pthread_cond_wait(&cond_available_telephone_operators, &mutex_available_telephone_operators);
        if(rc != 0){
            printf("Mutex: Error in condition wait with code %d\n", rc);
            pthread_exit(&rc);
        }
    }
	
	// available telephone operator was found
    printf("Phone Operator was found for order %d.\n", *oid);
    --available_telephone_operators; 
    rc = pthread_mutex_unlock(&mutex_available_telephone_operators);
    if(rc != 0){
        printf("Mutex: Error in unlocking with code %d\n", rc);
        pthread_exit(&rc);
    }
	
	
	clock_gettime(CLOCK_REALTIME, &order_start); // get the time the operator picked up the phone
	 printf("Taking order %d.\n", *oid); 
	 
	 // update the total wait time
	rc = pthread_mutex_lock(&mutex_total_wait_time);
	if(rc != 0)
	{
		printf("Mutex: Error in locking with code %d\n", rc);
        pthread_exit(&rc);
	}
		
	total_wait_time = total_wait_time + (order_start.tv_sec - customer_appeared.tv_sec); 
		
	rc = pthread_mutex_unlock(&mutex_total_wait_time);
	if(rc != 0)
	{
        printf("Mutex: Error in unlocking with code %d\n", rc);
        pthread_exit(&rc);
	}
		
	// update the maximum wait time if needed
	rc = pthread_mutex_lock(&mutex_max_wait_time); 
	if(rc != 0)
	{
        printf("Mutex: Error in locking with code %d\n", rc);
        pthread_exit(&rc);
	}
		
	if((order_start.tv_sec - customer_appeared.tv_sec) > max_wait_time)
	{
		max_wait_time = (order_start.tv_sec - customer_appeared.tv_sec); 
	}
		
	rc = pthread_mutex_unlock(&mutex_max_wait_time);
	if(rc != 0)
	{
       printf("Mutex: Error in unlocking with code %d\n", rc);
       pthread_exit(&rc);
	}
	 
	
	int no_pizzas = rand_r(&seedp) % N_order_high + N_order_low; // get the number of pizzas in the order through randomization
    
    	int wait = rand_r(&seedp)%T_payment_high+T_payment_low;
    	sleep(wait); // the operator is trying to charge the customer
	
	if((rand_r(&seedp) % 100) < P_fail) // if the payment fails
	{
		rc = pthread_mutex_lock(&mutex_failed_orders);
		if(rc != 0)
		{
        printf("Mutex: Error in locking with code %d\n", rc);
        pthread_exit(&rc);
		}
		
		++failed_orders; // update failed_orders
		
		rc = pthread_mutex_unlock(&mutex_failed_orders);
		if(rc != 0){
        printf("Mutex: Error in unlocking with code %d\n", rc);
        pthread_exit(&rc);
		}
		
		rc = pthread_mutex_lock(&mutex_lock_screen);
		if(rc != 0)
		{
        printf("Mutex: Error in locking with code %d\n", rc);
        pthread_exit(&rc);
		}
		
		fail_message(*oid); // print fail message
		
		rc = pthread_mutex_unlock(&mutex_lock_screen);
		if(rc != 0){
        printf("Mutex: Error in unlocking with code %d\n", rc);
        pthread_exit(&rc);
		}
		
		// release the telephone operator
		rc = pthread_mutex_lock(&mutex_available_telephone_operators);
		if(rc != 0)
		{
			printf("Mutex: Error in locking with code %d\n", rc);
			pthread_exit(&rc);
		}
		
		++available_telephone_operators;
		
		pthread_cond_signal(&cond_available_telephone_operators);
		rc = pthread_mutex_unlock(&mutex_available_telephone_operators);
		if(rc != 0)
		{
			printf("Mutex: Error in unlocking with code %d\n", rc);
			pthread_exit(&rc);
		}
		
		
		pthread_exit(oid); // exit the thread
	}
	
	// the payment was successful
	rc = pthread_mutex_lock(&mutex_successful_orders);
	if(rc != 0)
	{
       printf("Mutex: Error in locking with code %d\n", rc);
       pthread_exit(&rc);
	}
		
	++successful_orders; // update successful_orders
		
	rc = pthread_mutex_unlock(&mutex_successful_orders);
	if(rc != 0)
	{
       printf("Mutex: Error in unlocking with code %d\n", rc);
		pthread_exit(&rc);
	}
		
	rc = pthread_mutex_lock(&mutex_lock_screen);
	if(rc != 0)
	{
		printf("Mutex: Error in locking with code %d\n", rc);
        pthread_exit(&rc);
	}
		
	success_message(*oid); // print success message
		
	rc = pthread_mutex_unlock(&mutex_lock_screen);
	if(rc != 0)
	{
		printf("Mutex: Error in unlocking with code %d\n", rc);
        pthread_exit(&rc);
	}
	
	rc = pthread_mutex_lock(&mutex_total_income);
	if(rc != 0)
	{
		printf("Mutex: Error in locking with code %d\n", rc);
        pthread_exit(&rc);
	}
		
	total_income = total_income + C_pizza*no_pizzas; // update total income
		
	rc = pthread_mutex_unlock(&mutex_total_income);
	if(rc != 0)
	{
		printf("Mutex: Error in unlocking with code %d\n", rc);
        pthread_exit(&rc);
	}
	
	// release the telephone operator
	rc = pthread_mutex_lock(&mutex_available_telephone_operators);
    if(rc != 0)
	{
        printf("Mutex: Error in locking with code %d\n", rc);
        pthread_exit(&rc);
    }
	
    ++available_telephone_operators;
	
    pthread_cond_signal(&cond_available_telephone_operators);
    rc = pthread_mutex_unlock(&mutex_available_telephone_operators);
    if(rc != 0)
	{
        printf("Mutex: Error in unlocking with code %d\n", rc);
        pthread_exit(&rc);
    }
	
	// try to find an availabe cook
	rc = pthread_mutex_lock(&mutex_available_cooks);
    if(rc != 0)
	{
        printf("Mutex: Error in locking with code %d\n", rc);
        pthread_exit(&rc);
    }
    while(available_cooks<=0)
	{
        rc = pthread_cond_wait(&cond_available_cooks, &mutex_available_cooks);
        if(rc != 0){
            printf("Mutex: Error in condition wait with code %d\n", rc);
            pthread_exit(&rc);
        }
    }
	
	// available cook was found 
    printf("Cook was found for order %d.\n", *oid);
    --available_cooks; 
    rc = pthread_mutex_unlock(&mutex_available_cooks);
    if(rc != 0)
	{
        printf("Mutex: Error in unlocking with code %d\n", rc);
        pthread_exit(&rc);
    }
	
	clock_gettime(CLOCK_REALTIME, &order_prep); // get the time the pizzas started to be prepared
	
    sleep(no_pizzas*T_prep); // pizzas are being prepaered
	
    printf("%d pizzas prepared for order %d.\n", no_pizzas, *oid);
	
	// try to find available ovens
    rc = pthread_mutex_lock(&mutex_available_ovens);
    if(rc != 0){
        printf("Mutex: Error in locking with code %d\n", rc);
        pthread_exit(&rc);
    }
    while(available_ovens<no_pizzas)
	{
        rc = pthread_cond_wait(&cond_available_ovens, &mutex_available_ovens);
        if(rc != 0)
		{
            printf("Mutex: Error in condition wait with code %d\n", rc);
            pthread_exit(&rc);
        }
    }
	
	// available ovens were found and pizzas were inserted for the baking
    printf("Oven was found for order %d, starting to bake...\n", *oid);
    available_ovens = available_ovens - no_pizzas; 
    rc = pthread_mutex_unlock(&mutex_available_ovens);
    if(rc != 0)
	{
        printf("Mutex: Error in unlocking with code %d\n", rc);
        pthread_exit(&rc);
    }
	
    // release the cook because the pizzas were inserted in the ovens for baking
    rc = pthread_mutex_lock(&mutex_available_cooks);
    if(rc != 0)
	{
        printf("Mutex: Error in locking with code %d\n", rc);
        pthread_exit(&rc);
    }
	
    ++available_cooks;
	
    pthread_cond_signal(&cond_available_cooks);
    rc = pthread_mutex_unlock(&mutex_available_cooks);
    if(rc != 0){
        printf("Mutex: Error in unlocking with code %d\n", rc);
        pthread_exit(&rc);
    }
    
    clock_gettime(CLOCK_REALTIME, &order_bake); // get the time the pizzas were inserted into the ovens
    
    sleep(T_bake); // pizzas are being baked
    printf("Pizzas for order %d are done, waiting for packer!\n", *oid);
	
	// try to find an available packer
	rc = pthread_mutex_lock(&mutex_available_packer);
    if(rc != 0){
        printf("Mutex: Error in locking with code %d\n", rc);
        pthread_exit(&rc);
    }
    while(available_packer<=0){
        rc = pthread_cond_wait(&cond_available_packer, &mutex_available_packer);
        if(rc != 0){
            printf("Mutex: Error in condition wait with code %d\n", rc);
            pthread_exit(&rc);
        }
    }
	
	// available packer was found 
    printf("Packer was found for order %d.\n", *oid);
    --available_packer; 
    rc = pthread_mutex_unlock(&mutex_available_packer);
    if(rc != 0){
        printf("Mutex: Error in unlocking with code %d\n", rc);
        pthread_exit(&rc);
    }
	
	clock_gettime(CLOCK_REALTIME, &order_stop); // get the time the packer got the pizzas out of the oven and started to pack them
    
	// release the ovens
    rc = pthread_mutex_lock(&mutex_available_ovens);
    if(rc != 0){
        printf("Mutex: Error in locking with code %d\n", rc);
        pthread_exit(&rc);
    }
	
    available_ovens = available_ovens + no_pizzas;
	
    pthread_cond_signal(&cond_available_ovens);
    rc = pthread_mutex_unlock(&mutex_available_ovens);
    if(rc != 0){
        printf("Mutex: Error in unlocking with code %d\n", rc);
        pthread_exit(&rc);
    }
	
	sleep(T_pack); // pizzas are being packed
	printf("Packing of pizzas for order %d is done, waiting for delliverer!\n", *oid);
	
	clock_gettime(CLOCK_REALTIME, &order_packed); // get the time the pizzas got packed
	
	// calculate the preparation time
	preparation_time = ((order_packed.tv_sec - order_stop.tv_sec) + (order_stop.tv_sec - order_bake.tv_sec) + 
	(order_bake.tv_sec - order_prep.tv_sec) + (order_prep.tv_sec - order_start.tv_sec) + (order_start.tv_sec - customer_appeared.tv_sec));
	
	rc = pthread_mutex_lock(&mutex_lock_screen);
	if(rc != 0)
	{
		printf("Mutex: Error in locking with code %d\n", rc);
		pthread_exit(&rc);
	}
		
	prepared_message(*oid, preparation_time); // print prepared message
		
	rc = pthread_mutex_unlock(&mutex_lock_screen);
	if(rc != 0){
		printf("Mutex: Error in unlocking with code %d\n", rc);
        pthread_exit(&rc);
	}
	
	// release the packer
	rc = pthread_mutex_lock(&mutex_available_packer);
    if(rc != 0){
        printf("Mutex: Error in locking with code %d\n", rc);
        pthread_exit(&rc);
    }
    ++available_packer;
    pthread_cond_signal(&cond_available_packer);
    rc = pthread_mutex_unlock(&mutex_available_packer);
    if(rc != 0){
        printf("Mutex: Error in unlocking with code %d\n", rc);
        pthread_exit(&rc);
    }
	
	// try to find an availabe deliverer
	rc = pthread_mutex_lock(&mutex_available_deliverers);
    if(rc != 0){
        printf("Mutex: Error in locking with code %d\n", rc);
        pthread_exit(&rc);
    }
    while(available_deliverers<=0){
        rc = pthread_cond_wait(&cond_available_deliverers, &mutex_available_deliverers);
        if(rc != 0){
            printf("Mutex: Error in condition wait with code %d\n", rc);
            pthread_exit(&rc);
        }
    }
	
	// available deliverer was found 
    printf("Deliverer was found for order %d.\n", *oid);
    --available_deliverers; 
    
    rc = pthread_mutex_unlock(&mutex_available_deliverers);
    if(rc != 0){
        printf("Mutex: Error in unlocking with code %d\n", rc);
        pthread_exit(&rc);
    }
	
	int T_delivery = rand_r(&seedp)%T_del_high + T_del_low; // get the time the deliverer needs to deliver the pizza through+++++++++ randomization
	
    sleep(T_delivery); // pizzas are being delivered
    printf("Order for %d finally delivered!\n", *oid);
    
    clock_gettime(CLOCK_REALTIME, &order_delivered); // get the time the pizzas got delivered
	
	delivery_time = ((order_delivered.tv_sec - order_packed.tv_sec) + (order_packed.tv_sec - order_stop.tv_sec) + (order_stop.tv_sec - order_bake.tv_sec) + 
	(order_bake.tv_sec - order_prep.tv_sec) + (order_prep.tv_sec - order_start.tv_sec) + (order_start.tv_sec - customer_appeared.tv_sec)); // calculate the delivery time
	
	rc = pthread_mutex_lock(&mutex_lock_screen);
	if(rc != 0)
	{
		printf("Mutex: Error in locking with code %d\n", rc);
		pthread_exit(&rc);
	}
		
	delivered_message(*oid, delivery_time); // print delivered message
		
	rc = pthread_mutex_unlock(&mutex_lock_screen);
	if(rc != 0){
		printf("Mutex: Error in unlocking with code %d\n", rc);
        pthread_exit(&rc);
	}
    
    sleep(T_delivery); // deliverer returns to the pizzeria
	
    //release the deliverer because he is now pack at the pizzeria
    rc = pthread_mutex_lock(&mutex_available_deliverers);
    if(rc != 0){
        printf("Mutex: Error in locking with code %d\n", rc);
        pthread_exit(&rc);
    }
    ++available_deliverers;
    pthread_cond_signal(&cond_available_deliverers);
    rc = pthread_mutex_unlock(&mutex_available_deliverers);
    if(rc != 0){
        printf("Mutex: Error in unlocking with code %d\n", rc);
        pthread_exit(&rc);
    }
	
	// update total service time
	rc = pthread_mutex_lock(&mutex_total_service_time);
	if(rc != 0)
	{
		printf("Mutex: Error in locking with code %d\n", rc);
        pthread_exit(&rc);
	}
		
	total_service_time = total_service_time + delivery_time; 
		
	rc = pthread_mutex_unlock(&mutex_total_service_time);
	if(rc != 0)
	{
        printf("Mutex: Error in unlocking with code %d\n", rc);
        pthread_exit(&rc);
	}
	
	// update max service time if needed	
	rc = pthread_mutex_lock(&mutex_max_service_time); 
	if(rc != 0)
	{
        printf("Mutex: Error in locking with code %d\n", rc);
        pthread_exit(&rc);
	}
		
	if((delivery_time) > max_service_time)
	{
		max_service_time = delivery_time; 
	}
		
	rc = pthread_mutex_unlock(&mutex_max_service_time);
	if(rc != 0)
	{
       printf("Mutex: Error in unlocking with code %d\n", rc);
       pthread_exit(&rc);
	}
	
	// update total cold time
	rc = pthread_mutex_lock(&mutex_total_cold_time);
	if(rc != 0)
	{
		printf("Mutex: Error in locking with code %d\n", rc);
        pthread_exit(&rc);
	}
		
	total_cold_time = total_cold_time + (order_delivered.tv_sec - order_packed.tv_sec) + (order_packed.tv_sec - order_stop.tv_sec); 
		
	rc = pthread_mutex_unlock(&mutex_total_cold_time);
	if(rc != 0)
	{
        printf("Mutex: Error in unlocking with code %d\n", rc);
        pthread_exit(&rc);
	}
		
	// update max cold time if needed
	rc = pthread_mutex_lock(&mutex_max_cold_time); 
	if(rc != 0)
	{
        printf("Mutex: Error in locking with code %d\n", rc);
        pthread_exit(&rc);
	}
		
	if(((order_delivered.tv_sec - order_packed.tv_sec) + (order_packed.tv_sec - order_stop.tv_sec)) > max_cold_time)
	{
		max_cold_time = (order_delivered.tv_sec - order_packed.tv_sec) + (order_packed.tv_sec - order_stop.tv_sec); 
	}
		
	rc = pthread_mutex_unlock(&mutex_max_cold_time);
	if(rc != 0)
	{
       printf("Mutex: Error in unlocking with code %d\n", rc);
       pthread_exit(&rc);
	}
	
	pthread_exit(oid); // exit the thread
}

int main(int argc, char *argv[]) {

	// check if the user entered the right amount of parameters
    if(argc != 3) {
        printf("You have entered a wrong number of parameters.\n\n");
        exit(-1); // if not, exit 
    }

    int N_cust = atoi(argv[1]);
	// check if the user entered a valid number of customers
    if(N_cust <0) {
        printf("The pizzeria can't have a negative number of customers.\n\n");
        exit(-1); // if not, exit
    }

    seed = atoi(argv[2]); // get the seed
	
	printf("\nWelocme to our pizzeria.\nCustomers: %d.\nSeed: %d.\n\n", N_cust, seed);

    // initialize variables to be used
	available_telephone_operators = N_tel;
    available_cooks = N_cook;
    available_ovens = N_oven;
    available_deliverers = N_deliverer;
    available_packer = N_packer;
    preparation_time = 0;
    delivery_time = 0;
    total_income = 0;
    successful_orders = 0;
    failed_orders = 0;
    total_wait_time = 0;
    max_wait_time = 0;
    total_service_time = 0;
    max_service_time = 0;
    total_cold_time = 0;
     max_cold_time = 0;

    // initialize order id's
	
    int order_id[N_cust];
    for(int i=0;i<N_cust;++i)
    {
        order_id[i] = i+1;
    }

    //initialize order threads
	
    pthread_t *threads;
    threads = malloc(N_cust*sizeof(pthread_t));
    if(threads==NULL){
        printf("Error allocating memory.\n");
        exit(-1);
    }

    int rc;
    for(int i=0; i<N_cust; ++i)
    {
        rc = pthread_create(&threads[i], NULL, &order, &order_id[i]);
        
        int wait = rand_r(&seed)%T_order_high+T_order_low;
        sleep(wait); //simulate the waiting time for the next order to come
		
        if (rc != 0) {
            printf("Error: pthread_create() returned %d\n", rc);
            exit(-1);
        }
    }

    //join threads
    void *status;
    for(int i=0; i<N_cust; i++)
    {
        rc = pthread_join(threads[i], &status);
        if (rc != 0) {
            printf("Error: pthread_join() returned %d on thread %d\n", rc, order_id[i]);
            exit(-1);
        }
    }

	// print the final message
    final_message(N_cust, total_income, successful_orders, failed_orders, total_wait_time
	, max_wait_time, total_service_time, max_service_time, total_cold_time, max_cold_time);
	
    free(threads); // free the threads
	
    return 1; // exit
}
