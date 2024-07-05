#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//Classe que gerencia os shaders
#include "Shader.h"

//GLM
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//STB_IMAGE
#include <stb_image.h>

#include "Sprite.h"

// Prot�tipo da fun��o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Protótipos das funções
GLuint loadTexture(string filePath, int &imgWidth, int &imgHeight);


// Dimens�es da janela (pode ser alterado em tempo de execu��o)
const GLuint WIDTH = 1920, HEIGHT = 1080;

// Fun��o MAIN
int main()
{
	glfwInit();

	// Cria��o da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Tarefa M4", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da fun��o de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d fun��es da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informa��es de vers�o
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	//Habilitando a transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Habilitando o teste de profundidade
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);



	// Compilando e buildando o programa de shader

	Shader shader("tarefaM4.vs","tarefaM4.fs");
	//Habilita o shader que sera usado (glUseProgram)
	shader.Use();

	int imgWidth, imgHeight;
	GLuint texID = loadTexture("../img/Ryu_left.png", imgWidth, imgHeight);
	
	//Criação de um objeto Sprite
	Sprite sprite, sprite2, sprite3, background, ground, sprite4;
	sprite.inicializar(texID, 1, 1, glm::vec3(300.0,200.0,0.0), glm::vec3(imgWidth*2,imgHeight*2,1.0),0.0,glm::vec3(1.0,0.0,1.0));
	sprite.setShader(&shader);

	texID = loadTexture("../img/ChunLi_right.png", imgWidth, imgHeight);
	//Criação de um objeto Sprite
	sprite2.inicializar(texID, 1, 1, glm::vec3(900.0,200.0,0.0), glm::vec3(imgWidth*2,imgHeight*2,1.0),0.0,glm::vec3(1.0,0.0,1.0));
	sprite2.setShader(&shader);

	GLuint texID2 = loadTexture("../img/background.png", imgWidth, imgHeight);

	background.inicializar(texID2, 1, 1, glm::vec3(640.0,360.0,0.0), glm::vec3(imgWidth*2,imgHeight*3,1.0),0.0,glm::vec3(0.0,1.0,1.0));
	background.setShader(&shader);

	glm::mat4 projection = glm::ortho(0.0, 1280.0, 0.0, 720.0, -1.0, 1.0);
	//Enviando para o shader via variável do tipo uniform (glUniform....)
	shader.setMat4("projection",glm::value_ptr(projection));

	glActiveTexture(GL_TEXTURE0);
	shader.setInt("texBuffer", 0);
	
	// Loop da aplica��o - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float angulo = (float)glfwGetTime();
		sprite4.setAngulo(glm::degrees(angulo));

		background.desenhar();
		sprite.desenhar();
		sprite2.desenhar();

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	
	// Finaliza a execu��o da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Fun��o de callback de teclado - s� pode ter uma inst�ncia (deve ser est�tica se
// estiver dentro de uma classe) - � chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

GLuint loadTexture(string filePath, int &imgWidth, int &imgHeight)
{
	GLuint texID;

	// Gera o identificador da textura na memória 
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
    	if (nrChannels == 3) //jpg, bmp
    	{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    	}
    	else //png
    	{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    	}
    	glGenerateMipmap(GL_TEXTURE_2D);

		imgWidth = width;
		imgHeight = height;

		stbi_image_free(data);

		glBindTexture(GL_TEXTURE_2D, 0);

	}
	else
	{
    	 std::cout << "Failed to load texture" << std::endl;
	}


	return texID;
}

