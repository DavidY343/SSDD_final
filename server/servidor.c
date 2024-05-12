#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ifaddrs.h>
#include "lines.h"
#include "../server_rpc/mensaje.h"
#include "list.h"
#define MAX_VALUE_LENGTH 256

CLIENT *clnt;
int sd;
pthread_mutex_t mutex_mensaje;
pthread_cond_t cond_mensaje;
int not_finished = true;
char buffer[MAX_VALUE_LENGTH];
char *ip_rpc;

static int create_client()
{
	char	*host;

	host = ip_rpc;
	
	clnt = clnt_create (host, LIST_SERVICE, LIST_SERVICE_V1, "udp");
	if (clnt == NULL)
	{
		clnt_pcreateerror (host);
		exit (1);
	}
	return (0);
}

static void rpc_llamada(char *print_1_username, char *print_1_operation, char *print_1_date, char *print_1_file)
{
	enum clnt_stat retval_1;
	int result_1;

	if (create_client() == -1)
	{
		printf("Error initialiting rpc\n");
		pthread_exit(0);
	}
	retval_1 = print_1(print_1_username, print_1_operation, print_1_date, print_1_file, &result_1, clnt);
	if (retval_1 != RPC_SUCCESS)
	{
		clnt_perror (clnt, "call failed");
	}
	clnt_destroy (clnt);
}

static void send_error(int sc, int error)
{
	char error_str[2];
	error_str[0] = '0' + error;
	error_str[1] = '\0';
	if (sendMessage(sc, error_str, 2) < 0)
	{
		printf("Error enviando al socket\n");
	}
}

// Función para manejar las solicitudes de los clientes
void treat_request(void *sc_request)
{
	pthread_mutex_lock(&mutex_mensaje);
	int sc = *((int *) sc_request); // Descriptor de socket del cliente
	not_finished = false;
	pthread_cond_signal(&cond_mensaje);
	pthread_mutex_unlock(&mutex_mensaje);

	int error;
    char *print_1_username = (char *)malloc(MAX_VALUE_LENGTH * sizeof(char));
    char *print_1_operation = (char *)malloc(MAX_VALUE_LENGTH * sizeof(char));
    char *print_1_date = (char *)malloc(MAX_VALUE_LENGTH * sizeof(char));
    char *print_1_file = (char *)malloc(MAX_VALUE_LENGTH * sizeof(char));
    char *description = (char *)malloc(MAX_VALUE_LENGTH * sizeof(char));
    char *port = (char *)malloc(MAX_VALUE_LENGTH * sizeof(char));
	char *username2 = (char *)malloc(MAX_VALUE_LENGTH * sizeof(char));
	if (!print_1_username || !print_1_operation || !print_1_date || !print_1_file || !description || !port || !username2)
	{
        perror("Error asignando memoria");
        pthread_exit(0);
    }

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
		error = register_user(buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);

		// LLamar al rpc
		rpc_llamada(print_1_username, print_1_operation, print_1_date, "");

		// Enviar el código de error de vuelta al cliente en formato red
		send_error(sc, error);
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
		error = unregister_user(buffer);
		memset(buffer, 0, MAX_VALUE_LENGTH);
		
		// LLamar al rpc
		rpc_llamada(print_1_username, print_1_operation, print_1_date, "");

		// Enviar el código de error de vuelta al cliente en formato red
		send_error(sc, error);

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
		error = connect_user(sc, port, print_1_username);
		memset(buffer, 0, MAX_VALUE_LENGTH);

		// LLamar al rpc
		rpc_llamada(print_1_username, print_1_operation, print_1_date, "");

		// Enviar el código de error de vuelta al cliente en formato red
		send_error(sc, error);
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
		error = register_file_from_user(print_1_username, print_1_file, description);
		memset(buffer, 0, MAX_VALUE_LENGTH);

		// LLamar al rpc
		rpc_llamada(print_1_username, print_1_operation, print_1_date, print_1_file);

		// Enviar el código de error de vuelta al cliente en formato red
		send_error(sc, error);
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
		error = unregister_file_from_user(print_1_username, print_1_file);
		memset(buffer, 0, MAX_VALUE_LENGTH);

		// LLamar al rpc
		rpc_llamada(print_1_username, print_1_operation, print_1_date, print_1_file);

		// Enviar el código de error de vuelta al cliente en formato red
		send_error(sc, error);
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
		error = disconnect_user(print_1_username);
		memset(buffer, 0, MAX_VALUE_LENGTH);

		// LLamar al rpc
		rpc_llamada(print_1_username, print_1_operation, print_1_date, "");

		// Enviar el código de error de vuelta al cliente en formato red
		send_error(sc, error);
	}
else if (strcmp(buffer, "LIST_USERS") == 0)
	{
		t_response_user *respuesta_user = (t_response_user *) malloc(sizeof(t_response_user));
		int n_user;
		char *n_user_str;
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
		error = list_users(print_1_username, &respuesta_user, &n_user);
		memset(buffer, 0, MAX_VALUE_LENGTH);

		// LLamar al rpc
		rpc_llamada(print_1_username, print_1_operation, print_1_date, "");

		// Enviar el código de error de vuelta al cliente en formato red
		send_error(sc, error);
		if (error != 0)
		{
			close(sc);
			pthread_exit(0);
		}
		int num_digits = snprintf(NULL, 0, "%d", n_user) + 1;
		n_user_str = (char *)malloc(sizeof(char) * (num_digits));
		// Enviar el número de usuarios conectados al cliente
		snprintf(n_user_str, num_digits, "%d", n_user);
		if (sendMessage(sc, n_user_str, num_digits) < 0)
		{
			perror("Error enviando el número de usuarios");
			close(sc);
			pthread_exit(0);
		}
		free(n_user_str);

		// Enviar la información de cada usuario
		t_response_user *send = respuesta_user;
		int i = 0;
		while (i < n_user)
		{
			// Enviar el nombre de usuario
			if (sendMessage(sc, send->username, strlen(send->username) + 1) < 0)
			{
				perror("Error enviando el nombre del usuario");
				close(sc);
				pthread_exit(0);
			}
			// Enviar la dirección IP del usuario
			if (sendMessage(sc, send->ip, strlen(send->ip) + 1) < 0)
			{
				perror("Error enviando la dirección IP");
				close(sc);
				pthread_exit(0);
			}
			// Enviar el puerto del usuario
			if (sendMessage(sc, send->port, strlen(send->port) + 1) < 0)
			{
				perror("Error enviando el puerto");
				close(sc);
				pthread_exit(0);
			}
			if (send->next != NULL)
				send = send->next;

			i++;
		}

	}
	else if (strcmp(buffer, "LIST_CONTENT") == 0)
	{
		// Asegúrate de asignar memoria para respuesta_list
		t_response_list *respuesta_list = (t_response_list *) malloc(sizeof(t_response_list));
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
		int error = list_content(print_1_username, &respuesta_list, username2);

		// LLamar al rpc
		rpc_llamada(print_1_username, print_1_operation, print_1_date, "");

		// Enviar el código de error de vuelta al cliente en formato red
		send_error(sc, error);
		if (error != 0)
		{
			close(sc);
			pthread_exit(0);
		}
		// Ahora que tienes el listado, envía el número total de archivos al cliente
		int file_count = 0;
		char *file_count_str;
		t_response_list *count_ptr = respuesta_list;
		while (count_ptr != NULL) {
			if (count_ptr->filename[0] != '\0')
			{
				file_count++;
			}
			 count_ptr = count_ptr->next;
		}
		int num_digits = snprintf(NULL, 0, "%d", file_count) + 1;
		file_count_str = (char *)malloc(sizeof(char) * (num_digits));
		// Enviar el número de usuarios conectados al cliente
		snprintf(file_count_str, (num_digits), "%d", file_count);
		if (sendMessage(sc, file_count_str, num_digits) < 0)
		{
			perror("Error enviando el número de usuarios");
			close(sc);
			pthread_exit(0);
		}
		free(file_count_str);

		// Luego envía la información de cada archivo
		t_response_list *send_ptr = respuesta_list;
		while (send_ptr != NULL)
		{
			if (sendMessage(sc, send_ptr->filename, strlen(send_ptr->filename) + 1) < 0)
			{
				perror("Error enviando el nombre del archivo");
				break;
			}
			if (sendMessage(sc, send_ptr->description, strlen(send_ptr->description) + 1) < 0)
			{
				perror("Error enviando la descripción del archivo");
				break;
			}

			send_ptr = send_ptr->next;
		}
	}
	// Acabar 
	free(print_1_username);
    free(print_1_operation);
    free(print_1_date);
    free(print_1_file);
    free(description);
    free(port);
	free(username2);
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

	if (argc != 3)
	{
		printf("No tengo claro el numero de argumenos\n");
		return (-1);
	}
	pthread_mutex_init(&mutex_mensaje, NULL);
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
	server_addr.sin_port = htons(atoi(argv[2]));

	//enlaza el socket con la dirección IP y el número del puerto del servidor
	if (bind(sd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("Error en Bind\n");
		return (-1);
	}

	//el servidor se quede esperando una conexión entrante
	if (listen(sd, SOMAXCONN) < 0)
	{
		printf("Error en Listen\n");
		return (-1);
	}

	size = sizeof(client_addr);

	//Find local IP
	struct ifaddrs *ifaddr, *ifa;
    int family;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) 
	{
        perror("Error al obtener las interfaces");
        exit(EXIT_FAILURE);
    }


    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) 
	{
        if (ifa->ifa_addr == NULL) continue;

        family = ifa->ifa_addr->sa_family;

        if (family == AF_INET) { // Direcciones IPv4
            if (getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST) == 0)
			{
				if (strcmp(ifa->ifa_name, "eth0") == 0) 
				{
					ip_rpc = host;
					printf("s> init server %s:%s\n", host, argv[2]);
					break;
				}
			}

        }
    }
    freeifaddrs(ifaddr);

	// Bucle principal para aceptar conexiones y manejarlas en hilos separados
	while(1)
	{
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
    close(sd); 
	return (0);
}