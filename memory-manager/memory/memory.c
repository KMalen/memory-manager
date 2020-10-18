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
    struct Chunks* next;
};

/* Определяем элемент списка */
typedef struct list_node {
    struct list_node *next;
    int size;
    m_id m_id;
} node_chunk;

/* Определяем сам список */
typedef struct list {
    /*
     * Размер списка хранить не обязательно,
     * он нужен для упрощения работы
     */
    int size;
    /* начало списка */
    node_chunk *head;
    /* конец списка */
    node_chunk *tail;
} chunks_list;

/* Инициализация массива */
chunks_list * create_list(void)
{
    chunks_list *lt = malloc(sizeof(chunks_list));

    lt->size = 0;
    lt->head = NULL;
    lt->tail = lt->head;

    return lt;
}

/* Добавляем элемент в начало списка */
void list_push(chunks_list *lt, m_id ptr_chunk, int size_chunk)
{
    node_chunk * node = malloc(sizeof(node_chunk));
    node->m_id = ptr_chunk;
    node->size = size_chunk;
    
    node->next = lt->head;

    lt->head = node;
    lt->size += 1;
}

/* Добавляем элемент в конец списка */
void list_push_back(chunks_list *lt, m_id ptr_chunk, int size_chunk)
{
    node_chunk * node = malloc(sizeof(node_chunk));
    node->m_id = ptr_chunk;
    node->size = size_chunk;
    
    if(lt->tail != NULL)
        lt->tail->next = node;
    else {
        lt->head = node;
    }

    lt->tail = node;
    lt->size += 1;
}

chunks_list *list_of_chunks;

struct Chunks arrayOfChunks[10];
int number_of_chunks = 0;

m_id m_malloc(int size_of_chunk, m_err_code* error) {
    
    node_chunk *temp_node_chunk;
    temp_node_chunk = list_of_chunks->head;
    int all_size = 0;
    
    if (list_of_chunks->size == 0){
        
        list_push_back(list_of_chunks, _g_allocator_memory + _g_bytes_allocated, size_of_chunk);

        *error = M_ERR_OK;
        return _g_allocator_memory + _g_bytes_allocated;
    }
    
    for (int i = 0; i < list_of_chunks->size; i++) {
        
        if (list_of_chunks->head->m_id == list_of_chunks->tail->m_id) {
            break;
        }
        
        all_size += temp_node_chunk->size;
        
        if (list_of_chunks->head->m_id == NULL) {
            
            list_of_chunks->head->m_id = _g_allocator_memory + all_size;
            
            *error = M_ERR_OK;
            return _g_allocator_memory + all_size;
            
        }
        
        if (list_of_chunks->tail->m_id == NULL) {
            
            list_of_chunks->tail->m_id = _g_allocator_memory + all_size;
            
            *error = M_ERR_OK;
            return _g_allocator_memory + all_size;
            
        }
        
        if (temp_node_chunk->next != NULL && temp_node_chunk->next->m_id == NULL) {
            
            all_size += temp_node_chunk->next->size;
            temp_node_chunk->next->m_id = _g_allocator_memory + all_size + list_of_chunks->head->size;
            
            *error = M_ERR_OK;
            return _g_allocator_memory + all_size;
    
        }
        
        temp_node_chunk = temp_node_chunk->next;
        
    }
    
    if (_g_bytes_allocated + size_of_chunk > _g_allocator_memory_size) {
      *error = M_ERR_ALLOCATION_OUT_OF_MEMORY;
      return NULL;
    }

  _g_bytes_allocated += size_of_chunk;
    
    list_push_back(list_of_chunks, _g_allocator_memory + _g_bytes_allocated, size_of_chunk);

  *error = M_ERR_OK;
  return _g_allocator_memory + _g_bytes_allocated;
}


void m_free(m_id ptr, m_err_code* error) {
    
    if (list_of_chunks->head->m_id == ptr) {
        
        list_of_chunks->head->m_id = NULL;
        memset(ptr,0,list_of_chunks->head->size);
        
        *error = M_ERR_OK;
        return;
        
    }
    
    if (list_of_chunks->tail->m_id == ptr) {
        
        list_of_chunks->tail->m_id = NULL;
        memset(ptr,0,list_of_chunks->tail->size);
        
        *error = M_ERR_OK;
        return;
        
    }
    
    node_chunk *temp_node_chunk;
    temp_node_chunk = list_of_chunks->head;
    
    for (int i = 0; i < list_of_chunks->size; i++) {
        
        if (temp_node_chunk->next->m_id == ptr) {
            
            temp_node_chunk->next->m_id = NULL;
            memset(ptr,0,temp_node_chunk->next->size);
            
            *error = M_ERR_OK;
            return;
        }
        
        temp_node_chunk = temp_node_chunk->next;
        
    }
    
    
    *error = M_ERR_ALREADY_DEALLOCATED;
}


void m_read(m_id read_from_id, void* read_to_buffer, int size_to_read, m_err_code* error) {
    
    node_chunk *temp_node_chunk;
    temp_node_chunk = list_of_chunks->head;
    
    for (int i = 0; i < list_of_chunks->size; i++){
        
        if (temp_node_chunk->m_id == read_from_id) {
            if (size_to_read > temp_node_chunk->size){
                *error = M_ERR_OUT_OF_BOUNDS;
                return;
            }
        }
        
        if (temp_node_chunk->m_id == NULL){
            *error = M_ERR_INVALID_CHUNK;
            return;
        }
        
        temp_node_chunk = temp_node_chunk->next;
        
    }
    
  memcpy(read_to_buffer, read_from_id, size_to_read);
  *error = M_ERR_OK;
}


void m_write(m_id write_to_id, void* write_from_buffer, int size_to_write, m_err_code* error) {
    
    node_chunk *temp_node_chunk;
    temp_node_chunk = list_of_chunks->head;
    
    for (int i = 0; i < list_of_chunks->size; i++) {
        
        if (temp_node_chunk->m_id == write_to_id) {
            if (size_to_write > temp_node_chunk->size){
                *error = M_ERR_ALLOCATION_OUT_OF_MEMORY;
                return;
            }
            
            if (temp_node_chunk->m_id == NULL) {
                *error = M_ERR_INVALID_CHUNK;
                return;
            }
            
        }
        
        temp_node_chunk = temp_node_chunk->next;
    }
    
  memcpy(write_to_id, write_from_buffer, size_to_write);
  *error = M_ERR_OK;
}

//void m_writeToFreeChunk(void* write_from_buffer, int size_to_write){
//
//    bool m_write_done = false;
//    int selected_chunk_index = 0;
//
//    for (int i = 0; i < number_of_chunks; i++) {
//        if (size_to_write == arrayOfChunks[i].size) {
//
//            arrayOfChunks[i].m_id = _g_allocator_memory + arrayOfChunks[i].size;
//            memcpy(arrayOfChunks[i].m_id, write_from_buffer, size_to_write);
//
//            m_write_done = true;
//            selected_chunk_index = i;
//
//            break;
//        }
//    }
//
//    if (m_write_done) printf("%s%d\n", "Information recorded successfully in chunk: ", selected_chunk_index);
//    else printf("%s\n", "Information not recorded successfully, no chunk with selected size");
//
//}

void m_init(int number_of_pages, int size_of_page) {
  if (_g_allocator_memory != NULL) free(_g_allocator_memory);

  _g_allocator_memory_size = number_of_pages * size_of_page;
  _g_allocator_memory = malloc(_g_allocator_memory_size);
  _g_bytes_allocated = 0;

    list_of_chunks = create_list();
    
}

void* getAllocatorMemory(){
    return _g_allocator_memory;
}

void deleteChunkMemory(m_id* m_id) {
    *m_id = NULL;
}
