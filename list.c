#include "list.h"
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

List messages;
int inited_list = 0;
pthread_mutex_t mutex;

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
            if (previous)
            {
                previous->next = current->next;
            } else
            {
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
            if (strlen(current->ip) > 0)
            {
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

int disconnect_user(char *username)
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
            if (strlen(current->ip) == 0) {
                pthread_mutex_unlock(&mutex);
                return 2; // Usuario no está conectado
            }

            // Desconectar al usuario
            current->ip[0] = '\0'; // Reiniciar la dirección IP
            current->port[0] = '\0'; // Reiniciar el puerto

            pthread_mutex_unlock(&mutex);
            return 0; // Desconexión exitosa
        }

        current = current->next;
    }

    pthread_mutex_unlock(&mutex);
    return 1; // Usuario no encontrado
}

int register_file_from_user(char *username, char *file, char *description) {
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
            if (strlen(current->ip) == 0) {
                pthread_mutex_unlock(&mutex);
                return 2; // Usuario no está conectado
            }

            // Verificar si el archivo ya está registrado
            filelist_t *file_current = current->list_files;
            while (file_current) {
                if (strcmp(file_current->filename, file) == 0) {
                    pthread_mutex_unlock(&mutex);
                    return 3; // Archivo ya está publicado
                }

                file_current = file_current->next;
            }

            // Registrar el nuevo archivo y su descripción
            filelist_t *new_file = (filelist_t *)malloc(sizeof(filelist_t));
            if (!new_file) {
                pthread_mutex_unlock(&mutex);
                return 4; // Error al asignar memoria
            }

            strncpy(new_file->filename, file, MAX_VALUE_LENGTH);
            strncpy(new_file->description, description, MAX_VALUE_LENGTH);
            new_file->next = current->list_files; // Insertar al inicio de la lista
            current->list_files = new_file;

            pthread_mutex_unlock(&mutex);
            return 0; // Registro exitoso
        }

        current = current->next;
    }

    pthread_mutex_unlock(&mutex);
    return 1; // Usuario no encontrado
}

int unregister_file_from_user(char *username, char *file) {
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
            if (strlen(current->ip) == 0) {
                pthread_mutex_unlock(&mutex);
                return 2; // Usuario no está conectado
            }

            filelist_t *file_current = current->list_files;
            filelist_t *file_previous = NULL;

            // Buscar el archivo en la lista de archivos
            while (file_current) {
                if (strcmp(file_current->filename, file) == 0) {
                    if (file_previous) {
                        file_previous->next = file_current->next;
                    } else {
                        current->list_files = file_current->next; // Eliminar el primer archivo
                    }

                    free(file_current); // Liberar la memoria del archivo
                    pthread_mutex_unlock(&mutex);
                    return 0; // Archivo eliminado exitosamente
                }

                file_previous = file_current;
                file_current = file_current->next;
            }

            pthread_mutex_unlock(&mutex);
            return 3; // Archivo no encontrado
        }

        current = current->next;
    }

    pthread_mutex_unlock(&mutex);
    return 1; // Usuario no encontrado
}

int list_users(char *username, t_response_user *respuesta, int *n_user) {
    pthread_mutex_lock(&mutex);

    node_t *current = messages;

    // Buscar al usuario que realiza la operación para comprobar si está conectado
    while (current) {
        if (strcmp(current->username, username) == 0) {
            if (strlen(current->ip) == 0) {
                pthread_mutex_unlock(&mutex);
                return 2; // Usuario no está conectado
            }

            // Si está conectado, recorrer la lista de usuarios conectados y agregarlos a la respuesta
            int user_count = 0;
            node_t *user_current = messages;
            t_response_user *response_current = respuesta;

            while (user_current) {
                if (strlen(user_current->ip) > 0) { // Solo agregar usuarios conectados
                    strncpy(response_current->username, user_current->username, MAX_VALUE_LENGTH);
                    strncpy(response_current->ip, user_current->ip, MAX_VALUE_LENGTH);
                    strncpy(response_current->port, user_current->port, MAX_VALUE_LENGTH);

                    // Crear la siguiente respuesta si hay más usuarios conectados
                    if (user_current->next) {
                        response_current->next = (t_response_user *)malloc(sizeof(t_response_user));
                        response_current = response_current->next;
                    } else {
                        response_current->next = NULL;
                    }

                    user_count++; // Contar el número de usuarios conectados
                }

                user_current = user_current->next;
            }

            *n_user = user_count; // Devolver el número de usuarios conectados

            pthread_mutex_unlock(&mutex);
            return 0; // Éxito
        }

        current = current->next;
    }

    pthread_mutex_unlock(&mutex);
    return 1; // Usuario que solicita no encontrado
}

int list_content(char *username, t_response_list *respuesta, char *username_2) {
    pthread_mutex_lock(&mutex);

    node_t *current = messages;
    node_t *target_user = NULL;

    // Verificar que el usuario que realiza la operación está conectado
    while (current) {
        if (strcmp(current->username, username) == 0) {
            if (strlen(current->ip) == 0) {
                pthread_mutex_unlock(&mutex);
                return 2; // Usuario que realiza la operación no está conectado
            }

            break; // Encontramos al usuario que realiza la operación
        }

        current = current->next;
    }

    if (!current) {
        pthread_mutex_unlock(&mutex);
        return 1; // Usuario que realiza la operación no encontrado
    }

    // Buscar al usuario cuyos archivos queremos listar
    current = messages;
    while (current) {
        if (strcmp(current->username, username_2) == 0) {
            target_user = current; // Usuario cuyo contenido queremos listar
            break;
        }

        current = current->next;
    }

    if (!target_user) {
        pthread_mutex_unlock(&mutex);
        return 3; // Usuario cuyo contenido queremos listar no encontrado
    }

    t_response_list *response_current = respuesta;
    filelist_t *file_current = target_user->list_files;

    // Agregar la lista de archivos del usuario objetivo
    while (file_current) {
        strncpy(response_current->filename, file_current->filename, MAX_VALUE_LENGTH);
        strncpy(response_current->description, file_current->description, MAX_VALUE_LENGTH);

        if (file_current->next) {
            response_current->next = (t_response_list *)malloc(sizeof(t_response_list));
            response_current = response_current->next;
        } else {
            response_current->next = NULL;
        }

        file_current = file_current->next;
    }

    pthread_mutex_unlock(&mutex);
    return 0; // Éxito
}