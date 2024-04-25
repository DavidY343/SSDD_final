#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "claves.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include "lines.h"
#include "mensaje.h"
#include "list.h"
#define MAX_VALUE_LENGTH 256

CLIENT *clnt;
int sd;
pthread_mutex_t mutex_mensaje;
pthread_cond_t cond_mensaje;
int not_finished = true;
char buffer[MAX_VALUE_LENGTH];
pthread_mutex_t mutex_backend;

static int create_client()
{
	char	*host;

	host = getenv("IP_TUPLAS");
	
	clnt = clnt_create (host, LIST_SERVICE, LIST_SERVICE_V1, "udp");
	if (clnt == NULL)
	{
		clnt_pcreateerror (host);
		exit (1);
	}
	return (0);
}

// Función para manejar las solicitudes de los clientes
void treat_request(void *sc_request)
{
	pthread_mutex_lock(&mutex_mensaje);
	int sc = *((int *) sc_request); // Descriptor de socket del cliente
	not_finished = false;
	pthread_cond_signal(&cond_mensaje);
	pthread_mutex_unlock(&mutex_mensaje);
	enum clnt_stat retval_1;
	int result_1;
	char *print_1_username;
	char *print_1_operation;
	char *print_1_date;
	char *print_1_file;
	int error;
	char *description;
	char *port;


	// Recepción de la operación solicitada desde el cliente
	if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
	{
		perror("Error\n");
		close(sc);
		pthread_exit(0);
	}

	// OPERACIONES
	if (strcmp(buffer, "REGISTER") == 0)
	{
		sprintf(print_1_operation,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// DATE
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error2\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(print_1_date,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// USERNAME
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error3\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(print_1_username,"%s", buffer);
		
		// LLamar al backend para meter en la lista, luego memset para vaciar
		pthread_mutex_lock(&mutex_backend);
		error = register_user(buffer)
		pthread_mutex_unlock(&mutex_backend);
		memset(buffer, 0, MAX_VALUE_LENGTH);

		// LLamar al rpc
		if (create_client() == -1)
		{
			printf("Error initialiting rpc\n");
			return (-1);
		}
		retval_1 = print_1(print_1_username, print_1_operation, print_1_date, NULL, &result_1, clnt);
		if (retval_1 != RPC_SUCCESS) {
			clnt_perror (clnt, "call failed");
		}
		clnt_destroy (clnt);

		// Enviar el código de error de vuelta al cliente en formato red
		error = htonl(error);
		if (sendMessage(sc, (char *) &error, sizeof(int)) < 0)
		{
			printf("Error enviando al socket\n");
		}
	}
	else if (strcmp(buffer, "UNREGISTER") == 0)
	{
		sprintf(print_1_operation,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// DATE
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error2\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(print_1_date,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// USERNAME
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error3\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(print_1_username,"%s", buffer);

		// LLamar al backend para meter en la lista, luego memset para vaciar
		pthread_mutex_lock(&mutex_backend);
		error = unregister_user(buffer)
		pthread_mutex_unlock(&mutex_backend);
		memset(buffer, 0, MAX_VALUE_LENGTH);

		// LLamar al rpc
		if (create_client() == -1)
		{
			printf("Error initialiting rpc\n");
			return (-1);
		}
		retval_1 = print_1(print_1_username, print_1_operation, print_1_date, NULL, &result_1, clnt);
		if (retval_1 != RPC_SUCCESS) {
			clnt_perror (clnt, "call failed");
		}
		clnt_destroy (clnt);

		// Enviar el código de error de vuelta al cliente en formato red
		error = htonl(error);
		if (sendMessage(sc, (char *) &error, sizeof(int)) < 0)
		{
			printf("Error enviando al socket\n");
		}
	}
	else if (strcmp(buffer, "CONNECT") == 0)
	{
		sprintf(print_1_operation,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// DATE
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error2\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(print_1_date,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// USERNAME
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error3\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(print_1_username,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// PORT
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error3\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(port,"%s", buffer);

		// LLamar al backend para meter en la lista, luego memset para vaciar
		pthread_mutex_lock(&mutex_backend);
		error = connect_user(sc, port, print_1_username);
		pthread_mutex_unlock(&mutex_backend);
		memset(buffer, 0, MAX_VALUE_LENGTH);

		// LLamar al rpc
		if (create_client() == -1)
		{
			printf("Error initialiting rpc\n");
			return (-1);
		}
		retval_1 = print_1(print_1_username, print_1_operation, print_1_date, NULL, &result_1, clnt);
		if (retval_1 != RPC_SUCCESS) {
			clnt_perror (clnt, "call failed");
		}
		clnt_destroy (clnt);

		// Enviar el código de error de vuelta al cliente en formato red
		error = htonl(error);
		if (sendMessage(sc, (char *) &error, sizeof(int)) < 0)
		{
			printf("Error enviando al socket\n");
		}
	}
	else if (strcmp(buffer, "PUBLISH") == 0)
	{
		sprintf(print_1_operation,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// DATE
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error2\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(print_1_date,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// USERNAME
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error3\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(print_1_username,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// FILENAME
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error3\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(print_1_file,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// DESCRIPTION
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error3\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(description,"%s", buffer);

		// LLamar al backend para meter en la lista, luego memset para vaciar
		pthread_mutex_lock(&mutex_backend);
		error = register_file_from_user(print_1_username, print_1_file, description);
		pthread_mutex_unlock(&mutex_backend);
		memset(buffer, 0, MAX_VALUE_LENGTH);

		// LLamar al rpc
		if (create_client() == -1)
		{
			printf("Error initialiting rpc\n");
			return (-1);
		}
		retval_1 = print_1(print_1_username, print_1_operation, print_1_date, print_1_file, &result_1, clnt);
		if (retval_1 != RPC_SUCCESS) {
			clnt_perror (clnt, "call failed");
		}
		clnt_destroy (clnt);

		// Enviar el código de error de vuelta al cliente en formato red
		error = htonl(error);
		if (sendMessage(sc, (char *) &error, sizeof(int)) < 0)
		{
			printf("Error enviando al socket\n");
		}
	}
	else if (strcmp(buffer, "DELETE") == 0)
	{
		sprintf(print_1_operation,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// DATE
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error2\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(print_1_date,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// USERNAME
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error3\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(print_1_username,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// FILENAME
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error3\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(print_1_file,"%s", buffer);

		// LLamar al backend para meter en la lista, luego memset para vaciar
		pthread_mutex_lock(&mutex_backend);
		error = unregister_file_from_user(print_1_username, print_1_file);
		pthread_mutex_unlock(&mutex_backend);
		memset(buffer, 0, MAX_VALUE_LENGTH);

		// LLamar al rpc
		if (create_client() == -1)
		{
			printf("Error initialiting rpc\n");
			return (-1);
		}
		retval_1 = print_1(print_1_username, print_1_operation, print_1_date, print_1_file, &result_1, clnt);
		if (retval_1 != RPC_SUCCESS) {
			clnt_perror (clnt, "call failed");
		}
		clnt_destroy (clnt);

		// Enviar el código de error de vuelta al cliente en formato red
		error = htonl(error);
		if (sendMessage(sc, (char *) &error, sizeof(int)) < 0)
		{
			printf("Error enviando al socket\n");
		}
	}
	else if (strcmp(buffer, "DISCONNECT") == 0)
	{
		sprintf(print_1_operation,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// DATE
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error2\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(print_1_date,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// USERNAME
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error3\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(print_1_username,"%s", buffer);

		// LLamar al backend para meter en la lista, luego memset para vaciar
		pthread_mutex_lock(&mutex_backend);
		error = disconnect_user(print_1_username);
		pthread_mutex_unlock(&mutex_backend);
		memset(buffer, 0, MAX_VALUE_LENGTH);

		// LLamar al rpc
		if (create_client() == -1)
		{
			printf("Error initialiting rpc\n");
			return (-1);
		}
		retval_1 = print_1(print_1_username, print_1_operation, print_1_date, NULL, &result_1, clnt);
		if (retval_1 != RPC_SUCCESS) {
			clnt_perror (clnt, "call failed");
		}
		clnt_destroy (clnt);

		// Enviar el código de error de vuelta al cliente en formato red
		error = htonl(error);
		if (sendMessage(sc, (char *) &error, sizeof(int)) < 0)
		{
			printf("Error enviando al socket\n");
		}
	}
	else if (strcmp(buffer, "LIST_USERS") == 0)
	{
		t_response_user *respuesta_user = (t_response_user *) malloc(sizeof(t_response_user));
		respuesta_user->next = NULL;
		int n_user;
		sprintf(print_1_operation,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// DATE
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error2\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(print_1_date,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// USERNAME
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error3\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(print_1_username,"%s", buffer);

		// LLamar al backend para meter en la lista, luego memset para vaciar
		pthread_mutex_lock(&mutex_backend);
		error = list_users(print_1_username, respuesta_user, &n_user);
		pthread_mutex_unlock(&mutex_backend);
		memset(buffer, 0, MAX_VALUE_LENGTH);

		// LLamar al rpc
		if (create_client() == -1)
		{
			printf("Error initialiting rpc\n");
			return (-1);
		}
		retval_1 = print_1(print_1_username, print_1_operation, print_1_date, NULL, &result_1, clnt);
		if (retval_1 != RPC_SUCCESS) {
			clnt_perror (clnt, "call failed");
		}
		clnt_destroy (clnt);

		// Enviar el código de error de vuelta al cliente en formato red
		error = htonl(error);
		if (sendMessage(sc, (char *) &error, sizeof(int)) < 0)
		{
			printf("Error enviando al socket\n");
		}

		// Enviar el número de usuarios conectados al cliente
		int n_users_network = htonl(n_user);
		if (sendMessage(sc, (char *) &n_users_network, sizeof(int)) < 0)
		{
			perror("Error enviando el número de usuarios");
			close(sc);
			pthread_exit(0);
		}

		// Enviar la información de cada usuario
		t_response_user *current = respuesta_user;
		while (current != NULL)
		{
			// Enviar el nombre de usuario
			if (sendMessage(sc, current->username, strlen(current->username) + 1) < 0)
			{
				perror("Error enviando el nombre del usuario");
				close(sc);
				pthread_exit(0);
			}

			// Enviar la dirección IP del usuario
			if (sendMessage(sc, current->ip, strlen(current->ip) + 1) < 0)
			{
				perror("Error enviando la dirección IP");
				close(sc);
				pthread_exit(0);
			}

			// Enviar el puerto del usuario
			if (sendMessage(sc, current->port, strlen(current->port) + 1) < 0)
			{
				perror("Error enviando el puerto");
				close(sc);
				pthread_exit(0);
			}

			current = current->next;
		}
		t_response_user *temp;
		current = respuesta_user;
		while (current != NULL) {
			temp = current;
			current = current->next;
			free(temp);
		}
	}
	else if (strcmp(buffer, "LIST_CONTENT") == 0)
	{
		t_response_list *respuesta_list = (t_response_list *) malloc(sizeof(t_response_list));
		char *username2;
		sprintf(print_1_operation,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// DATE
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error2\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(print_1_date,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// USERNAME
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error3\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(print_1_username,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// USERNAME2
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error3\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(username2,"%s", buffer);

		// LLamar al backend para meter en la lista, luego memset para vaciar
		pthread_mutex_lock(&mutex_backend);
		error = list_content(print_1_username, respuesta_list, username2);
		pthread_mutex_unlock(&mutex_backend);
		memset(buffer, 0, MAX_VALUE_LENGTH);

		// LLamar al rpc
		if (create_client() == -1)
		{
			printf("Error initialiting rpc\n");
			return (-1);
		}
		retval_1 = print_1(print_1_username, print_1_operation, print_1_date, NULL, &result_1, clnt);
		if (retval_1 != RPC_SUCCESS) {
			clnt_perror (clnt, "call failed");
		}
		clnt_destroy (clnt);

		// Enviar el código de error de vuelta al cliente en formato red
		error = htonl(error);
		if (sendMessage(sc, (char *) &error, sizeof(int)) < 0)
		{
			printf("Error enviando al socket\n");
		}
	}
	else if (strcmp(buffer, "LIST_CONTENT") == 0)
	{
		// Asegúrate de asignar memoria para respuesta_list
		t_response_list *respuesta_list = (t_response_list *) malloc(sizeof(t_response_list));
		respuesta_list->next = NULL;
		char *username2;
		sprintf(print_1_operation,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// DATE
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error2\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(print_1_date,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// USERNAME
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error3\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(print_1_username,"%s", buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		// USERNAME2
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error3\n");
			close(sc);
			pthread_exit(0);
		}
		sprintf(username2,"%s", buffer);

		// LLamar al backend para meter en la lista, luego memset para vaciar
		pthread_mutex_lock(&mutex_backend);
		int error = list_content(print_1_username, respuesta_list, username2);
		pthread_mutex_unlock(&mutex_backend);

		if (create_client() == -1)
		{
			printf("Error initialiting rpc\n");
			return (-1);
		}
		retval_1 = print_1(print_1_username, print_1_operation, print_1_date, NULL, &result_1, clnt);
		if (retval_1 != RPC_SUCCESS) {
			clnt_perror (clnt, "call failed");
		}
		clnt_destroy (clnt);

		// Enviar el código de error de vuelta al cliente en formato red
		error = htonl(error);
		if (sendMessage(sc, (char *) &error, sizeof(int)) < 0)
		{
			printf("Error enviando al socket\n");
		}

		// Ahora que tienes el listado, envía el número total de archivos al cliente
		int file_count = 0;
		t_response_list *current = respuesta_list;
		while (current != NULL) {
			file_count++;
			current = current->next;
		}

		int network_file_count = htonl(file_count);
		if (sendMessage(sc, (char *) &network_file_count, sizeof(int)) < 0) {
			perror("Error enviando el conteo de archivos");
			free(respuesta_list);
			close(sc);
			pthread_exit(0);
		}

		// Luego envía la información de cada archivo
		current = respuesta_list;
		while (current != NULL) {
			if (sendMessage(sc, current->filename, strlen(current->filename) + 1) < 0) {
				perror("Error enviando el nombre del archivo");
				break;
			}

			if (sendMessage(sc, current->description, strlen(current->description) + 1) < 0) {
				perror("Error enviando la descripción del archivo");
				break;
			}

			current = current->next;
		}

		// Limpia la memoria al final
		t_response_list *temp;
		current = respuesta_list;
		while (current != NULL) {
			temp = current;
			current = current->next;
			free(temp);
		}
	}
	else
	{
		printf("Ha habido un error\n");
		printf("%s\n", buffer);
	}
	// Acabar 
	close(sc);
	pthread_exit(0);
}

int main(int argc, char *argv[])
	{
	struct sockaddr_in server_addr,  client_addr;
	socklen_t size;
	pthread_attr_t t_attr;
	pthread_t thid;
	int sc;
	int val = 1;

	if (argc != 4)
	{
		printf("No tengo claro el numero de argumenos\n");
		return (-1);
	}
	pthread_mutex_init(&mutex_mensaje, NULL);
	pthread_mutex_init(&mutex_backend, NULL);
	pthread_cond_init(&cond_mensaje, NULL);
	pthread_attr_init(&t_attr);
	pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

	if ((sd = socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		printf ("SERVER: Error en el socket\n");
		return (-1);
	}
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));

	bzero((char *)&server_addr, sizeof(server_addr));//ponemos a cero server_addr
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(atoi(argv[1]));

	//enlaza el socket con la dirección IP y el número del puerto del servidor
	if (bind(sd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("Error en Bind\n");
		return (-1);
	}
	// Obtener información del servidor después de bind para conocer la dirección y el puerto
    struct sockaddr_in local_addr;
    socklen_t local_size = sizeof(local_addr);
	if (getsockname(sd, (struct sockaddr *)&local_addr, &local_size) == 0) {
        printf("Servidor IP local: %s, Puerto: %d\n",
               inet_ntoa(local_addr.sin_addr),
               ntohs(local_addr.sin_port));
    } else {
        printf("Error al obtener la IP local\n");
    }
	//el servidor se quede esperando una conexión entrante
	if (listen(sd, SOMAXCONN) < 0) {
		printf("Error en Listen\n");
		return (-1);
	}

	size = sizeof(client_addr);
	/*
	// FInd local ip
	struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1) {// obtener las direcciones de red de todas las interfaces
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }
	char *ip_server = NULL;
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (strcmp(ifa->ifa_name, "eth0") == 0 && ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *sa = (struct sockaddr_in *) ifa->ifa_addr;
            ip_server = inet_ntoa(sa->sin_addr);
            break;
        }
    }

    freeifaddrs(ifaddr); // liberar la memoria asignada a la lista de direcciones
    if (ip_server == NULL) { // si no se encuentra la direccion de eth0
        fprintf(stderr, "Error: no se encontr贸 la direcci贸n de eth0\n");
        exit(EXIT_FAILURE);
    }*/
    printf("s> init server %s: %s\n", inet_ntoa(local_addr.sin_addr), argv[2]);


	// Bucle principal para aceptar conexiones y manejarlas en hilos separados
	while(1)
	{
		printf("s>\n");
		// Aceptar la conexión entrante
		sc = accept(sd, (struct sockaddr *)&client_addr, (socklen_t *)&size);
		if (sc == -1) {
			printf("Error en accept\n");
			return (-1);
		}
		// Crear un hilo para manejar la solicitud del cliente
		if (pthread_create(&thid, &t_attr, (void *)treat_request, (void *)&sc)== 0)
		{
			pthread_mutex_lock(&mutex_mensaje);
			while (not_finished)
				pthread_cond_wait(&cond_mensaje, &mutex_mensaje);
			not_finished = true;
			pthread_mutex_unlock(&mutex_mensaje);
		} 
	}
    if (pthread_mutex_destroy(&mutex_mensaje) != 0)
	{ 
        perror("Error en el destroy mutex\n");
        close (sc);
    }
	if (pthread_mutex_destroy(&mutex_backend) != 0)
	{ 
        perror("Error en el destroy mutex\n");
        close (sc);
    }
    close(sd); 
	return (0);
}