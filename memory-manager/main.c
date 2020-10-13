//
//  main.c
//  memory-manager
//
//  Created by Kiril Malenchik on 10/10/20.
//

#include <stdlib.h>

#include "memory/memory.h"

#include <stdio.h>




void errorAbort(int error_code);

int main(int argc, const char * argv[]) {
    m_init(1,500);
    int error_code;
    
    m_id chunk_1 = m_malloc(13, &error_code);
    errorAbort(error_code);
    
    m_id chunk_2 = m_malloc(20, &error_code);
    errorAbort(error_code);

    m_id chunk_3 = m_malloc(50, &error_code);
    errorAbort(error_code);
    
    m_write(chunk_1, "Hello World!", 13, &error_code);
    errorAbort(error_code);

    m_write(chunk_2, "Operating Systems", 18, &error_code);
    errorAbort(error_code);

    m_write(chunk_3, "Super dumb memory allocator", 28, &error_code);
    errorAbort(error_code);

    char buffer[50];

    m_read(chunk_1, buffer, 13, &error_code);
    errorAbort(error_code);
    printf("%s\n", buffer);

    m_read(chunk_2, buffer, 18, &error_code);
    errorAbort(error_code);
    printf("%s\n", buffer);

    m_read(chunk_3, buffer, 28, &error_code);
    errorAbort(error_code);
    printf("%s\n", buffer);

    m_free(chunk_1, &error_code);
    errorAbort(error_code);

    m_free(chunk_2, &error_code);
    errorAbort(error_code);

    m_free(chunk_3, &error_code);
    errorAbort(error_code);
}

void errorAbort(int error_code){
    
    if (error_code != M_ERR_OK){
        printf("%d\n", error_code);
        abort();
    }
}

//void toArray(m_id* idChunk, int sizeChunk, int index){
//    arrayOfChunks[index].m_id = idChunk;
//    arrayOfChunks[index].size = sizeChunk;
//}

// TODO:

// придумать структуру хранения сегментов

// додумать логику

// очистка удаляет сегмент

// 6 байт есть, но они не вместе ( последовательно ) ( написать такой тест ) ( сегменты не передвигаются ( показательные тесты )

// хранится должно в ините и если его вывести, то выведится все подряд в ините
