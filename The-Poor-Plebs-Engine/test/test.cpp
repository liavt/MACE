#include <iostream>
#include <GLFW/glfw3.h>

int main() {
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(800, 600, "GLFW window test!", NULL, NULL);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}