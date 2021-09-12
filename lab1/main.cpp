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

void delFromArr(GLfloat* arr, int& vertCount, int index);
//nan destroyer
void nanDestroyer(GLfloat* arr, int& vertCount);



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

    const double step = 0.001;

    const GLfloat a = 0.5;

    int stepCount = 2 * (1 / step);
    int vertCount = (stepCount + 1) * 2;
    int indCount = vertCount * 3;
    GLfloat* vertices = (GLfloat*)malloc(sizeof(GLfloat) * indCount);
    double x;
    double y;
    int index = 0;
    // y = plotFunc(1.0, a);
    // vertices[index] = -1.0f;
    // vertices[index + 1] = (GLfloat)y;
    // vertices[index + 2] = 0.0f;
    x = -1.0;
    for (index = 0; index < indCount / 2; index += 3) {
        //std::cout << "index / 3 = " << index / 3 << ", x = " << x << std::endl;
        y = plotFunc(x, a);
        vertices[index] = (GLfloat)x;
        vertices[index + 1] = (GLfloat)y;
        vertices[index + 2] = 0.0f;
        vertices[indCount - 1 - index - 2] = (GLfloat)x;
        vertices[indCount - 1 - index - 1] = -(GLfloat)y;
        vertices[indCount - 1 - index] = 0.0f;
        x += step;
    }

    // std::cout << "vertCount = " << vertCount << std::endl;
    nanDestroyer(vertices, vertCount);
    // std::cout << "vertCount = " << vertCount << std::endl;
    // y = plotFunc(1.0, a);
    // vertices[index] = 1.0f;
    // vertices[index + 1] = (GLfloat)y;
    // vertices[index + 2] = 0.0f;
    //std::cout << "index count = " << index / 3 << ", vertCount = " << vertCount << std::endl;
    // index = 0;
    // while (index < indCount) {
    //     std::cout << vertices[index] << '\t' << vertices[index + 1] << '\t' << vertices[index + 2] << '\n';
    //     index += 3;
    // }

    GLuint plot_VBO, plot_VAO;
    glGenVertexArrays(1, &plot_VAO);
    glGenBuffers(1, &plot_VBO);

    glBindVertexArray(plot_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, plot_VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertCount * 3, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	while (!glfwWindowShouldClose(window)) {
    	glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(plot_VAO);
        glDrawArrays(GL_LINE_LOOP, 0, vertCount);
        //glDrawArrays(GL_POINTS, 0, vertCount);
        glBindVertexArray(0);

    	glfwSwapBuffers(window);
	}
    glDeleteVertexArrays(1, &plot_VAO);
    glDeleteBuffers(1, &plot_VBO);
    free(vertices);

	glfwTerminate();

    return 0;
}

void nanDestroyer(GLfloat* arr, int& vertCount) {
    for (int i = vertCount * 3 - 3; i >= 0; i -= 3) {
        if (abs(arr[i + 1]) != abs(arr[i + 1])) {
            delFromArr(arr, vertCount, i);
        }
    }
}

void delFromArr(GLfloat* arr, int& vertCount, int index) {
    for (int i = index + 3; i < vertCount * 3; i += 3) {
        arr[i - 3] = arr[i];
        arr[i - 2] = arr[i + 1];
        arr[i - 1] = arr[i + 2];
    }
    --vertCount;
}
