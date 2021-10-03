#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <iostream>

#include "shader.hpp"

GLuint ShaderCompilation(const GLchar* source, const int shaderType);
GLuint MakeProgram(const GLuint shader1, const GLuint shader2);

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void vertexRecount(const GLfloat* vertices, GLfloat* drawVert, const int vertCount, const int width, const int height);


//returns value of y for given x and a
double plotFunc(double x, double a);

void delFromArr(GLfloat* arr, int& vertCount, int index);
//nan destroyer
void nanDestroyer(GLfloat* arr, int& vertCount);


bool IS_WINDOW_RESIZED = false;


int main() {
    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	if(!glfwInit()) {
    	return -1;
	}

	GLFWwindow* window = glfwCreateWindow(800, 600, "Computer Graphics Lab 1", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
    	std::cout << "Failed to initialize GLEW" << std::endl;
    	return -1;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

    Shader shader1("vertexShader.vert", "fragmentShader.frag");
    Shader shader2("vertexShader.vert", "axisShader.frag");

    const double step = 0.001;

    const GLfloat a = 1;

    int stepCount = 2 * (1 / step);
    int vertCount = (stepCount + 1) * 2;
    int indCount = vertCount * 3;
    GLfloat* vertices = (GLfloat*)malloc(sizeof(GLfloat) * indCount);
    double x;
    double y;
    int index = 0;
    x = -1.0;
    for (index = 0; index < indCount / 2; index += 3) {
        y = plotFunc(x, a);
        vertices[index] = (GLfloat)x;
        vertices[index + 1] = (GLfloat)y;
        vertices[index + 2] = 0.0f;
        vertices[indCount - 1 - index - 2] = (GLfloat)x;
        vertices[indCount - 1 - index - 1] = -(GLfloat)y;
        vertices[indCount - 1 - index] = 0.0f;
        x += step;
    }
    nanDestroyer(vertices, vertCount);

    GLfloat* drawVert = (GLfloat*)malloc(sizeof(GLfloat) * indCount);
    for (int i = 2; i < vertCount * 3; i += 3) {
        drawVert[i] = 0.0f;
    }
    glfwGetWindowSize(window, &width, &height);
    vertexRecount(vertices, drawVert, vertCount, width, height);

    GLuint plot_VBO, plot_VAO;
    glGenVertexArrays(1, &plot_VAO);
    glGenBuffers(1, &plot_VBO);

    glBindVertexArray(plot_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, plot_VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertCount * 3, drawVert, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLfloat axisVertices[] = {
        -1.0f,  0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f
    };
    GLuint axisIndices[] = {
        0, 1, 4,
        2, 3
    };

    GLuint axis_VAO, axis_VBO, axis_EBO;
    glGenVertexArrays(1, &axis_VAO);
    glGenBuffers(1, &axis_VBO);
    glGenBuffers(1, &axis_EBO);

    glBindVertexArray(axis_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, axis_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, axis_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(axisIndices), axisIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	while (!glfwWindowShouldClose(window)) {
    	glfwPollEvents();

        if (IS_WINDOW_RESIZED) {
            glfwGetWindowSize(window, &width, &height);
            //std::cout << width << " " << height << std::endl;
            vertexRecount(vertices, drawVert, vertCount, width, height);

            glBindBuffer(GL_ARRAY_BUFFER, plot_VBO);

            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertCount * 3, drawVert, GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            IS_WINDOW_RESIZED = false;
        }

        glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader2.Use();
        glBindVertexArray(axis_VAO);
        glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_INT, 0);

        shader1.Use();
        glBindVertexArray(plot_VAO);
        glDrawArrays(GL_LINE_LOOP, 0, vertCount);
        glBindVertexArray(0);

    	glfwSwapBuffers(window);
	}
    glDeleteVertexArrays(1, &plot_VAO);
    glDeleteBuffers(1, &plot_VBO);
    glDeleteBuffers(1, &axis_EBO);
    free(vertices);
    free(drawVert);

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    IS_WINDOW_RESIZED = true;
    //std::cout << width << " " << height << std::endl;
}

void vertexRecount(const GLfloat* vertices, GLfloat* newVert, const int vertCount, const int width, const int height) {
    const double PLOT_SCALE = 0.8;
    const double RATIO_BARRIER = 0.4;
    double ratio = (double)height / width;
    if (ratio < RATIO_BARRIER) {
        ratio = ratio / RATIO_BARRIER;
        //std::cout << "ratio is " << ratio << std::endl;
        for (int i = 0; i < vertCount * 3; i += 3) {
            newVert[i] = vertices[i] * ratio * PLOT_SCALE;
            newVert[i + 1] = vertices[i + 1] * (1 / RATIO_BARRIER) * PLOT_SCALE;
        }
    } else {
        ratio = 1 / ratio;
        //std::cout << "ratio is " << ratio << std::endl;
        for (int i = 0; i < vertCount * 3; i += 3) {
            newVert[i] = vertices[i] * PLOT_SCALE;
            newVert[i + 1] = vertices[i + 1] * ratio * PLOT_SCALE;
        }
    }
    
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

double plotFunc(double x, double a) {
    double posY;
    posY = sqrt((-2 * x * x - a + sqrt(8 * a * x * x + a * a)) / 2.0);
    return posY;
}