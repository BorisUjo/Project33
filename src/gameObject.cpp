#include <gameobject.h>
#include <glm/gtc/type_ptr.hpp>

void GameObject::render()
{
	objectMesh.render();
}

void GameObject::picking_render(ShaderManager& shaderManager, Camera& camera)
{
	auto* pickingShader = &shaderManager.getShader(PICKING_SHADER_INDEX);
	pickingShader->bind();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, transform.position);
	model = glm::rotate(model, glm::radians(transform.angle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, transform.scale);
	glUniformMatrix4fv(pickingShader->getUniform("PVM"), 1, GL_FALSE, glm::value_ptr(camera.get_PV_static()));
	glUniformMatrix4fv(pickingShader->getUniform("transform"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform1ui(pickingShader->getUniform("objectIndex"), meshID);
	glUniform1ui(pickingShader->getUniform("drawIndex"), meshID);
}

void GameObject::bind_shader(ShaderManager& shaderManager, Camera& camera)
{
	auto* defaultShader = &shaderManager.getShader(DEFAULT_SHADER_INDEX);
	defaultShader->bind();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, transform.position);
	model = glm::rotate(model, glm::radians(transform.angle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, transform.scale);
	float lightX = 5.0f * sin((float)glfwGetTime());
	float lightZ = 5.0f * cos((float)glfwGetTime());

	glm::vec3 lightPos = glm::vec3(lightX, 1.0f, lightZ);
	lightPos = transform.position;
	lightPos.y += 5.0f;

	glUniformMatrix4fv(defaultShader->getUniform("PVM"), 1, GL_FALSE, glm::value_ptr(camera.get_PV_static()));
	glUniformMatrix4fv(defaultShader->getUniform("transform"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(defaultShader->getUniform("lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform1i(defaultShader->getUniform("tex0"), 0);
	glUniform1i(defaultShader->getUniform("isTile"), false);
	glUniform1i(defaultShader->getUniform("isForest"), false); // temporary
}

void GameObject::bind_texture(TextureManager& textureManager)
{
	textureManager.texturesList[textureID].bind();
}