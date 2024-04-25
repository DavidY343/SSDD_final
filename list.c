#include "list.h"
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

extern List messages;
extern int inited_list = 0;
extern pthread_mutex_t mutex;

int init_list()
{
    pthread_mutex_lock(&mutex);
    if (inited_list)
    {
        pthread_mutex_unlock(&mutex);
        return (0);
    }
    messages = NULL;
    inited_list = 1;
    pthread_mutex_unlock(&mutex);
    return 0;
}

int destroy_list() {
    pthread_mutex_lock(&mutex);
    if (!inited_list)
        init_list();

    // Liberar la lista de usuarios
    node_t *current = messages;
    while (current) {
        node_t *next = current->next;
        
        // Liberar la lista de archivos
        filelist_t *file_current = current->list_files;
        while (file_current) {
            filelist_t *file_next = file_current->next;
            free(file_current);
            file_current = file_next;
        }

        free(current); // Liberar el nodo de usuario
        current = next;
    }

    messages = NULL;
    inited_list = 0;
    pthread_mutex_unlock(&mutex);
    return 0;
}

int register_user(char *username)
{
    pthread_mutex_lock(&mutex);
    if (!inited_list)
    {
        pthread_mutex_unlock(&mutex);
        init_list();
        pthread_mutex_lock(&mutex);
    }
    // Verificar si el usuario ya existe
    node_t *current = messages;
    while (current){
        if (strcmp(current->username, username) == 0)
        {
            pthread_mutex_unlock(&mutex);
            return 1;
        }
        current = current->next;
    }

    // Crear un nuevo nodo para el usuario
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (!new_node) {
        pthread_mutex_unlock(&mutex);
        return -1; // Error al asignar memoria
    }

    strncpy(new_node->username, username, MAX_VALUE_LENGTH);
    new_node->list_files = NULL;
    new_node->next = messages;
    messages = new_node;

    pthread_mutex_unlock(&mutex);
    return 0;
}

int unregister_user(char *username)
{
    pthread_mutex_lock(&mutex);
	if (!inited_list)
    {
        pthread_mutex_unlock(&mutex);
        init_list();
        pthread_mutex_lock(&mutex);
    }
    node_t *current = messages;
    node_t *previous = NULL;

    // Buscar al usuario en la lista
    while (current) {
        if (strcmp(current->username, username) == 0) {
            if (previous) {
                previous->next = current->next;
            } else {
                messages = current->next; // Eliminar el primer nodo
            }

            // Liberar el nodo de usuario y sus archivos
            filelist_t *file_current = current->list_files;
            while (file_current) {
                filelist_t *file_next = file_current->next;
                free(file_current);
                file_current = file_next;
            }

            free(current); // Liberar el nodo de usuario
            pthread_mutex_unlock(&mutex);
            return 0;
        }

        previous = current;
        current = current->next;
    }

    pthread_mutex_unlock(&mutex);
    return 1; // Usuario no encontrado
}

int connect_user(int socket, char *port, char *username)
{
    pthread_mutex_lock(&mutex);
    if (!inited_list)
    {
        pthread_mutex_unlock(&mutex);
        init_list();
        pthread_mutex_lock(&mutex);
    }
    node_t *current = messages;

    // Buscar al usuario por su nombre
    while (current) {
        if (strcmp(current->username, username) == 0) {
            if (strlen(current->ip) > 0) {
                pthread_mutex_unlock(&mutex);
                return 2; // El usuario ya está conectado
            }

            // Obtener la dirección IP del socket
            struct sockaddr_in peer_addr;
            socklen_t addr_len = sizeof(peer_addr);
            if (getpeername(socket, (struct sockaddr *)&peer_addr, &addr_len) == 0) {
                strncpy(current->ip, inet_ntoa(peer_addr.sin_addr), MAX_VALUE_LENGTH);
                strncpy(current->port, port, MAX_VALUE_LENGTH);
                pthread_mutex_unlock(&mutex);
                return 0; // Conexión exitosa
            }

            pthread_mutex_unlock(&mutex);
            return 3; // Error al obtener la dirección IP
        }

        current = current->next;
    }

    pthread_mutex_unlock(&mutex);
    return 1; // Usuario no encontrado
}
