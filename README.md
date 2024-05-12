## Forma de compilación

La compilación de nuestro proyecto es sencilla, en caso del código del servidor se realiza mediante make. Explicado a continuación:
•	Variables: Se establecen variables para el compilador (CC) y los flags de compilación (CFLAGS).
•	SERVIDOR y CLIENTE DE RPC: Se definen las fuentes y objetos del servidor y el cliente de RPC.
•	SERVIDOR RPC: Se definen las fuentes y objetos del servidor RPC.
•	all: Es la regla principal que compila tanto el servidor como el servidor RPC.
•	Compilación de objetos del cliente de RPC: Se define una regla para compilar los objetos del cliente de RPC.
•	Compilación del servidor: Se define una regla para compilar el servidor, utilizando los objetos del servidor y del cliente de RPC.
•	Compilación de objetos del servidor RPC: Se define una regla para compilar los objetos del servidor RPC.
•	Compilación del servidor RPC: Se define una regla para compilar el servidor RPC, utilizando los objetos del servidor RPC.
•	runs y runrpc: Son comandos para ejecutar el servidor y el servidor RPC, respectivamente.
•	clean y fclean: Son reglas para limpiar los archivos generados durante la compilación, con fclean eliminando también los binarios.
•	re: Es una regla para reconstruir todo el proyecto desde cero.
Todos los archivos .c tienen flags comunes como -Wall -Werror para garantizar un código limpio. Además, se añaden las flags -lpthread o -lrt respectivamente.


## Ejecución

La ejecución del programa se divide en cuatro partes. Primero, se abren cuatro terminales. 

En la primera terminal, se ejecuta el servidor RPC con el comando "./servidor_rpc". Aquí se imprimirán todas las peticiones que lleguen al servidor.
En la siguiente terminal, se ejecuta el servicio web, por ejemplo, "python3 ws-time-service.py". Los mensajes impresos en esta terminal son irrelevantes para el funcionamiento del programa.
Luego, se abre una tercera terminal y se inicia el servidor, especificando el puerto deseado, por ejemplo, "-p 4242". La sintaxis sería "./servidor -p 4242". En esta terminal se imprimirá la dirección IP del servidor, la cual se necesitará para la configuración del cliente.
Finalmente, en la cuarta terminal, se ejecuta el cliente con el comando "python3 client.py -s 172.22.219.110 -p 4242" para este ejemplo. Una vez que todo esté configurado en el cliente, se pueden comenzar a enviar comandos.
