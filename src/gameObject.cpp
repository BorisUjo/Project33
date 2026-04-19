#include <gameobject.h>
#include <glm/gtc/type_ptr.hpp>

void GameObject::render()
{
	objectMesh.render();
}

void GameObject::picking_render(ShaderManager& shaderManager, TempCamera& camera)
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

void GameObject::bind_shader(ShaderManager& shaderManager, TempCamera& camera)
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
	glUniform1i(defaultShader->getUniform("tex0"), 0);
	glUniform3f(defaultShader->getUniform("lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(defaultShader->getUniform("objPos"), transform.position.x, transform.position.y, transform.position.z);
	glUniform1f(defaultShader->getUniform("glfwTime"), (float)glfwGetTime());
}

void GameObject::bind_texture(TextureManager& textureManager)
{
	textureManager.texturesList[textureID].bind();
}

void GameObject::destroy()
{
	auto& gm = GameManager::getInstance();
	gm.destroy(this);
}




void TilePlane::bind_shader(ShaderManager& shaderManager, TempCamera& camera)
{

	if (get_render_object()->get_render_state() == false)
	{
		return;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	auto* planeShader = &shaderManager.getShader(TILE_PLANE_SHADER_INDEX);
	planeShader->bind();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, transform.position);
	model = glm::rotate(model, glm::radians(transform.angle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, transform.scale);

	glUniformMatrix4fv(planeShader->getUniform("PVM"), 1, GL_FALSE, glm::value_ptr(camera.get_PV_static()));
	glUniformMatrix4fv(planeShader->getUniform("transform"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform1i(planeShader->getUniform("tex0"), 0);
	glUniform3f(planeShader->getUniform("planeColor"), color.x, color.y, color.z);

	//glDisable(GL_BLEND);


}

void TilePlane::set_active(bool state)
{
	get_render_object()->set_render_state(state);
}
