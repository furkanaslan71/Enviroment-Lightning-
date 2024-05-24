#pragma once
#include <Skybox.h>


Skybox::Skybox(){}

void Skybox::SetupSkybox(const char* filePath)
{
    m_SkyboxCube.LoadFromOBJ("assets/model/cube.obj");
    m_SkyboxShader.Load("assets/shaders/skyboxVert.glsl", "assets/shaders/skyboxFrag.glsl");
	
    int width, height, nrChannels;
    try 
    {   
        //stbi_set_flip_vertically_on_load(true);

        m_data = stbi_loadf(filePath, &width, &height, &nrChannels, 0);

        if (m_data == nullptr) 
        {
            std::cerr << "Failed to load HDR image." << std::endl;
            return;
        }
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, m_data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(m_data);


    }
    catch (std::ifstream::failure& e) 
    {
        std::cout << "ERROR::HDR_IMAGE::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    glGenTextures(1, &cubeMapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
    unsigned int textureSize = 2000;
    for (unsigned int i = 0; i < 6; i++) 
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, textureSize, textureSize, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    Shader shader("assets/shaders/cubevert.glsl", "assets/shaders/cubefrag.glsl");
    Mesh cube;
    cube.LoadFromOBJ("assets/model/cube.obj");

   
    shader.Use();
    shader.SetUniformMat4fv("projection", glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 200.0f));
    shader.SetUniform1i("equirectangularMap", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glViewport(0, 0, textureSize, textureSize);

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);


    for (unsigned int i = 0; i < 6; i++)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMapTexture, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        
        //shader.SetUniformMat4fv("model", glm::mat4(1.0f));
        shader.SetUniformMat4fv("view", captureViews[i]);
        
        cube.Draw();

    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1000, 1000);
}


void Skybox::Render(Camera& camera)
{
    glDepthMask(GL_FALSE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
    glm::mat4 view = glm::mat4(glm::mat3(camera.m_ViewMatrix)); // Remove translation

    m_SkyboxShader.Use();
  
    m_SkyboxShader.SetUniformMat4fv("view", view);

    m_SkyboxShader.SetUniformMat4fv("projection", camera.m_ProjectionMatrix);
    m_SkyboxCube.Draw();
    glDepthMask(GL_TRUE);

}



