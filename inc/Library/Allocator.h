#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "../Container/Vector.h"

#if defined __linux__
#include <sys/mman.h>
#elif defined _WIN32
#include <windows.h>
#else
#error "Plattform not supported!"
#endif


typedef struct AllocatorHeader {
	unsigned int length;
    unsigned int flags;
} AllocatorHeader;


#define BLOCKPRINT_MAX	16U

typedef struct Allocator {
	void* memory;
    unsigned long length;
} Allocator;

Allocator Allocator_New(unsigned long length){
	Allocator ac;
    #if defined __linux__
    ac.memory = mmap(NULL,length,PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS,-1,0);
    #elif defined _WIN32
    ac.memory = VirtualAlloc(
        NULL,               
        length,             
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );
    #else
    #error "Plattform not supported!"
    #endif
	
    ac.length = length;
    memset(ac.memory,0,length);
    
	AllocatorHeader* first = ac.memory;
    first->length = length - sizeof(AllocatorHeader);
    first->flags = 0U;
	return ac;
}
void Allocator_Free(Allocator* ac){
	if(ac->memory != NULL){
        #if defined __linux__
        munmap(ac->memory,ac->length);
        #elif defined _WIN32
        if (!VirtualFree(ac->memory,0,MEM_RELEASE)) {
            printf("[Allocator]: FreeVirtualFree -> Error!\n");
        }
        #else
        #error "Plattform not supported!"
        #endif
    }
    ac->memory = NULL;
    ac->length = 0;
}

void* Allocator_BlockAlloc(Allocator* ac,unsigned long size){
    void* ptr = NULL;
    AllocatorHeader* first = ac->memory;
    
    while(1){
        if((char*)first + sizeof(AllocatorHeader) + size > (char*)ac->memory + ac->length){
			printf("[Allocator]: BlockAlloc -> size not available!\n");
            return NULL;
        }else if(first->flags == 0 && first->length >= size){
            unsigned long rem = first->length - size;
            
			if(rem <= sizeof(AllocatorHeader)){
                size = first->length;
            }

            first->flags = 1;
            first->length = size;
            ptr = (char*)first + sizeof(AllocatorHeader);

            AllocatorHeader* next = (AllocatorHeader*)((char*)ptr + size);
            if(rem > sizeof(AllocatorHeader) && (char*)next + rem <= (char*)ac->memory + ac->length){
                next->length = rem - sizeof(AllocatorHeader);
                next->flags = 0;
            }
            return ptr;
        }else{
            first = (AllocatorHeader*)((char*)first + sizeof(AllocatorHeader) + first->length);
        }
    }
    return ptr;
}
void Allocator_BlockFree(Allocator* ac,void* ptr){
    if((char*)ptr < (char*)ac->memory || (char*)ptr > (char*)ac->memory + ac->length){
		printf("[Allocator]: BlockFree -> pointer is not in the range of the allocator!\n");
		return;
	}
    
    AllocatorHeader* prev = NULL;
    AllocatorHeader* first = ac->memory;
    
    while(1){
        if((char*)first + sizeof(AllocatorHeader) + first->length > (char*)ac->memory + ac->length){
			printf("[Allocator]: BlockFree -> pointer is not in the range of the allocator!\n");
            return;
        }else if((char*)ptr >= (char*)first && (char*)ptr < (char*)first + sizeof(AllocatorHeader) + first->length){
            if(first->flags == 0){
				printf("[Allocator]: BlockFree -> double free pointer!\n");
				return;
			}
			
			first->flags = 0;
            
            AllocatorHeader* next = (AllocatorHeader*)((char*)first + sizeof(AllocatorHeader) + first->length);
            if((void*)next < ac->memory + ac->length){// remove =
                if(next->flags==0){
                    first->length += sizeof(AllocatorHeader) + next->length;
                }
            }
            if((void*)prev >= ac->memory){
                if(prev->flags==0){
                    prev->length += sizeof(AllocatorHeader) + first->length;
                }
            }
			return;
        }else{
            prev = first;
            first = (AllocatorHeader*)((char*)first + sizeof(AllocatorHeader) + first->length);
        }
    }
}
void* Allocator_BlockRealloc(Allocator* ac,void* ptr,unsigned long size){
    if((char*)ptr < (char*)ac->memory || (char*)ptr > (char*)ac->memory + ac->length){
		printf("[Allocator]: BlockRealloc -> pointer is not in the range of the allocator!\n");
        return NULL;
    }
    
    AllocatorHeader* first = ac->memory;
    
    while(1){
        if((char*)first + sizeof(AllocatorHeader) + first->length > (char*)ac->memory + ac->length){
            return NULL;
        }else if((char*)ptr >= (char*)first && (char*)ptr < (char*)first + sizeof(AllocatorHeader) + first->length){
            AllocatorHeader* next = (AllocatorHeader*)((char*)first + sizeof(AllocatorHeader) + first->length);
            
            if(first->length>size){
                long diff = first->length - size - sizeof(AllocatorHeader);
                if(first->length > size - sizeof(AllocatorHeader)){
                    first->length = size;
                    AllocatorHeader* renext = (AllocatorHeader*)((char*)first + sizeof(AllocatorHeader) + first->length);
                    renext->flags = 0;
                    renext->length = diff;
                }
                return ptr;
            }else if(next->flags==0){
                if(first->length + sizeof(AllocatorHeader) + next->length > size){
                    long diff = first->length + next->length - size;// +  sizeof(AllocatorHeader) & -  sizeof(AllocatorHeader) => 0
                    if(first->length + next->length > size){
                        first->length = size;
                        
						AllocatorHeader* renext = (AllocatorHeader*)((char*)first + sizeof(AllocatorHeader) + first->length);
                        renext->flags = 0;
                        renext->length = diff;
                    }else{
                        first->length += sizeof(AllocatorHeader) + next->length;
                    }
                    return ptr;
                }
            }
            void* newptr = Allocator_BlockAlloc(ac,size);
            memcpy(newptr,ptr,first->length);
            Allocator_BlockFree(ac,ptr);
            return newptr;
        }else{
            first = (AllocatorHeader*)((char*)first + sizeof(AllocatorHeader) + first->length);
        }
    }
    return ptr;
}
void* Allocator_BlockCalloc(Allocator* ac,unsigned long count,unsigned long elementsize){
    void* ptr = Allocator_BlockAlloc(ac,count * elementsize);
    memset(ptr,0,count * elementsize);
    return ptr;
}

void Allocator_Print(Allocator* ac){
    printf("------------------ Allocator -----------------\n");
    printf("| Memory: %p\n",ac->memory);
    printf("| Length: %lu\n",ac->length);

    AllocatorHeader* first = ac->memory;
    while((void*)first>=ac->memory && (void*)first<ac->memory + ac->length){
        printf("| Block: %u (%x):\t",first->length,first->flags);
		
		unsigned int maxLen = first->length < BLOCKPRINT_MAX ? first->length : BLOCKPRINT_MAX;
		for(int i = 0;i<maxLen;i++){
			unsigned char byte = *(unsigned char*)((char*)first + sizeof(AllocatorHeader) + i);
			if(byte<16) printf("0");
			printf("%x",byte);
		}
		if(maxLen == BLOCKPRINT_MAX) printf("...\t");

		char Buffer[BLOCKPRINT_MAX + 1];
		memset(Buffer,0,BLOCKPRINT_MAX + 1);
		memcpy(Buffer,(char*)first + sizeof(AllocatorHeader),maxLen);
		printf("%s",Buffer);

		if(maxLen == BLOCKPRINT_MAX) printf("...\t");

        printf("\n");
        first = (AllocatorHeader*)((char*)first + sizeof(AllocatorHeader) + first->length);
    }
    printf("----------------------------------------------\n");
}

#endif //!ALLOCATOR_H