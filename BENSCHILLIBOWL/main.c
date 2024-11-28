#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "BENSCHILLIBOWL.h"

// Feel free to play with these numbers! This is a great way to
// test your implementation.
#define BENSCHILLIBOWL_SIZE 100
#define NUM_CUSTOMERS 90
#define NUM_COOKS 10
#define ORDERS_PER_CUSTOMER 3
#define EXPECTED_NUM_ORDERS NUM_CUSTOMERS * ORDERS_PER_CUSTOMER

// Global variable for the restaurant.
BENSCHILLIBOWL *bcb;

/**
 * Thread function that represents a customer.
 */
void* BENSCHILLIBOWLCustomer(void* tid) {
    int customer_id = (int)(long)tid;

    for (int i = 0; i < ORDERS_PER_CUSTOMER; i++) {
        // Allocate space for a new order
        Order *order = malloc(sizeof(Order));
        if (!order) {
            perror("Failed to allocate memory for order");
            continue;
        }

        // Populate the order with menu item and customer ID
        order->menu_item = PickRandomMenuItem();
        order->customer_id = customer_id;

        // Add the order to the restaurant
        int order_number = AddOrder(bcb, order);
        printf("Customer #%d placed Order #%d: %s\n", customer_id, order_number, order->menu_item);
    }

    return NULL;
}

/**
 * Thread function that represents a cook.
 */
void* BENSCHILLIBOWLCook(void* tid) {
    int cook_id = (int)(long)tid;
    int orders_fulfilled = 0;

    while (1) {
        // Get an order from the restaurant
        Order *order = GetOrder(bcb);
        if (!order) break;  // No more orders to fulfill

        // Fulfill the order and free memory
        printf("Cook #%d fulfilled Order #%d: %s for Customer #%d\n",
               cook_id, order->order_number, order->menu_item, order->customer_id);
        free(order);
        orders_fulfilled++;
    }

    printf("Cook #%d fulfilled %d orders\n", cook_id, orders_fulfilled);
    return NULL;
}

/**
 * Main function:
 *  - Open the restaurant.
 *  - Create customer and cook threads.
 *  - Wait for all threads to finish.
 *  - Close the restaurant.
 */
int main() {
    pthread_t customers[NUM_CUSTOMERS];
    pthread_t cooks[NUM_COOKS];

    // Open the restaurant
    bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE, EXPECTED_NUM_ORDERS);

    // Create cook threads
    for (int i = 0; i < NUM_COOKS; i++) {
        pthread_create(&cooks[i], NULL, BENSCHILLIBOWLCook, (void*)(long)(i + 1));
    }

    // Create customer threads
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_create(&customers[i], NULL, BENSCHILLIBOWLCustomer, (void*)(long)(i + 1));
    }

    // Wait for all customers to finish
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_join(customers[i], NULL);
    }

    // Wait for all cooks to finish
    for (int i = 0; i < NUM_COOKS; i++) {
        pthread_join(cooks[i], NULL);
    }

    // Close the restaurant
    CloseRestaurant(bcb);

    return 0;
}
