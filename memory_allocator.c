#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

union blockHeader {

    struct {
        size_t size ;
        uint8_t isFree ;
        union blockHeader *next ;
    } struc ;

    char alignmentBytes[16] ;


};

typedef union blockHeader blockHeader_t ;




blockHeader_t *head ,*tail ;

pthread_mutex_t global_lock ;




void *malloc(size_t size) {

    void *block ;
    blockHeader_t *header ;
    
    if (!size) {
        return NULL ;
    }



    block = sbrk(sizeof(blockHeader_t) + size) ;
    if ( block == (void*) -1) {
        return NULL ;

    }

    *tail = block ;

    header = block ;
    header->struc.size = size ;
    header->struc.isFree = 0 ;
    header->struc.next = NULL ;

    if (!head){
        head = header ;
    }
    if (!tail) {
        tail = header ;
    }

    tail = header ;

    return block + sizeof() ;




    return block ;


}