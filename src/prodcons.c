#ifdef _POSIX_C_SOURCE 
    #undef _POSIX_C_SOURCE
#endif
#define _POSIX_C_SOURCE 200112L

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "DBGpthread.h"
#include <time.h>
#include <stdlib.h>

#define N_PRODS 3
#define N_CONS 3

#define PIENO 1 
#define VUOTO 0

int buffer = VUOTO; 

int distribBigliettiProd = 0, turnoProdCorrente = 0;
int distribBigliettiCons = 0, turnoConsCorrente = 0;

/* protegge distribBigliettiProd */
pthread_mutex_t mutexDistribProd = PTHREAD_MUTEX_INITIALIZER;
/* protegge buffer & turnoProdCorrente */
pthread_mutex_t mutexProd = PTHREAD_MUTEX_INITIALIZER;

/* protegge distribBigliettiProd */
pthread_mutex_t mutexDistribCons = PTHREAD_MUTEX_INITIALIZER;
/* protegge buffer & turnoProdCorrente */
pthread_mutex_t mutexCons = PTHREAD_MUTEX_INITIALIZER;

/* consente producer di mettersi in wait se o il buffer è pieno o non è il suo turno di depositare */
pthread_cond_t condTurnoProd = PTHREAD_COND_INITIALIZER;

/* consente consumer di mettersi in wait se o il buffer è vuoto o non è il suo turno di servirsi */
pthread_cond_t condTurnoCons = PTHREAD_COND_INITIALIZER;


void *producer( void *arg ) {
    char producerId[128];
    int biglietto;
    sprintf( producerId, "ProducerID: %" PRIiPTR "", (intptr_t)arg );
    while( 1 ) {
        /* fase di inserimento in coda FIFO produttori */
        DBGpthread_mutex_lock( &mutexDistribProd, producerId );
        biglietto = distribBigliettiProd;
        distribBigliettiProd++;
        DBGpthread_mutex_unlock( &mutexDistribProd, producerId );
        
        /* produzione prodotto */
        DBGsleep( 2, producerId );

        /* controllo se il buffer è pieno o no e se è il mio turno */
        DBGpthread_mutex_lock( &mutexProd, producerId );
        while( buffer >= PIENO || biglietto != turnoProdCorrente ){
            DBGpthread_cond_wait( &condTurnoProd, &mutexProd, producerId );
        }
        /* se il buffer non è pieno ed è il mio turno di riempirlo procedo
         *(in questo punto del codice ho sicuramnete la mutua esclusione per )
         */
        /* Il deposito nel buffer richiede 1 secondo (che non può essere parallelizzato) */
        DBGsleep( 1, producerId );
        buffer = PIENO;
        printf( "%s filled the buffer (ticket: %d)\n", producerId, biglietto );
        turnoProdCorrente++;
        DBGpthread_cond_broadcast( &condTurnoCons, producerId );
        DBGpthread_mutex_unlock( &mutexProd, producerId );
    }
    pthread_exit( NULL );
}

void *consumer( void *arg ) {
    char consumerId[128];
    int biglietto;
    sprintf( consumerId, "ConsumerID: %" PRIiPTR "", (intptr_t)arg );
    
    while( 1 ) {
        /* fase di inserimento in coda FIFO consumatori */
        DBGpthread_mutex_lock( &mutexDistribCons, consumerId );
        biglietto = distribBigliettiCons;
        distribBigliettiCons++;
        DBGpthread_mutex_unlock( &mutexDistribCons, consumerId );

         /* controllo se il buffer è vuoto o no e se è il mio turno */
        DBGpthread_mutex_lock( &mutexProd, consumerId );
        while ( buffer <= VUOTO || biglietto != turnoConsCorrente ) {
            DBGpthread_cond_wait(&condTurnoCons, &mutexProd, consumerId );
        }
        /* se il buffer non è vuoto ed è il mio turno di prelevare procedo
         *(in questo punto del codice ho sicuramnete la mutua esclusione per )
         */
        /* Prelevare dal buffer richiede 1 secondo (che non può essere parallelizzato) */
        DBGsleep( 1, consumerId );
        buffer = VUOTO;
        printf( "%s emptied the buffer (ticket: %d)\n", consumerId, biglietto );
        turnoConsCorrente++;
        DBGpthread_cond_broadcast( &condTurnoProd, consumerId );
        DBGpthread_mutex_unlock( &mutexProd, consumerId );
    }
    pthread_exit( NULL );
} 


int main( void ) {
    
    pthread_t tid;
    intptr_t t;
    
    for( t=0; t<N_CONS; t++ ) {
        DBGpthread_create( &tid, NULL, consumer, (void *)t, "creazione consumator" );
    }

    for( t=0; t<N_PRODS; t++ ) {
        DBGpthread_create( &tid, NULL, producer, (void *)t, "creazione produttore" );
    } 

    pthread_exit( NULL );
    return 0;
}