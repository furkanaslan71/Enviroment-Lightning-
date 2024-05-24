#include "Object.h"


Object::Object()
{}

void Object::Render(std::shared_ptr<Camera> camera, int numLights, int specularOn, float kd, float ks, float kt, float km)
{
	m_Shader->Use();
	m_Shader->SetUniformMat4fv("model", glm::mat4(1.0f));
	m_Shader->SetUniform1i("specularOn", specularOn);
	m_Shader->SetUniform1f("kd", kd);
	m_Shader->SetUniform1f("ks", ks);
	m_Shader->SetUniform1f("kt", kt);
	m_Shader->SetUniform1f("km", km);
	m_Shader->SetUniformMat4fv("view", camera->m_ViewMatrix);
	m_Shader->SetUniformMat4fv("projection", camera->m_ProjectionMatrix);
	m_Shader->SetUniform3f("eyePos", camera->m_Position);
	m_Shader->SetUniform1i("numLights", numLights);
	m_Shader->SetUniform1i("Cubemap", 0);
	m_Mesh->Draw();
}

void Object::Load()
{
	m_Shader = std::make_unique<Shader>("assets/shaders/objectvert.glsl", "assets/shaders/lightprobefrag.glsl");
	m_Mesh = std::make_unique<Mesh>("assets/model/bunny.obj");
}

void Object::changeShader(int i)
{
	switch (i)
	{
	case 1:
		m_Shader->Load("assets/shaders/objectvert.glsl", "assets/shaders/lightprobefrag.glsl");
		break;
	case 2:
		m_Shader->Load("assets/shaders/objectvert.glsl", "assets/shaders/mirrorfrag.glsl");
		break;
	case 3:
		m_Shader->Load("assets/shaders/objectvert.glsl", "assets/shaders/glassfrag.glsl");
		break;
	case 4:
		m_Shader->Load("assets/shaders/objectvert.glsl", "assets/shaders/glossyfrag.glsl");
		break;
	case 5:
		m_Shader->Load("assets/shaders/objectvert.glsl", "assets/shaders/speculardiscofrag.glsl");
		break;
	}
}
