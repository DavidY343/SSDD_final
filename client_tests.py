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

	# Test para comprobar que da error cuando intentas publicar un archivo estando el usuario sin registrar
	def test_k_publish_exceed_bytes_failure(self):
		print('\n\nTEST11\n')

		# Registrar un usuario
		client.register('test11-user')

		# Conectarlo
		client.connect('test11-user')

		# Eliminar el registro del usuario
		client.unregister('test11-user')

		# Intenta publicar un archivo en el servidor
		result = client.publish('test11-file', 'test11-descripcion')

		# Desconectarlo para que no de errores después
		client.disconnect('test11-user')

		# Verifica el error
		self.assertEqual(result, client.RC.ERROR)

	# Test para comprobar que da error cuando intentas publicar un archivo estando el usuario sin conectar
	def test_l_publish_exceed_bytes_failure(self):
		print('\n\nTEST12\n')

		# Registrar un usuario
		client.register('test12-user')

		# Intenta publicar un archivo en el servidor
		result = client.publish('test12-file', 'test12-descripcion')

		# Verifica el error
		self.assertEqual(result, client.RC.USER_ERROR)

	# Test para comprobar que da error cuando intentas publicar un archivo ya publicado
	def test_m_publish_success(self):
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

if __name__ == '__main__':
	unittest.main()
