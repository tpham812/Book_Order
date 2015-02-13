#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "DataStructures.c"
#include "utility.c"
#include "BookOrder.h"

pthread_mutex_t lock, plock;
pthread_cond_t notfull, notempty;
pthread_t producer;
pthread_t *consumer;
int endOfFile = 0;
int consumerThreads = 0;

/** Producer that process book orders and append it to the queue */
void *readFromOrderFile(void *fileName) {

	FILE *file;
	BookOrder *bookOrder;
	char *order, *token;
	char *orderInfo[4];
	int i;

	file = fopen((char*)fileName, "r");
	if(file == NULL) {printf("Failed to open up \"%s\". Program will exit.\n", (char*)fileName); exit(0);}
	printf("Producer starting up.\n");
	while(!feof(file) && error == 0) {
		pthread_mutex_lock(&plock);
		/* Wait for consumer threads if queue is full */
		while(QueueSize == 10) {
			if(error > 0) {
				fclose(file);
				pthread_mutex_unlock(&plock);
				/* Continue to signal to consumer threads notempty if they are still active */
				while(consumerThreads > 0){
					pthread_cond_signal(&notempty);
				}
				printf("Producer thread finished.\n");
				return;
			}
			printf("\nProducer waits because queues or buffers are full.\n");
			pthread_cond_signal(&notempty);	/* Signal to consumer threads that queue is not empty */		
			pthread_cond_wait(&notfull, &plock);
			printf("\nProducer resumes. Buffer space is available.\n");
		}
		order = getStringFromFile(file);
		if(order[0] == '\0') continue; 
		token = strtok(order, "\"|");
		i = 0;
		/* Tokenize the book order */
		while(token != NULL) {
			orderInfo[i] = token;
			token = strtok(NULL, "\"|");
			i++;
		}
		/* Create book order */
		createBookOrder(&bookOrder, orderInfo);
		/* Add book order to queue */
		addToBookQueueList(&bookOrder);
		pthread_cond_signal(&notempty);
		free(order);
		pthread_mutex_unlock(&plock);	
	}
	endOfFile = 1;
	printf("All book orders have been read in.\n");		
	fclose(file);
	/* Continue to signal to consumer threads notempty if they are still active */
	while(consumerThreads > 0){
		pthread_cond_signal(&notempty);
	}
	printf("Producer thread finished.\n");
}

/** Consumer that takes book order from the queue and process the order to the customer */ 
void *processOrder(void *category) {
	
	printf("Consumer thread for: %s is starting up.\n", (char *)category); 
	while(1 && error == 0) {
		pthread_mutex_lock(&lock);
		/** Wait for producer is queue is empty */
		while(QueueSize == 0) {
			if(endOfFile == 1 || error > 0) {
				pthread_mutex_unlock(&lock); 
				printf("Consumer thread of category: %s finished.\n", (char*)category);
				consumerThreads--;
				return;
			}
			printf("\nConsumer of category: %s waits because queues or buffers are empty.\n", (char*)category);
			pthread_cond_signal(&notfull); /* Signal to produce that queue is not full */
			pthread_cond_wait(&notempty, &lock);
			printf("\nConsumer of category: %s resumes. Ready for processing.\n", (char*)category);
		}		
		BookOrder *bookOrders = bookQueue -> bookOrders;
		BookOrder *temp = NULL;
		/* Process the book order from the queue */
		if(bookOrders != NULL) {
			Book *book;
			temp = bookOrders -> nextBookOrder;
			if(strcmp(bookOrders -> category, (char*)category) == 0) {
				if(bookOrders == bookQueue -> tail) bookQueue -> tail = NULL;
				bookQueue -> bookOrders = temp;
				createBook(&book, &bookOrders); /** Create book */
				addBookToCustomer(&book, book -> ID % SIZE); /** Add book to customer */
				destroyBookOrder(&bookOrders);
				QueueSize--;
		   		pthread_cond_signal(&notfull);	/** Signal that queue is not full */
		 	}
		}
		pthread_mutex_unlock(&lock);
	}
	pthread_mutex_lock(&lock);
	consumerThreads--;
	pthread_mutex_unlock(&lock);
	printf("Consumer thread of category: %s finished.\n", (char*)category);
}

/** Reading in category file and counting the number of categories */
int readFromCategoryFile(char *fileName) {

	FILE *file;
	Category *cat;
	char *category;
	file = fopen(fileName, "r");
	if(file == NULL) return 0;
	createQueue();
	while(!feof(file)) {
		category = getStringFromFile(file);
		if(category[0] == '\0') continue;
		createCategoryNode(&cat, category); 
		addToCategoriesList(&cat); /** Create a category list */
		numOfCategory++;	/** Increment number of category */
		free(category);
	}
	fclose(file);
	return 1;
}

/** Reading in customer file */
int readFromCustomerFile(char *fileName) {

	FILE *file;
	Customer *customer;
	int ch;
	char *line, *token;
	char *info[6];
	file = fopen(fileName, "r");
	int i;
	if(file == NULL) return 0;
	while(!feof(file)) {
		line = getStringFromFile(file);
		if(line[0] == '\0') continue; 
		token = strtok(line, "\"|");
		i = 0;
		/** Tokenize customer info */
		while(token != NULL) {
			info[i] = token;
			token = strtok(NULL, "\"|");
			i++;
		}
		createCustomer(&customer, info); /** Create customer node */
		addCustomer(&customer, customer -> ID % SIZE); /** Add customer node to hashmap and list */
		free(line);
	}
	fclose(file);
	return 1;
}

/** Prints the final report of the orders */
int printReport() {

	FILE *file;
	file = fopen("finalreport.txt", "w");
	if(file == NULL) {printf("Failed to open up \"finalreport.txt\". Program will now exit.\n"); exit(0);}
	Customer *curr = C_List.head;
	while(curr != NULL) {
		fprintf(file, "=== BEGIN CUSTOMER INFO ===\n### BALANCE ###\n");
		fprintf(file, "Customer name: %s\nCustomer ID number: %d\n", curr -> name, curr -> ID);
		fprintf(file, "Remaining credit balance after all purchases (a dollar amount): %.2f\n", curr -> balance);
		fprintf(file, "### SUCCESSFUL ORDERS ###\n");
		Book *s_book = curr -> successfulOrder;
		while(s_book != NULL) {
			fprintf(file, "\"%s\"|%.2f|%.2f\n", s_book -> title, s_book -> price, s_book -> balance);
			s_book = s_book -> nextBook;
		}
		fprintf(file, "### REJECTED ORDERS ###\n");
		Book *u_book = curr -> unsuccessfulOrder;
		while(u_book != NULL) {
			fprintf(file, "\"%s\"|%.2f\n", u_book -> title, u_book -> price);
			u_book = u_book -> nextBook;
		}
		fprintf(file, "=== END CUSTOMER INFO ===\n\n");
		curr = curr -> nextCustomerL;
	}
	fclose(file);
	return 1;
}

int main(int argc, char **argv) {

	int check;
	initializeCustomerList();
	initializeCustomerHashMap();
	/** Error handling */
	if(argc < 4) {printf("Did not enter in enough files for Book Order processing. Program will exit.\n"); return 0;}
	else if(argc > 4) {printf("Entered in an extra argument for Book Order processing. Program will exit.\n"); return 0;}
	if(readFromCustomerFile(argv[1]) == 0){printf("Failed to open up \"%s\". Program will exit.\n", argv[1]); return 0;}
	if(readFromCategoryFile(argv[3]) == 0){printf("Failed to open up \"%s\". Program will exit.\n", argv[2]); return 0;}
	/** Initialize condition variables and mutex */
	pthread_cond_init(&notfull, NULL);
	pthread_cond_init(&notempty, NULL);
	pthread_mutex_init(&lock, NULL);
	pthread_mutex_init(&plock, NULL);
	/** Create producer thread */
	check = pthread_create(&producer, NULL, &readFromOrderFile, argv[2]);
	if(check != 0) {printf("Creating of producer thread failed. Program will exit.\n"); return 0;}	
	/** Create the amount of consumer threads depending on the number of categories in book orders */
	consumer = (pthread_t*)malloc(sizeof(pthread_t) * numOfCategory);
	int i = 0;
  	Category *curr = categoriesList;
	/** Create the consumer threads */
	while(curr != NULL) {
		check = pthread_create(&consumer[i], NULL, &processOrder, curr -> category );
		if(check != 0) {printf("Creating of producer thread failed. Program will exit.\n"); return 0;}
		curr = curr -> nextCategory;	
		i++;
		consumerThreads++;
	}
	i = 0;
	pthread_join(producer, NULL);
	while(i < numOfCategory) {
		pthread_join(consumer[i], NULL);
		i++;
	}
	if(error == 1) {
		printf("A customer with ID: %d was not found. Program will now exit.\n", invalidID);
		return 0;
	}
	else {	
		/** Print out the total revenue */
		printf("\nTotal Revenue: $%.2f\n", revenue);

		/** Print final report of orders */
		printReport();
		printf("Final report is written in \"finalreport.txt\".\n");
	}
	
	/** Destroying the condition variables */
	pthread_cond_destroy(&notfull);
	pthread_cond_destroy(&notempty);
	/** Destroying the mutex locks */
	pthread_mutex_destroy(&lock);
	pthread_mutex_destroy(&plock);
	/** Destroying the consumer thread pointers */
	free(consumer);
	consumer = NULL;
	/** Destroying the data structures */
	initializeCustomerHashMap();
	destroyC_List();
	destroyCategoriesList();
	destroyQueue();
	return 1;
}
