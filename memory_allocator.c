#include <pthread.h>
#include <string.h>
#include <unistd.h>

union blockHeader {

    struct {
        size_t size ;
        unsigned isFree ;
        union blockHeader *next ;
    } struc ;

    char alignmentBytes[16] ;


};
typedef union blockHeader blockHeader_t ;


blockHeader_t *getFreeBlock(size_t size);


blockHeader_t *head = NULL,*tail = NULL;
pthread_mutex_t lock ;

void *malloc(size_t size) {

    blockHeader_t *header ;
    void *block ;

    
    
    if (!size) {
        return NULL ;
    }

    // Search for a free block that exists

    pthread_mutex_lock(&lock);

    header = getFreeBlock(size) ;
    if (header){
        header->struc.isFree = 0  ;
        pthread_mutex_unlock(&lock);
        return (void*) (header + 1 ) ;
 
    }

    // No free block already exists, we extend the heap


    block = sbrk(sizeof(blockHeader_t) + size ) ;
    if ( block == (void*) -1) {
        pthread_mutex_unlock(&lock);
        return NULL ;

    }


    // Update the linked list
    header = block ;
    header->struc.size = size ;
    header->struc.isFree = 0 ;
    header->struc.next = NULL ;

    if (!head){
        head = header ;
    }
    if (tail) {
        tail->struc.next = header ;
    }

    tail = header ;
    pthread_mutex_unlock(&lock);
    // Return the adress of the usefull block (hide the header layer)
    return (void*) (header + 1) ;


}

blockHeader_t *getFreeBlock(size_t size){

    blockHeader_t *current ;

    current = head ;
    // Go through the linked list
    while (current) {
            if ( current->struc.isFree  && current->struc.size >= size){
                return current ;
            }
            current = current->struc.next  ;

    }

    return NULL ;

}


void free(void *block){
    // If the block at the end of the heap, release it to the OS else mark it 
    // free for later allocation

    blockHeader_t *header,*temp ;
    void *heapBreak;
    void *blockAdress ;
    
    if(!block) {
        return ;
    }

    pthread_mutex_lock(&lock);

    // arithmetic operations are illegal for void* so we cast it
    header = (blockHeader_t*)block - 1 ;
    // Get the heap break
    heapBreak = sbrk(0) ;
    blockAdress = (void*)( (char* )block + header->struc.size );

    if (blockAdress == heapBreak){

        // if the block at the end of the heap
        // Remove the item from the linked list
        if(head == tail){
            
            head = tail = NULL ;
        }
        else {
            temp = head ;
            while (temp)
            {
                if(temp->struc.next == tail){
                    temp->struc.next = NULL ;
                    tail = temp ;
                }
                temp = temp->struc.next ;
            }
        }
        // Free the space by calling sbrk with a negative size 

        sbrk(- sizeof(blockHeader_t) - header->struc.size );
        pthread_mutex_unlock(&lock);
        return ;
    }


    // if block not at the end of the heap mark it free 

    header->struc.isFree = 1 ;
    pthread_mutex_unlock(&lock);

}


void *calloc(size_t n , size_t nsize){
    // Allocates an array of n elements of nsize and initialize them to 0
    size_t size ; 
    void *block ;

    if (!n || !nsize) {
        return NULL ;
    }

    size = n * nsize ;
    // Check for multiplication overflow
    if (nsize != size / n){
        return NULL;
        }

    block = malloc(size);
    if(!block){
        return NULL ;
    }

    memset(block,0,size);
    return block ;


}


void* realloc(void* block, size_t new_size){
    // Changes the size of an allocated block

    blockHeader_t *header ;
    void *new_block ;

    if(!block || ! !new_size){
        return malloc(new_size);
    }

    header = (blockHeader_t*) block - 1 ;
    // Check if new size equal older size
    if( header->struc.size >= new_size){
        return block ;
    }
    // Allocate a new block and free the older one.
    new_block = malloc(new_size) ;

    if(new_block){
        memcpy(new_block,block,header->struc.size);
        free(block) ;
        
    }

	return new_block;
	
}
