#include <testscene.h>
#include <mesh.h>
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
void TestingScene::init()
{
	defaultShader.loadShaderProgramFromFile(RESOURCES_PATH "vertex.vert", RESOURCES_PATH "fragment.frag");
	defaultShader.bind();

	Mesh mesh;

	loadMeshFromFile(RESOURCES_PATH "flag_post_triangulated.obj", mesh);

	object1.initialise(mesh);

}

void TestingScene::update()
{

	defaultShader.bind();

	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, glm::vec3(0,-0.5f,0));
	transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
	
	glUniformMatrix4fv(defaultShader.getUniform("transform"), 1, GL_FALSE, glm::value_ptr(transform));

	glUniform1f(defaultShader.getUniform("time"), (float)glfwGetTime());

	object1.render();
}
