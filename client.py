from enum import Enum
import argparse
import socket
import threading
import zeep

class client :

    # ******************** TYPES *********************
    # *
    # * @brief Return codes for the protocol methods
    class RC(Enum) :
        OK = 0
        ERROR = 1
        USER_ERROR = 2
        OTHER_CASES = 3
        ANOTHER_CASES = 4

    # ****************** ATTRIBUTES ******************
    _server = None
    _port = -1
    _connected = False
    _user = None
    _archivos = {}
    _stop_flag = threading.Event()

    # ******************** METHODS *******************

    @staticmethod
    def get_time():
        wsdl_url = "http://localhost:8000/?wsdl"
        soap = zeep.Client(wsdl=wsdl_url) 
        return soap.service.get_time()

    @staticmethod
    def read_response(socket):
        response = ''
        while True:
            server_response = socket.recv(1)
            if (server_response == b'\0'):
                break
            response += server_response.decode()
        return response

    @staticmethod  
    def connect_socket(port = None, server = None):
        try:
            socketS = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            if port is None:
                port = client._port
            if server is None:
                server = client._server
            server_address = (server, port)
            socketS.connect(server_address)
            return socketS
        except Exception as e:
            print('Error al crear el socket: ', e)
            return None

    # Método para hacer las comprobaciones de las cadenas que no pueden superar los 256 bytes.
    # Se comprueba 255 para tener en cuenta el \0
    @staticmethod
    def excede_tamano(cadena):
        encoded_cadena = cadena.encode('utf-8')
        # Comprobar si la longitud de la cadena codificada excede 255 bytes
        return len(encoded_cadena) > 255

    @staticmethod
    def register(user) :
        if client.excede_tamano(user):
            print("El nombre de usuario excede el tamaño máximo permitido")
            print("REGISTER FAIL")
            return client.RC.USER_ERROR
        socketS = client.connect_socket()
        if socketS is None:
            print("Error al crear el socket. No se pudo establecer la conexión.")
            print("REGISTER FAIL")
            return client.RC.USER_ERROR
        try:
            message = (
                b'REGISTER\0' + 
                client.get_time().encode('utf-8') + b'\0' +
                user.encode('utf-8') + b'\0'
            )
            socketS.sendall(message)
            response = client.read_response(socketS)
            #Gestion del resultado
            if response == '0':
                print("REGISTER OK")
                return client.RC.OK
            elif response == '1':
                print("USERNAME IN USE")
                return client.RC.ERROR
            else:
                print("REGISTER FAIL")
                return client.RC.USER_ERROR
        except Exception as e:
            print('Error:', e)
            print("REGISTER FAIL")
            return client.RC.USER_ERROR

        finally:
            socketS.close()

   
    @staticmethod
    def unregister(user) :
        socketS = client.connect_socket()
        if socketS is None:
            print("Error al crear el socket. No se pudo establecer la conexión.")
            print("UNREGISTER FAIL")
            return client.RC.ERROR
        try:
            message = (
                b'UNREGISTER\0' + 
                client.get_time().encode('utf-8') + b'\0' +
                user.encode('utf-8') + b'\0'
            )
            socketS.sendall(message)
            response = client.read_response(socketS)
            #Gestion del resultado
            if response == '0':
                print("UNREGISTER OK")
                return client.RC.OK
            elif response == '1':
                print("USER DOES NOT EXIST")
                return client.RC.ERROR
            else:
                print("UNREGISTER FAIL")
                return client.RC.USER_ERROR
        except Exception as e:
            print('Error:', e)
            print("UNREGISTER FAIL")
            return client.RC.USER_ERROR

        finally:
            socketS.close()

    @staticmethod
    def listen_requests(server_socket):
        try:
            server_socket.listen(5)

            # Limpiar el evento
            client._stop_flag.clear()

            while not client._stop_flag.is_set():
                # Aceptar una conexión entrante
                client_socket, client_address = server_socket.accept()
                op = read_response(client_socket)
                if op == "GET_FILE":
                    file_name = read_response(client_socket)
                    if file_name in client._archivos:
                        desc = client._archivos[file_name]
                        message = (
                            b'0\0' +
                            desc.encode('utf-8') + b'\0'
                        )
                    else:
                        message = (
                            b'1\0'
                        )
                else:
                    message = (
                        b'2\0'
                    )
                client_socket.sendall(message)
                client_socket.close()
        except Exception as e:
            print('Error:', e)
        finally:
            server_socket.close()


    @staticmethod
    def connect(user) :
        socketS = client.connect_socket()
        if socketS is None:
            print("Error al crear el socket. No se pudo establecer la conexión.")
            print("CONNECT FAIL")
            return client.RC.OTHER_CASES
        try:
            # Abrir el socket
            server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_address = ('', 0)
            server_socket.bind(server_address)
            _, port = server_socket.getsockname()

            # Aviso al servidor de que me conecto
            message = (
                b'CONNECT\0' +
                client.get_time().encode('utf-8') + b'\0' +
                user.encode('utf-8') + b'\0' + 
                str(port).encode('utf-8') + b'\0'
            )
            socketS.sendall(message)
            response = client.read_response(socketS)

            #Gestion del resultado
            if response == '0':
                print("CONNECT OK")

                # Creacion del hilo
                server_thread = threading.Thread(target=client.listen_requests, args=(server_socket,))
                # Para que el hilo sea no joinable
                server_thread.daemon = True
                server_thread.start()

                client._connected = True
                client._user = user
                return client.RC.OK
            elif response == '1':
                print("CONNECT FAIL, USER DOES NOT EXIST")
                server_socket.close()
                return client.RC.ERROR
            elif response == '2':
                print("USER ALREADY CONNECTED")
                server_socket.close()
                return client.RC.USER_ERROR
            else:
                print("CONNECT FAIL")
                server_socket.close()
                return client.RC.OTHER_CASES
        except Exception as e:
            print('Error:', e)
            print("CONNECT FAIL")
            server_socket.close()
            return client.RC.OTHER_CASES

        finally:
            socketS.close()
    
    @staticmethod
    def  disconnect(user) :
        socketS = client.connect_socket()
        if socketS is None:
            print("Error al crear el socket. No se pudo establecer la conexión.")
            print("DISCONNECT FAIL")
            return client.RC.OTHER_CASES
        try:
            message = (
                b'DISCONNECT\0' + 
                client.get_time().encode('utf-8') + b'\0' +
                user.encode('utf-8') + b'\0'
            )
            socketS.sendall(message)
            response = client.read_response(socketS)
            client._connected = False
            client._user = None
            client._archivos = {}
            # Activamos el evento para que el hilo pare
            client._stop_flag.set()
            #Gestion del resultado
            if response == '0':
                print("DISCONNECT OK")
                return client.RC.OK
            elif response == '1':
                print("DISCONNECT FAIL / USER DOES NOT EXIST")
                return client.RC.ERROR
            elif response == '2':
                print("DISCONNECT FAIL / USER NOT CONNECTED")
                return client.RC.USER_ERROR
            else:
                print("DISCONNECT FAIL")
                return client.RC.OTHER_CASES
        except Exception as e:
            print('Error:', e)
            print("DISCONNECT FAIL")
            return client.RC.OTHER_CASES

        finally:
            socketS.close()

    @staticmethod
    def  publish(fileName,  description) :
        if client.excede_tamano(fileName):
            print("El nombre del archivo excede el tamaño máximo permitido")
            print("PUBLISH FAIL")
            return client.RC.ANOTHER_CASES
        socketS = client.connect_socket()
        if socketS is None:
            print("Error al crear el socket. No se pudo establecer la conexión.")
            print("PUBLISH FAIL")
            return client.RC.ANOTHER_CASES
        try:
            if not client._connected:
                print("PUBLISH FAIL, USER NOT CONNECTED")
                return client.RC.USER_ERROR
            message = (
                b'PUBLISH\0' + 
                client.get_time().encode('utf-8') + b'\0' +
                client._user.encode('utf-8') + b'\0' + 
                fileName.replace(" ", "").encode('utf-8') + b'\0' + 
                description.encode('utf-8') + b'\0'
            )
            socketS.sendall(message)
            response = client.read_response(socketS)
            if response == '0':
                print("PUBLISH OK")
                client._archivos[fileName] = description
                return client.RC.OK
            elif response == '1':
                print("PUBLISH FAIL, USER DOES NOT EXIST")
                return client.RC.ERROR
            elif response == '2':
                print("PUBLISH FAIL, USER NOT CONNECTED")
                return client.RC.USER_ERROR
            elif response == '3':
                print("PUBLISH FAIL, CONTENT ALREDAY PUBLISHED")
                return client.RC.OTHER_CASES
            else:
                print("PUBLISH FAIL")
                return client.RC.ANOTHER_CASES
        except Exception as e:
            print("PUBLISH FAIL")
            print('Error:', e)
            return client.RC.ANOTHER_CASES

        finally:
            socketS.close()

    @staticmethod
    def  delete(fileName) :
        if client.excede_tamano(fileName):
            print("El nombre del fichero excede el tamaño máximo permitido")
            print("DELETE FAIL")
            return client.RC.ANOTHER_CASES
        socketS = client.connect_socket()
        if socketS is None:
            print("Error al crear el socket. No se pudo establecer la conexión.")
            print("DELETE FAIL")
            return client.RC.ANOTHER_CASES
        try:
            if not client._connected:
                print("DELETE FAIL, USER NOT CONNECTED")
                return client.RC.USER_ERROR
            message = (
                b'DELETE\0' + 
                client.get_time().encode('utf-8') + b'\0' +
                client._user.encode('utf-8') + b'\0' + 
                fileName.replace(" ", "").encode('utf-8') + b'\0'
            )
            socketS.sendall(message)
            response = client.read_response(socketS)
            if response == '0':
                print("DELETE OK")
                if fileName in client._archivos:
                    del client._archivos[fileName]
                return client.RC.OK
            elif response == '1':
                print("DELETE FAIL, USER DOES NOT EXIST")
                return client.RC.ERROR
            elif response == '2':
                print("DELETE FAIL, USER NOT CONNECTED")
                return client.RC.USER_ERROR
            elif response == '3':
                print("DELETE FAIL, CONTENT NOT PUBLISHED")
                return client.RC.OTHER_CASES
            else:
                print("DELETE FAIL")
                return client.RC.ANOTHER_CASES
        except Exception as e:
            # El print del error quizás se puede quitar
            print('Error:', e)
            print("DELETE FAIL")
            return client.RC.ANOTHER_CASES

        finally:
            socketS.close()

    @staticmethod
    def  listusers() :
        socketS = client.connect_socket()
        if socketS is None:
            print("Error al crear el socket. No se pudo establecer la conexión.")
            print("LIST_USERS FAIL")
            return client.RC.OTHER_CASES
        try:
            if not client._connected:
                print("LIST_USERS FAIL, USER NOT CONNECTED")
                return client.RC.USER_ERROR
            message = (
                b'LIST_USERS\0' + 
                client.get_time().encode('utf-8') + b'\0' +
                client._user.encode('utf-8') + b'\0'
            )
            socketS.sendall(message)
            response = client.read_response(socketS)
            #Gestion del resultado
            if response == '0':
                # Recibir toda la lista de usuarios
                print("LIST_USERS OK")
                number_of_users = int(client.read_response(socketS))
                for i in range(number_of_users):
                    users_info = client.read_response(socketS)
                    print("\t" + users_info + "\t", end=" ")
                    users_info = client.read_response(socketS)
                    print(users_info + "\t", end=" ")
                    users_info = client.read_response(socketS)
                    print(users_info + "\n")
                return client.RC.OK
            elif response == '1':
                print("LIST_USERS FAIL, USER DOES NOT EXIST")
                return client.RC.ERROR
            elif response == '2':
                print("LIST_USERS FAIL, USER NOT CONNECTED")
                return client.RC.USER_ERROR
            else:
                print("LIST_USERS FAIL")
                return client.RC.OTHER_CASES
        except Exception as e:
            print('Error:', e)
            print("LIST_USERS FAIL")
            return client.RC.OTHER_CASES

        finally:
            socketS.close()

    @staticmethod
    def  listcontent(user) :
        socketS = client.connect_socket()
        if socketS is None:
            print("Error al crear el socket. No se pudo establecer la conexión.")
            print("LIST_CONTENT FAIL")
            return client.RC.ANOTHER_CASES
        try:
            if not client._connected:
                print("LIST_CONTENT FAIL, USER NOT CONNECTED")
                return client.RC.USER_ERROR
            message = (
                b'LIST_CONTENT\0' + 
                client.get_time().encode('utf-8') + b'\0' +
                client._user.encode('utf-8') + b'\0' +
                user.encode('utf-8') + b'\0'
            )
            socketS.sendall(message)
            response = client.read_response(socketS)
            #Gestion del resultado
            if response == '0':
                print("LIST_CONTENT OK")
                # Recibir el contenido del usuario
                number_of_files = int(client.read_response(socketS))
                for i in range(number_of_files):
                    file_info = client.read_response(socketS)
                    print("\t" + file_info + "\t", end=" ")
                    file_info = client.read_response(socketS)
                    print(file_info)
                return client.RC.OK
            elif response == '1':
                print("LIST_CONTENT FAIL, USER DOES NOT EXIST")
                return client.RC.ERROR
            elif response == '2':
                print("LIST_CONTENT FAIL, USER NOT CONNECTED")
                return client.RC.USER_ERROR
            elif response == '3':
                print("LIST_CONTENT FAIL, REMOTE USER DOES NOT EXIST")
                return client.RC.OTHER_CASES
            else:
                print("LIST_CONTENT FAIL")
                return client.RC.ANOTHER_CASES
        except Exception as e:
            print('Error:', e)
            print("LIST_CONTENT FAIL")
            return client.RC.ANOTHER_CASES

        finally:
            socketS.close()

    # Metodo que solo sirve para procesar la respuesta del servidor cuando pides usuario
    @staticmethod
    def get_data_by_user(socket, user):
        number_of_users = int(client.read_response(socket))
        for i in range(number_of_users):
            username = client.read_response(socket)
            ip = client.read_response(socket)
            port = client.read_response(socket)

            if username == user:
                # Se encontró el usuario, guardando la información de servidor y puerto
                server = ip
                port = int(port)  # Asegurándonos de que el puerto sea un entero
                return server, port

        # Si el usuario no se encuentra, devolver 2
        return 2

    @staticmethod
    def  getfile(user,  remote_FileName,  local_FileName) :
        socketS = client.connect_socket()
        if socketS is None:
            print("Error al crear el socket. No se pudo establecer la conexión.")
            print("GET_FILE FAIL")
            return client.RC.USER_ERROR
        try:
            if not client._connected:
                print("GET_FILE FAIL")
                return client.RC.USER_ERROR
            # Pedir ip y puerto al servidor a partir del nombre de usuario
            message = (
                b'LIST_USERS\0' + 
                client.get_time().encode('utf-8') + b'\0' +
                client._user.encode('utf-8') + b'\0'
            )
            socketS.sendall(message)
            response = client.read_response(socketS)
            # Procesar la respuesta del servidor
            if response == '0':
                data = client.get_data_by_user(socketS, user)
                if data == 2:
                    print("GET_FILE FAIL")
                    return client.RC.USER_ERROR
                else:
                    server, port = data
            else:
                print("GET_FILE FAIL")
                return client.RC.USER_ERROR
            # Ya tenemos la info que necesitamos y podemos pedirsela al otro cliente
            socketC = client.connect_socket(port, server)
            if socketC is None:
                print("Error al crear el socket. No se pudo establecer la conexión.")
                print("GET_FILE FAIL")
                return client.RC.USER_ERROR
            messageC = (
                b'GET_FILE\0' +
                remote_FileName.encode('utf-8') + b'\0'
            )
            socketC.sendall(messageC)
            responseC = client.read_response(socketC)
            if responseC == '0':
                print("GET_FILE OK")
                file_desc = client.read_response(socketC)
                client._archivos[local_FileName] = file_desc
                return client.RC.OK
            elif responseC == '1':
                print("GET_FILE FAIL / FILES NOT EXIST")
                return client.RC.ERROR
            else:
                print("GET_FILE FAIL")
                return client.RC.USER_ERROR
        except Exception as e:
            # El print del error quizás se puede quitar
            print('Error:', e)
            print("GET_FILE FAIL")
            return client.RC.USER_ERROR

        finally:
            socketS.close()

    # *
    # **
    # * @brief Command interpreter for the client. It calls the protocol functions.
    @staticmethod
    def shell():

        while (True) :
            try :
                command = input("c> ")
                line = command.split(" ")
                if (len(line) > 0):

                    line[0] = line[0].upper()

                    if (line[0]=="REGISTER") :
                        if (len(line) == 2) :
                            client.register(line[1])
                        else :
                            print("Syntax error. Usage: REGISTER <userName>")

                    elif(line[0]=="UNREGISTER") :
                        if (len(line) == 2) :
                            client.unregister(line[1])
                        else :
                            print("Syntax error. Usage: UNREGISTER <userName>")

                    elif(line[0]=="CONNECT") :
                        if (len(line) == 2) :
                            client.connect(line[1])
                        else :
                            print("Syntax error. Usage: CONNECT <userName>")
                    
                    elif(line[0]=="PUBLISH") :
                        if (len(line) >= 3) :
                            #  Remove first two words
                            description = ' '.join(line[2:])
                            client.publish(line[1], description)
                        else :
                            print("Syntax error. Usage: PUBLISH <fileName> <description>")

                    elif(line[0]=="DELETE") :
                        if (len(line) == 2) :
                            client.delete(line[1])
                        else :
                            print("Syntax error. Usage: DELETE <fileName>")

                    elif(line[0]=="LIST_USERS") :
                        if (len(line) == 1) :
                            client.listusers()
                        else :
                            print("Syntax error. Use: LIST_USERS")

                    elif(line[0]=="LIST_CONTENT") :
                        if (len(line) == 2) :
                            client.listcontent(line[1])
                        else :
                            print("Syntax error. Usage: LIST_CONTENT <userName>")

                    elif(line[0]=="DISCONNECT") :
                        if (len(line) == 2) :
                            client.disconnect(line[1])
                        else :
                            print("Syntax error. Usage: DISCONNECT <userName>")

                    elif(line[0]=="GET_FILE") :
                        if (len(line) == 4) :
                            client.getfile(line[1], line[2], line[3])
                        else :
                            print("Syntax error. Usage: GET_FILE <userName> <remote_fileName> <local_fileName>")

                    elif(line[0]=="QUIT") :
                        if (len(line) == 1) :
                            # Activamos el evento para que el hilo pare
                            client._stop_flag.set()
                            #hacer shutdown del socket qyue esta en escuha, se cierra con una excepcion tratandola clientsocket.shutdown y un try except dentro del hilo
                            break
                        else :
                            print("Syntax error. Use: QUIT")
                    else :
                        print("Error: command " + line[0] + " not valid.")
            except Exception as e:
                print("Exception: " + str(e))

    # *
    # * @brief Prints program usage
    @staticmethod
    def usage() :
        print("Usage: python3 client.py -s <server> -p <port>")


    # *
    # * @brief Parses program execution arguments
    @staticmethod
    def  parseArguments(argv) :
        parser = argparse.ArgumentParser()
        parser.add_argument('-s', type=str, required=True, help='Server IP')
        parser.add_argument('-p', type=int, required=True, help='Server Port')
        args = parser.parse_args()

        if (args.s is None):
            parser.error("Usage: python3 client.py -s <server> -p <port>")
            return False

        if ((args.p < 1024) or (args.p > 65535)):
            parser.error("Error: Port must be in the range 1024 <= port <= 65535")
            return False
        
        client._server = args.s
        client._port = args.p

        return True


    # ******************** MAIN *********************
    @staticmethod
    def main(argv) :
        if (not client.parseArguments(argv)) :
            client.usage()
            return

        #  Write code here
        client.shell()
        print("+++ FINISHED +++")
    

if __name__=="__main__":
    client.main([])