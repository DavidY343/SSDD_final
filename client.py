from enum import Enum
import argparse
import sockets

class client :

    # ******************** TYPES *********************
    # *
    # * @brief Return codes for the protocol methods
    class RC(Enum) :
        OK = 0
        ERROR = 1
        USER_ERROR = 2
        OTHER_CASES = 3
        ANTOHER_CASES = 4

    # ****************** ATTRIBUTES ******************
    _server = None
    _port = -1
    _connected = False
    _user = None

    # ******************** METHODS *******************

    def read_response(socketC):
        response = ''
        while True:
            server_response = socketC.recv(1)
            if (server_response == b'\0'):
                break
            response += server_response.decode()
        return response

    def connect_socket():
        try:
            socketC = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_address = (self._server, self._port)
            print('connecting to {} port {}'.format(*server_address))
            return socketC
        except Exception as e:
            print('Error al crear el socket: ', e)
            return None

    # Método para hacer las comprobaciones de las cadenas que no pueden superar los 256 bytes.
    # Se comprueba 255 para tener en cuenta el \0
    def excede_limite_255_bytes(cadena):
        encoded_cadena = cadena.encode('utf-8')
        # Comprobar si la longitud de la cadena codificada excede 255 bytes
        return len(encoded_cadena) > 255

    @staticmethod
    def  register(user) :
        socketC = connect_socket()
        if socketC is None:
            print("Error al crear el socket. No se pudo establecer la conexión.")
            print("REGISTER FAIL")
            return client.RC.ERROR
        try:
            message = (
                b'REGISTER\0' + 
                user.encode('utf-8') + b'\0'
            )
            socketC.sendall(message)
            response = read_response(socketC)

            #Gestion del resultado
            if response == 0:
                print("REGISTER OK")
                return client.RC.OK
            elif response == 1:
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
            print('closing socket')
            socketC.close()

   
    @staticmethod
    def  unregister(user) :
        socketC = connect_socket()
        if socketC is None:
            print("Error al crear el socket. No se pudo establecer la conexión.")
            print("UNREGISTER FAIL")
            return client.RC.ERROR
        try:
            message = (
                b'UNREGISTER\0' + 
                user.encode('utf-8') + b'\0'
            )
            socketC.sendall(message)
            response = read_response(socketC)

            #Gestion del resultado
            if response == 0:
                print("UNREGISTER OK")
                return client.RC.OK
            elif response == 1:
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
            print('closing socket')
            socketC.close()


    @staticmethod
    def  connect(user) :
        socketC = connect_socket()
        if socketC is None:
            print("Error al crear el socket. No se pudo establecer la conexión.")
            print("CONNECT FAIL")
            return client.RC.OTHER_CASES
        try:
            message = (
                b'CONNECT\0' + 
                user.encode('utf-8') + b'\0' + 
                self._port.encode('utf-8') + b'\0'
            )
            socketC.sendall(message)
            response = read_response(socketC)

            #Gestion del resultado
            if response == 0:
                print("CONNECT OK")
                self._connected = True
                self._user = user
                #Lanzar un hilo para que se convierta en server y taaal
                return client.RC.OK
            elif response == 1:
                print("CONNECT FAIL, USER DOES NOT EXIST")
                return client.RC.ERROR
            elif response == 2:
                print("USER ALREADY CONNECTED")
                return client.RC.USER_ERROR
            else:
                print("CONNECT FAIL")
                return client.RC.OTHER_CASES
        except Exception as e:
            print('Error:', e)
            print("CONNECT FAIL")
            return client.RC.OTHER_CASES

        finally:
            print('closing socket')
            socketC.close()
    
    @staticmethod
    def  disconnect(user) :
        socketC = connect_socket()
        if socketC is None:
            print("Error al crear el socket. No se pudo establecer la conexión.")
            print("DISCONNECT FAIL")
            return client.RC.OTHER_CASES
        try:
            message = (
                b'DISCONNECT\0' + 
                user.encode('utf-8') + b'\0'
            )
            socketC.sendall(message)
            response = read_response(socketC)
            self._connected = False
            self._user = None
            #Parar ejecución del hilo
            #Gestion del resultado
            if response == 0:
                print("DISCONNECT OK")
                return client.RC.OK
            elif response == 1:
                print("DISCONNECT FAIL / USER DOES NOT EXIST")
                return client.RC.ERROR
            elif response == 2:
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
            print('closing socket')
            socketC.close()

    @staticmethod
    def  publish(fileName,  description) :
        socketC = connect_socket()
        if socketC is None:
            print("PUBLISH FAIL")
            print("Error al crear el socket. No se pudo establecer la conexión.")
            return client.RC.ANTOHER_CASES
        try:
            message = (
                b'PUBLISH\0' + 
                self._user.encode('utf-8') + b'\0' + 
                fileName.replace(" ", "").encode('utf-8') + b'\0' + 
                description.encode('utf-8') + b'\0'
            )
            socketC.sendall(message)
            response = read_response(socketC)
            if response == 0:
                print("PUBLISH OK")
                return client.RC.OK
            elif response == 1:
                print("PUBLISH FAIL, USER DOES NOT EXIST")
                return client.RC.ERROR
            elif response == 2:
                print("PUBLISH FAIL, USER NOT CONNECTED")
                return client.RC.USER_ERROR
            elif response == 3:
                print("PUBLISH FAIL, CONTENT ALREDAY PUBLISHED")
                return client.RC.OTHER_CASES
            else:
                print("PUBLISH FAIL")
                return client.RC.ANTOHER_CASES
        except Exception as e:
            print("PUBLISH FAIL")
            print('Error:', e)
            return client.RC.ANTOHER_CASES

        finally:
            print('closing socket')
            socketC.close()

    @staticmethod
    def  delete(fileName) :
        socketC = connect_socket()
        if socketC is None:
            print("Error al crear el socket. No se pudo establecer la conexión.")
            print("DELETE FAIL")
            return client.RC.ANTOHER_CASES
        try:
            message = (
                b'DELETE\0' + 
                self._user.encode('utf-8') + b'\0' + 
                fileName.replace(" ", "").encode('utf-8') + b'\0'
            )
            socketC.sendall(message)
            response = read_response(socketC)
            if response == 0:
                print("DELETE OK")
                return client.RC.OK
            elif response == 1:
                print("DELETE FAIL, USER DOES NOT EXIST")
                return client.RC.ERROR
            elif response == 2:
                print("DELETE FAIL, USER NOT CONNECTED")
                return client.RC.USER_ERROR
            elif response == 3:
                print("DELETE FAIL, CONTENT NOT PUBLISHED")
                return client.RC.OTHER_CASES
            else:
                print("DELETE FAIL")
                return client.RC.ANTOHER_CASES
        except Exception as e:
            # El print del error quizás se puede quitar
            print('Error:', e)
            print("DELETE FAIL")
            return client.RC.ANTOHER_CASES

        finally:
            print('closing socket')
            socketC.close()

    @staticmethod
    def  listusers() :
        socketC = connect_socket()
        if socketC is None:
            print("Error al crear el socket. No se pudo establecer la conexión.")
            print("LIST_USERS FAIL")
            return client.RC.OTHER_CASES
        try:
            message = (
                b'LIST_USERS\0' + 
                self._user.encode('utf-8') + b'\0'
            )
            socketC.sendall(message)
            response = read_response(socketC)
            self._connected = False
            self._user = None
            #Gestion del resultado
            if response == 0:
                # Recibir un segundo mensaje con toda la lista de usuarios
                print("LIST_USERS OK")
                return client.RC.OK
            elif response == 1:
                print("LIST_USERS FAIL, USER DOES NOT EXIST")
                return client.RC.ERROR
            elif response == 2:
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
            print('closing socket')
            socketC.close()

    @staticmethod
    def  listcontent(user) :
        #  Write your code here
        return client.RC.ERROR

    @staticmethod
    def  getfile(user,  remote_FileName,  local_FileName) :
        #  Write your code here
        return client.RC.ERROR

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
                            #Parar ejecución del hilo
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
            parser.error("Error: Port must be in the range 1024 <= port <= 65535");
            return False;
        
        _server = args.s
        _port = args.p

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