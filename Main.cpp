#include <glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<iostream>
#include "C_Shader.h"
#include "C_Buffer.h"
#include "src/shapeDATA.h"
#include "genShape.h"
	
float WINDOW_WIDTH = 480;
float WINDOW_HEIGHT = 480;
const float BGcolor[4] = { 0.7f , 0.75f , 0.85f , 1.0f };

float fpsCOUNTER() {
	static float last_time = 0.0f;
	float curr_time = glfwGetTime();
	float fps = curr_time - last_time;
	fps = 1 / fps;
	last_time = curr_time;
	return fps;
	// yaha fps mene dynamically nikala easier way bhi hai
}
float Z_CHANGE=5.0f;
void processINPUTS(GLFWwindow * window);
bool ErrorLog();
void pollInput(GLFWwindow* window, int key, int scancode, int action, int mods);

void window_resizer(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	WINDOW_HEIGHT = height;
	WINDOW_WIDTH = width;
}

int main() {
	
	float FPS;
	short INPUT_DELAY = 0;

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	if (!glfwInit()) {
		std::cout<<"Failed to initialize GLFW \n";
	}
	//specify to show which gl version is we using and core or waht level
	//core me humko khud array buffer banana hota hai
	//default me compactible mode par hota hai usme vao khud banta hai
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//creates window of size with 1st null value shouws to fullscreen or not , 2nd is mystery
	GLFWwindow *Apple = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "crimeMaster GOGO", nullptr, nullptr);
	if (!Apple) {
		glfwTerminate();
		std::cout<<"Failed to create GLFW window \n";
	}

	//used for FrameLimit
	//glfwSwapInterval(1);

	//make our all context region to screen
	glfwMakeContextCurrent(Apple);

	//loads glad for usage 
	//opengl provides some function in driver of gpu which i used by gpu but for us to use that func we need their address so glad gives us address of func if we dont use glad we need to use windows pointer to get func add.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout<<"Failed to initialize GLAD";
	}

	glEnable(GL_DEPTH_TEST);

	glClearColor(BGcolor[0], BGcolor[1], BGcolor[2], BGcolor[3]);
	//specifys clear colour remember not background collor but main colour of window

	shapeDATA mess = genShape::genCUBE();
	
	C_Buffer cache;
	cache.parseBuffer(mess);
	//itna hosiyari ke jagah me TOTALtiangle bhi use kar sakta tha but wahi hard coded way is not better
	
	C_Shader newShader;
	newShader.activate();


	int viewLOC = glGetUniformLocation(newShader.GPUcode, "view");
	int modeLOC = glGetUniformLocation(newShader.GPUcode, "model");
	int projLOC = glGetUniformLocation(newShader.GPUcode, "projection");

	glUniformMatrix4fv(modeLOC, 1, GL_FALSE, glm::value_ptr(model));
	glfwSetFramebufferSizeCallback(Apple, window_resizer);
		
		//viewport is the region where opengl will draw
		//0,0 is the bottom left corner of window
		//width and height is the size of window

	glfwSetKeyCallback(Apple,pollInput);
	while (!glfwWindowShouldClose(Apple)) {

		FPS = fpsCOUNTER();
		std::cout << FPS << "\n";
		
		if (INPUT_DELAY >= 60) { processINPUTS(Apple); INPUT_DELAY = 0; }

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// previus buffer jo rewrite nhi huwa usko clean karta hai even when new elements is not drawn at top
		view = glm::mat4(1.0f);
		view = glm::translate(view , glm::vec3(0.0f , 0.0f, -(Z_CHANGE)));
		glUniformMatrix4fv(viewLOC, 1, GL_FALSE, glm::value_ptr(view));
		// baar baar mat ko 1.0 par set karna padta hai kyuki nhi maalum
		model = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(60.0f) , (float)(WINDOW_WIDTH/WINDOW_HEIGHT) , 0.1f , 10.f);
		glUniformMatrix4fv(projLOC, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(cache.vertexArrayScript);

		glDrawElements(GL_TRIANGLES, mess.NUM_INDEXES, GL_UNSIGNED_INT, 0);//drawarray sirf vertecis data ko draw karta hai but agar elemnt specify kiya tab gpu indexed buffer read karke uske vertices draw karta hai

		glfwSwapBuffers(Apple);
		//swaps loaded buffer with present buffer
		glfwPollEvents();
		if (ErrorLog())  std::cout << "An OpenGL error occurred!  \n " ; 

		INPUT_DELAY += 1;
	}
	glfwDestroyWindow(Apple);
	cache.clean();
	newShader.revoke();

	glfwTerminate();
	return 0;
}

void pollInput(GLFWwindow* window , int key ,int scancode , int action ,int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

void processINPUTS(GLFWwindow* window)
{
	//yaha par mene change boht kam kiya hai par end me mujhe ye func ke call acc to fps set karna hai
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)Z_CHANGE -= 0.05f;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)Z_CHANGE += 0.05f;
}

bool ErrorLog() {
	GLenum error = glGetError();
	//fetch error and store for further use
	if (error != GL_NO_ERROR) {
		std::cout << "Main-Error Log - 0x" << error << "\n";
		return true;
	}
	else return false;
}
