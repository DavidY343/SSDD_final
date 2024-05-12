import unittest
from client import client

class TestClient(unittest.TestCase):

	client._port = 4242
	client._server = '172.22.219.110'
	
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
	
	"""
	# Test para comprobar que da error la lista de contenido cuando el usuario no está conectado
	def test_u_list_content_not_connected_failure(self):
		print('\n\nTEST21\n')

		# Muestra a los usuarios conectados
		result = client.listcontent('test21-user')

		# Verifica que se ha publicado
		self.assertEqual(result, client.RC.USER_ERROR)
	
	"""
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
		client.publish('test23-file', 'test23-descripcion')

		# Recibe el archivo del mismo cliente
		result = client.getfile('test23-user', 'test23-file', 'test23_2-file')

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
	
	"""
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
	"""
if __name__ == '__main__':
	unittest.main()
