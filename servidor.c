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

	#define MAX_VALUE_LENGTH 256

int sd; //descriptor del socket global
pthread_mutex_t mutex_mensaje;
pthread_cond_t cond_mensaje;
int not_finished = true;
char buffer[MAX_VALUE_LENGTH];

// Función para manejar las solicitudes de los clientes
void treat_request(void *sc_request)
{
	pthread_mutex_lock(&mutex_mensaje);
	int sc = *((int *) sc_request); // Descriptor de socket del cliente
	not_finished = false;
	pthread_cond_signal(&cond_mensaje);
	pthread_mutex_unlock(&mutex_mensaje);


	int op;
	int key;
	char v1[MAX_VALUE_LENGTH];
	int N;
	double	v2[20];
	int error;

	// Recepción de la operación solicitada desde el cliente
	if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
	{//recibe el username
		perror("Error\n");
		close(sc);
		pthread_exit(0);
	}

	// OPERACIONES
	if (strcmp(buffer, "REGISTER") == 0)
	{
		memset(buffer, 0, MAX_VALUE_LENGTH);
		if(readLine(sc, buffer, MAX_VALUE_LENGTH) < 0)
		{
			perror("Error2\n");
			close(sc);
			pthread_exit(0);
		}
		error = resgister(buffer);
		if (error == 0)
		{

		}
		else
		{

		}

	}
	// Enviar el código de error de vuelta al cliente en formato red
	error = htonl(error);
	if (sendMessage(sc, (char *) &error, sizeof(int)) < 0)
    {
        printf("Error enviando al socket\n");
    }
	close(sc);
	pthread_exit(0);
	/*
	switch (op)
	{
		case 0: // Reseteo de la lista
			error = init();
			break;
		case 1: // Establecer un valor
			if (recvMessage(sc, (char *) &key, sizeof(int)) < 0)
			{
				printf("Error recibiendo del socket\n");
			}
			key = ntohl(key);
			
			readLine(sc, v1, MAX_VALUE_LENGTH);

			if (recvMessage(sc, (char *) &N, sizeof(int)) < 0)
			{
				printf("Error recibiendo del socket\n");
			}
			N = ntohl(N);

			for (int i = 0; i < N; i++) {
				char v2_str[20];
				readLine(sc, v2_str, 20);
				v2[i] = strtod(v2_str, NULL);
			}

			error = set_value(key, v1, N, v2);
			break;
		case 2: // Obtener un valor

			if (recvMessage(sc, (char *) &key, sizeof(int)) < 0)
			{
				printf("Error recibiendo del socket\n");
			}
			key = ntohl(key);

			error = get_value(key, v1, &N, v2);

			if (sendMessage(sc, v1, strlen(v1) + 1) < 0)
			{
				printf("Error recibiendo del socket\n");
			}

			int N_network = htonl(N);
			if (sendMessage(sc, (char *)&N_network, sizeof(int)) < 0)
			{
				printf("Error enviando al socket\n");
			}

			for (int i = 0; i < N; i++)
			{
				char v2_str[20]; 
				snprintf(v2_str, sizeof(v2_str), "%lf", v2[i]);
				if (sendMessage(sc, v2_str, strlen(v2_str) + 1) < 0)
				{
					printf("Error enviando al socket\n");
				}
			}

			if (sendMessage(sc, (char *) &error, sizeof(int)) < 0)
			{
				printf("Error enviando al socket\n");
			}
			break;
		case 3: // Modificar un valor
			if (recvMessage(sc, (char *) &key, sizeof(int)) < 0)
			{
				printf("Error recibiendo del socket\n");
			}
			key = ntohl(key);

			readLine(sc, v1, MAX_VALUE_LENGTH);

			if (recvMessage(sc, (char *) &N, sizeof(int)) < 0)
			{
				printf("Error recibiendo del socket\n");
			}
			N = ntohl(N);

			for (int i = 0; i < N; i++) {
				char v2_str[20];
				readLine(sc, v2_str, 20);
				v2[i] = strtod(v2_str, NULL);
			}

			error = modify_value(key, v1, N, v2);
			break;
		case 4: // Eliminar una clave
			if (recvMessage(sc, (char *) &key, sizeof(int)) < 0)
			{
				printf("Error recibiendo del socket\n");
			}
			key = ntohl(key);

			error = delete_key(key);
			break;
		case 5: // Verificar si existe una clave
			if (recvMessage(sc, (char *) &key, sizeof(int)) < 0)
			{
				printf("Error recibiendo del socket\n");
			}
			key = ntohl(key);

			error = exist(key);
			break;
	}
	// Enviar el código de error de vuelta al cliente en formato red
	error = htonl(error);
	if (sendMessage(sc, (char *) &error, sizeof(int)) < 0)
    {
        printf("Error enviando al socket\n");
    }
	// Cerrar el socket del cliente
	close(sc);
	// Salir del hilo
	pthread_exit(0);
	*/
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

	//el servidor se quede esperando una conexión entrante
	if (listen(sd, SOMAXCONN) < 0) {
		printf("Error en Listen\n");
		return (-1);
	}

	size = sizeof(client_addr);

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
    }
    printf("s> init server %s: %s\n", ip_server, argv[2]);


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
		printf("conexión aceptada de IP: %s   Puerto: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
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
    };
    close(sd); 
	return (0);
}