#ifndef _LISTA_H
#define _LISTA_H        1

#define MAX_VALUE_LENGTH	256
#include <stddef.h>

typedef struct filelist
{
	char filename[MAX_VALUE_LENGTH];
	char description[MAX_VALUE_LENGTH];
	struct filelist *next;
} filelist_t ;
typedef struct Node{ 
	char 	username[MAX_VALUE_LENGTH];
	char 	ip[MAX_VALUE_LENGTH];
	char	port[MAX_VALUE_LENGTH];
	struct filelist *list_files;
	struct 	Node *next; 
} node_t;


typedef node_t *List;


int init_list();
int destroy();
/**
 * @brief  Esta funcion se le pasara un usuario y se le asignara un nodo al usuario
 * a su vez comprobara si el usuario existe o no previamente
 * 
 * @param username nombre del usuario.
 * @return int La función devuelve 0 en caso de éxito y -1 en caso de error.
 * @retval 0 en caso de éxito.
 * @retval 1 en caso de que el uuario exista.
 * @retval 2 en cualquier otro caso.
 */
int register_user(char *username);
/**
 * @brief  Esta funcion se le pasara un usuario y se le borrara de la base de datos
 * independientemente de si tiene asignao los demas parametros
 * 
 * @param username nombre del usuario.
 * @return int La función devuelve 0 en caso de éxito y -1 en caso de error.
 * @retval 0 en caso de éxito.
 * @retval 1 en caso de que el uuario no exista.
 * @retval 2 en cualquier otro caso.
 */
int unregister_user(char *username);
/**
 * @brief A esta funcion se le pasa el socket del usuario de ahi se saca la ip
 * y se le asignan los valores de ip y puerto al usuario proporcionado. Se considera
 * que el usuario ya esta conectado si tiene ip y puerto asignado
 * 
 * @return int La función devuelve 0 en caso de éxito y -1 en caso de error.
 * @retval 0 en caso de éxito.
 * @retval 1 en caso de que el usuario no exista.
 * @retval 2 en caso de que el usuario ya este conectado.
 * @retval 3 en cualquier otro caso
 */
int connect_user(int socket, char *port, char *username);


#endif

