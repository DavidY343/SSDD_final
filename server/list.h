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
typedef struct Node
{ 
	char 	username[MAX_VALUE_LENGTH];
	char 	ip[MAX_VALUE_LENGTH];
	char	port[MAX_VALUE_LENGTH];
	struct filelist *list_files;
	struct 	Node *next; 
} node_t;


typedef node_t *List;


int init_list();
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
 *
 * @retval 0 en caso de éxito.
 * @retval 1 en caso de que el usuario no exista.
 * @retval 2 en caso de que el usuario ya este conectado.
 * @retval 3 en cualquier otro caso
 */
int connect_user(int socket, char *port, char *username);
/**
 * @brief A esta funcion se le pasa el nombre de usuario y se comprueba si esta conectado
 * que corresponde si (strlen(current->ip) > 0) y si esta conectado pues se le desconecta
 * para ello se vuelven a poner a \0 tanto su ip como su puerto
 * 
 * 
 * @retval 0 en caso de éxito.
 * @retval 1 en caso de que el usuario no exista.
 * @retval 2 en caso de que el usuario no este conectado.
 * @retval 3 en cualquier otro caso
 */
int disconnect_user(char *username);
/**
 * @brief A esta funcion se le pase el nombre de usuario y el fichero y la descripcion
 * que desea guardar. Ese fichero y ese descripcion se guarda con el usuario
 * 
 *
 * @retval 0 en caso de éxito.
 * @retval 1 en caso de que el usuario no exista.
 * @retval 2 en caso de que el usuario no este conectado.
 * @retval 3 en caso de que el fichero ya este publicado
 * @retval 4 en cualquier otro caso
 */
int register_file_from_user(char *username, char *file, char *description);
/**
 * @brief A esta funcion se le pase el nombre de usuario y el fichero que se desea
 * eliminar. 
 * 

 * @retval 0 en caso de éxito.
 * @retval 1 en caso de que el usuario no exista.
 * @retval 2 en caso de que el usuario no este conectado.
 * @retval 3 en caso de que el fichero no este publicado
 * @retval 4 en cualquier otro caso
 */
int unregister_file_from_user(char *username, char *file);

typedef struct s_response_user
{
	char 	username[MAX_VALUE_LENGTH];
	char	ip[MAX_VALUE_LENGTH];
	char	port[MAX_VALUE_LENGTH];
	struct s_response_user *next;
} t_response_user;

/**
 * @brief Un usuario pide que se le pase la lista entera de usuarios CONECTADOS
 *  para eso se guardaran todos los usuarios en una structura response. 
 * Pero para ellos el usuario debe estar registrado y estar conectado.
 * 
 * @param respuesta es la strucutra donde se devolvera la informacion
 * @param n_user dira cuantos usuarios hay conectados ahora mismo
 * @retval 0 en caso de éxito.
 * @retval 1 en caso de que el usuario no exista.
 * @retval 2 en caso de que el usuario no este conectado.
 * @retval 3 en cualquier otro caso
 */
int list_users(char *username, t_response_user **respuesta, int *n_user);

typedef struct s_response_list
{
	char filename[MAX_VALUE_LENGTH];
	char description[MAX_VALUE_LENGTH];
	struct s_response_list *next;
} t_response_list;
/**
 * @brief un usuario pide conocer los ficheros publicados por un determinado usuario. 
 * se devolvera la lista ficheros publicados por ese usuario. 
 * Pero para ello el usuario debe estar registrado y estar conectado.
 * 
 * @param respuesta es la strucutra donde se devolvera la informacion
 * @param username_2 es el nombre del usuario del que se le saca la informacion
 * @retval 0 en caso de éxito.
 * @retval 1 en caso de que el usuario no exista.
 * @retval 2 en caso de que el usuario que realizada la operacion no este conectado.
 * @retval 3 en caso de que el usuario cuyo contenido se quiere conocer no existe.
 * @retval 4 en cualquier otro caso.
 */
int list_content(char *username, t_response_list **respuesta, char *username_2);


#endif

