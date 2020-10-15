//
//  memory.c
//  memory-manager
//
//  Created by Kiril Malenchik on 10/10/20.
//

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "memory.h"

char* _g_allocator_memory = NULL;
int _g_allocator_memory_size = 0;
int _g_bytes_allocated = 0;

struct Chunks {
    void* m_id;
    int size;
};

struct Chunks arrayOfChunks[10];
int number_of_chunks = 0;

m_id m_malloc(int size_of_chunk, m_err_code* error) {
    
  if (_g_bytes_allocated + size_of_chunk > _g_allocator_memory_size) {
    *error = M_ERR_ALLOCATION_OUT_OF_MEMORY;
    return NULL;
  }

  _g_bytes_allocated += size_of_chunk;
    
    arrayOfChunks[number_of_chunks].m_id = _g_allocator_memory + _g_bytes_allocated;
    arrayOfChunks[number_of_chunks].size = size_of_chunk;
    number_of_chunks++;

  *error = M_ERR_OK;
  return _g_allocator_memory + _g_bytes_allocated;
}


void m_free(m_id ptr, m_err_code* error) {
    
    for (int i = 0; i < number_of_chunks; i++) {
        
        if (ptr == arrayOfChunks[i].m_id) {
            
            arrayOfChunks[i].m_id = NULL;
            deleteChunkMemory(ptr);
            ptr = NULL;
            
            *error = M_ERR_OK;
            return;
        }
    }
    
    *error = M_ERR_ALREADY_DEALLOCATED;
}


void m_read(m_id read_from_id, void* read_to_buffer, int size_to_read, m_err_code* error) {
    
    for (int i = 0; i < number_of_chunks; i++) {
        if (read_from_id == arrayOfChunks[i].m_id) {
            if (size_to_read > arrayOfChunks[i].size) {
                *error = M_ERR_OUT_OF_BOUNDS;
                return;
            }
        }
        
        if (arrayOfChunks[i].m_id == NULL) {
            *error = M_ERR_INVALID_CHUNK;
            return;
        }
    }
    
  memcpy(read_to_buffer, read_from_id, size_to_read);
  *error = M_ERR_OK;
}


void m_write(m_id write_to_id, void* write_from_buffer, int size_to_write, m_err_code* error) {
    
    for (int i = 0; i < number_of_chunks; i++) {
        if (write_to_id == arrayOfChunks[i].m_id) {
            if (size_to_write > arrayOfChunks[i].size) {
                *error = M_ERR_ALLOCATION_OUT_OF_MEMORY;
                return;
            }
            
            if (arrayOfChunks[i].m_id == NULL) {
                *error = M_ERR_INVALID_CHUNK;
                return;
            }
        }
    }
    
  memcpy(write_to_id, write_from_buffer, size_to_write);
  *error = M_ERR_OK;
}

void m_writeToFreeChunk(void* write_from_buffer, int size_to_write){
    
    bool m_write_done = false;
    int selected_chunk_index = 0;
    
    for (int i = 0; i < number_of_chunks; i++) {
        if (size_to_write == arrayOfChunks[i].size) {
            
            arrayOfChunks[i].m_id = _g_allocator_memory + arrayOfChunks[i].size;
            memcpy(arrayOfChunks[i].m_id, write_from_buffer, size_to_write);
            
            m_write_done = true;
            selected_chunk_index = i;
            
            break;
        }
    }
    
    if (m_write_done) printf("%s%d\n", "Information recorded successfully in chunk: ", selected_chunk_index);
    else printf("%s\n", "Information not recorded successfully, no chunk with selected size");
    
}

void m_init(int number_of_pages, int size_of_page) {
  if (_g_allocator_memory != NULL) free(_g_allocator_memory);

  _g_allocator_memory_size = number_of_pages * size_of_page;
  _g_allocator_memory = malloc(_g_allocator_memory_size);
  _g_bytes_allocated = 0;

}

void* getAllocatorMemory(){
    return _g_allocator_memory;
}

void deleteChunkMemory(m_id* m_id) {
    *m_id = NULL;
}
