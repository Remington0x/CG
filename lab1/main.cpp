#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <iostream>

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

//returns value of y for given x and a
double plotFunc(double x, double a) {
    double posY;
    posY = sqrt((-2 * pow(x, 2) - a + sqrt(8 * a * pow(x, 2) + pow(a, 2))) / 2.0);
    return posY;
}



const GLchar* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "void main() {\n"
    "gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
    "}\n\0";

const GLchar* fragmentShaderSource = "#version 330 core\n"
    "out vec4 color;\n"
    "void main() {\n"
    "color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

int main() {
    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	if(!glfwInit()) {
    	return -1;
	}

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, keyCallback);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
    	std::cout << "Failed to initialize GLEW" << std::endl;
    	return -1;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


    //shaders
    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    //Check for errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
    	glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    	std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    //Check for errors
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
    	glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    	std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    //Check for errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    //if linking went ok, delete shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);



    // GLfloat vertices[] = {
    //     -0.5f, -0.5f, 0.0f,
    //     0.5f, -0.5f, 0.0f,
    //     0.0f,  0.5f, 0.0f
    // };

    const double step = 0.001;

    // int vertCount = 2;
    // int index = 0;
    // GLfloat vertices[] = {
    //     -1, 0, 0,
    //     0, 0, 0,
    //     0, -0, 0,
    //     1, 0, 0
    // };


    //const GLfloat a = 1.0;

    int vertCount = 2 * (1 / step);
    GLfloat* vertices = (GLfloat*)malloc(sizeof(GLfloat) * 3 * vertCount * 2);
    double x;
    double y;
    int index = 0;
    vertices[index] = -1.0f;
    vertices[index + 1] = 0.0f;
    vertices[index + 2] = 0.0f;
    x = -1.0 + step;
    for (index = 3; index / 3 < vertCount * 2 - 1; index += 6) {
        // std::cout << "index / 3 = " << index / 3 << ", x = " << x << std::endl;
        y = plotFunc(x, 1);
        vertices[index] = (GLfloat)x;
        vertices[index + 1] = (GLfloat)y;
        vertices[index + 2] = 0.0f;
        vertices[index + 3] = (GLfloat)x;
        vertices[index + 4] = -(GLfloat)y;
        vertices[index + 5] = 0.0f;
        x += step;
    }
    vertices[index] = 1.0f;
    vertices[index + 1] = 0.0f;
    vertices[index + 2] = 0.0f;
    //std::cout << "index count = " << index / 3 << ", vertCount * 2 = " << vertCount * 2 << std::endl;
    // index = 0;
    // while (index / 3 < vertCount * 2 - 1) {
    //     std::cout << vertices[index] << '\t' << vertices[index + 1] << '\t' << vertices[index + 2] << '\t';
    //     std::cout << vertices[index + 3] << '\t' << vertices[index + 4] << '\t' << vertices[index + 5] << '\t';
    //     std::cout << std::endl;
    //     index += 6;
    // }

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertCount * 2 * 3, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	while (!glfwWindowShouldClose(window)) {
    	glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, vertCount * 2);
        glBindVertexArray(0);

    	glfwSwapBuffers(window);
	}
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    //free(vertices);

	glfwTerminate();

    return 0;
}
