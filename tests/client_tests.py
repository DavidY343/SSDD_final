import os
import sys

# Agregar la ruta del directorio anterior al path de Python
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
sys.path.append(parent_dir)

import unittest
from client import client

class TestClient(unittest.TestCase):

	client._port = 4242
	client._server = '172.22.219.110'
	# Test básico que prueba a registrar a un usuario
	def test_a_register_success(self):
		print('\n\nTEST1\n')

		# Intenta registrar un usuario
		result = client.register('test1-user')

		# Verifica que el registro sea exitoso
		self.assertEqual(result, client.RC.OK)

	# Test para comprobar que si el usuario está registrado da error
	def test_b_register_repeated_failure(self):
		print('\n\nTEST2\n')

		# Registrar un usuario
		client.register('test2-user')
		
		# Intenta registrar un usuario con el mismo nombre
		result = client.register('test2-user')

		# Verifica que el registro sea un fallo de tipo 1
		self.assertEqual(result, client.RC.ERROR)

	# Test para comprobar que si el nombre de usuario es demasiado largo da error
	def test_c_register_exceed_bytes_failure(self):
		print('\n\nTEST3\n')

		# Intenta registrar un usuario con un nombre demasiado largo
		result = client.register('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa')

		# Verifica que el registro sea un fallo de tipo 2
		self.assertEqual(result, client.RC.USER_ERROR)

	# Test para comprobar que funciona el darse de baja
	def test_d_unregister_success(self):
		print('\n\nTEST4\n')

		# Registrar un usuario
		client.register('test4-user')

		# Darlo de baja
		result = client.unregister('test4-user')

		# Verifica que se ha dado de baja de forma exitosa
		self.assertEqual(result, client.RC.OK)

	# Test para comprobar que no se puede dar de baja un usuario que no existe
	def test_e_unregister_nonexistent_user_failure(self):
		print('\n\nTEST5\n')

		# Dar de baja un usuario que no está registrado
		result = client.unregister('test5-user')

		# Verifica el error
		self.assertEqual(result, client.RC.ERROR)

	# Test para comprobar que funciona la conexion
	def test_f_connect_success(self):
		print('\n\nTEST6\n')

		# Registrar un usuario
		client.register('test6-user')

		# Conectarlo
		result = client.connect('test6-user')

		# Desconectarlo para que no de errores después
		client.disconnect('test6-user')

		# Verifica que se ha conectado
		self.assertEqual(result, client.RC.OK)

	# Test para comprobar que da error cuando intentas conectar a un usuario no registrado
	def test_g_connect_nonexistent_user_failure(self):
		print('\n\nTEST7\n')

		# Intenta conectar a un usuario no registrado
		result = client.connect('test7-user')

		# Verifica el error
		self.assertEqual(result, client.RC.ERROR)

	# Test para comprobar que da error cuando intentas conectar a un usuario ya conectado
	def test_h_connect_already_connected_user_failure(self):
		print('\n\nTEST8\n')

		# Registrar un usuario
		client.register('test8-user')

		# Conecta al ususario
		client.connect('test8-user')

		# Intenta volver a conectar al usuario
		result = client.connect('test8-user')

		# Desconectarlo para que no de errores después
		client.disconnect('test8-user')

		# Verifica el error
		self.assertEqual(result, client.RC.USER_ERROR)

	# Test para comprobar que funciona la publicación de archivos
	def test_i_publish_success(self):
		print('\n\nTEST9\n')

		# Registrar un usuario
		client.register('test9-user')

		# Conectarlo
		client.connect('test9-user')

		# Publica un archivo en el servidor
		result = client.publish('test9-file', 'test9-descripcion')

		# Desconectarlo para que no de errores después
		client.disconnect('test9-user')

		# Verifica que se ha publicado
		self.assertEqual(result, client.RC.OK)

	# Test para comprobar que da error cuando intentas publicar un archivo con un nombre muy grande
	def test_j_publish_exceed_bytes_failure(self):
		print('\n\nTEST10\n')

		# Registrar un usuario
		client.register('test10-user')

		# Conectarlo
		client.connect('test10-user')

		# Intenta publicar un archivo en el servidor
		result = client.publish('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa', 'test10-descripcion')

		# Desconectarlo para que no de errores después
		client.disconnect('test10-user')

		# Verifica el error
		self.assertEqual(result, client.RC.ANOTHER_CASES)
	"""
	# Test para comprobar que da error cuando intentas publicar un archivo estando el usuario sin registrar
	def test_k_publish_not_register_failure(self):
		print('\n\nTEST11\n')

		# Registrar un usuario
		client.register('test11-user')

		# Conectarlo
		client.connect('test11-user')

		# Eliminar el registro del usuario
		client.unregister('test11-user')

		# Intenta publicar un archivo en el servidor
		result = client.publish('test11-file', 'test11-descripcion')

		# Verifica el error
		self.assertEqual(result, client.RC.ERROR)
	"""
	# Test para comprobar que da error cuando intentas publicar un archivo estando el usuario sin conectar
	def test_l_publish_not_connected_failure(self):
		print('\n\nTEST12\n')

		# Registrar un usuario
		client.register('test12-user')
		
		# Intenta publicar un archivo en el servidor
		result = client.publish('test12-file', 'test12-descripcion')

		# Verifica el error
		self.assertEqual(result, client.RC.USER_ERROR)

	# Test para comprobar que da error cuando intentas publicar un archivo ya publicado
	def test_m_already_published_failure(self):
		print('\n\nTEST13\n')

		# Registrar un usuario
		client.register('test13-user')

		# Conectarlo
		client.connect('test13-user')

		# Publica un archivo en el servidor
		client.publish('test13-file', 'test13-descripcion')

		# Intento publicar uno con el mismo nombre
		result = client.publish('test13-file', 'test13-descripcion-2')

		# Desconectarlo para que no de errores después
		client.disconnect('test13-user')

		# Verifica que se ha publicado
		self.assertEqual(result, client.RC.OTHER_CASES)
	
	# Test para comprobar que funciona la eliminación de archivos publicados
	def test_n_delete_success(self):
		print('\n\nTEST14\n')

		# Registrar un usuario
		client.register('test14-user')

		# Conectarlo
		client.connect('test14-user')

		# Publica un archivo en el servidor
		client.publish('test14-file', 'test14-descripcion')

		# Intento publicar uno con el mismo nombre
		result = client.delete('test14-file')

		# Desconectarlo para que no de errores después
		client.disconnect('test14-user')

		# Verifica que se ha publicado
		self.assertEqual(result, client.RC.OK)
	"""
	# Test para comprobar que da error cuando intentas eliminar un archivo publicado estando el usuario sin registrar
	def test_o_delete_not_register_failure(self):
		print('\n\nTEST15\n')

		# Registrar un usuario
		client.register('test15-user')

		# Conectarlo
		client.connect('test15-user')

		# Publica archivo del cliente que acabo de crear
		client.publish('test15-file', 'test15-descripcion')

		# Eliminar el registro del usuario
		client.unregister('test15-user')

		# Intenta eliminar el archivo de antes
		result = client.delete('test15-file')

		# Verifica el error
		self.assertEqual(result, client.RC.ERROR)
	"""
	# Test para comprobar que da error cuando intentas eliminar un archivo publicado estando el usuario sin conectar
	def test_p_delete_not_connected_failure(self):
		print('\n\nTEST16\n')

		# Intenta publicar un archivo en el servidor
		result = client.delete('test16-file')

		# Verifica el error
		self.assertEqual(result, client.RC.USER_ERROR)

	# Test para comprobar que de error cuando intentas eliminar un archivo que no ha sido publicado
	def test_q_delete_not_exists_failure(self):
		print('\n\nTEST17\n')

		# Registrar un usuario
		client.register('test17-user')

		# Conectarlo
		client.connect('test17-user')

		# Intento publicar uno con el mismo nombre
		result = client.delete('test17-file')

		# Desconectarlo para que no de errores después
		client.disconnect('test17-user')

		# Verifica que se ha publicado
		self.assertEqual(result, client.RC.OTHER_CASES)

	# Test para comprobar que funciona la lista de usuarios conectados
	def test_r_list_users_success(self):
		print('\n\nTEST18\n')

		# Registrar un usuario
		client.register('test18-user')

		# Conectarlo
		client.connect('test18-user')

		# Muestra a los usuarios conectados
		result = client.listusers()

		# Desconectarlo para que no de errores después
		client.disconnect('test18-user')

		# Verifica que se ha publicado
		self.assertEqual(result, client.RC.OK)

	# Test para comprobar que da error cuando intentas ver la lista de usuarios sin estar conectado
	def test_s_list_users_not_connected_failure(self):
		print('\n\nTEST19\n')

		# Intenta mostrar a los usuarios conectados
		result = client.listusers()

		# Verifica que se ha publicado
		self.assertEqual(result, client.RC.USER_ERROR)
	
	# Test para comprobar que funciona la lista de contenido de un usuario
	def test_t_list_content_success(self):
		print('\n\nTEST20\n')

		# Registrar un usuario
		client.register('test20-user')

		# Conectarlo
		client.connect('test20-user')

		# Publica un archivo
		client.publish('test20-file', 'test20-descripcion')

		# Muestra a los usuarios conectados
		result = client.listcontent('test20-user')

		# Desconectarlo para que no de errores después
		client.disconnect('test20-user')

		# Verifica que se ha publicado
		self.assertEqual(result, client.RC.OK)
	
	
	# Test para comprobar que da error la lista de contenido cuando el usuario no está conectado
	def test_u_list_content_not_connected_failure(self):
		print('\n\nTEST21\n')

		# Muestra a los usuarios conectados
		result = client.listcontent('test21-user')

		# Verifica que se ha publicado
		self.assertEqual(result, client.RC.USER_ERROR)
	
	
	# Test para comprobar que de error la lista de contenido cuando el usuario remoto no existe
	def test_v_list_content_not_exists_failure(self):
		print('\n\nTEST22\n')

		# Registrar un usuario
		client.register('test22-user')

		# Conectarlo
		client.connect('test22-user')

		# Muestra a los usuarios conectados
		result = client.listcontent('test22_2-user')

		# Desconectarlo para que no de errores después
		client.disconnect('test22-user')

		# Verifica que se ha publicado
		self.assertEqual(result, client.RC.OTHER_CASES)
	
	# Test para comprobar que funciona get file
	def test_w_get_file_success(self):
		print('\n\nTEST23\n')

		# Registrar un usuario
		client.register('test23-user')

		# Conectarlo
		client.connect('test23-user')

		# Publica un archivo
		client.publish('tests/test23-file', 'test23-descripcion')

		# Recibe el archivo del mismo cliente
		result = client.getfile('test23-user', 'tests/test23-file', 'tests/test23_2-file')

		# Desconectarlo para que no de errores después
		client.disconnect('test23-user')

		# Verifica que se ha publicado
		self.assertEqual(result, client.RC.OK)
	
	
	# Test para comprobar que da error el get file cuando el usuario no está conectado
	def test_x_get_file_not_connected_failure(self):
		print('\n\nTEST24\n')

		# Muestra a los usuarios conectados
		result = client.getfile('test24-user', 'test24-file', 'test24_2-file')

		# Verifica que se ha publicado
		self.assertEqual(result, client.RC.USER_ERROR)
	
	
	# Test para comprobar que funciona get file
	def test_y_get_file_not_exists_failure(self):
		print('\n\nTEST25\n')

		# Registrar un usuario
		client.register('test25-user')

		# Conectarlo
		client.connect('test25-user')

		# Muestra a los usuarios conectados
		result = client.getfile('test25-user', 'test25-file', 'test25_2-file')

		# Desconectarlo para que no de errores después
		client.disconnect('test25-user')

		# Verifica que se ha publicado
		self.assertEqual(result, client.RC.ERROR)

if __name__ == '__main__':
	unittest.main()
	
