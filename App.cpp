#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include "App.h"

App::App()
{
    m_WindowState = WindowState(1000, 1000, "Skybox");
    lastPosition.x = 1000 / 2.0f;
    lastPosition.y = 1000 / 2.0f;
    newPosition = lastPosition;
    path = "assets/hdr/Thumersbach.hdr";
    //path = "assets/hdr/Snowstreet.hdr";
    //path = "assets/hdr/Test.hdr";
    //path = "assets/hdr/Beach.hdr";
    exposure = 0.18;
    specularOn = 1;
    filePath = path.c_str();
    kd = 1e-5f;
    ks = 1e-5f;
    kt = 1.0;
    km = 0.9;
    mode = 1;
    Init();
    PrepareScene();
}

App::~App()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();

}

void App::Init()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    if (!glfwInit())
    {
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(m_WindowState.width, m_WindowState.height, "Skybox", NULL, NULL);
    glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            App* app = (App*)glfwGetWindowUserPointer(window);
            if (action == GLFW_PRESS || action == GLFW_REPEAT)
            {
                switch (key)
                {
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, true);
                    break;
                case GLFW_KEY_1:
                    app->m_Object.changeShader(1);
                    app->mode = 1;
                    app->m_QuadShader.SetUniform1i("mode", app->mode);
                    break;
                case GLFW_KEY_2:
                    app->m_Object.changeShader(2);
                    app->mode = 2;
                    app->m_QuadShader.SetUniform1i("mode", app->mode);
                    break;
                case GLFW_KEY_3:
                    app->m_Object.changeShader(3);
                    app->mode = 3;
                    app->m_QuadShader.SetUniform1i("mode", app->mode);
                    break;
                case GLFW_KEY_4:
                    app->m_Object.changeShader(4);
                    app->mode = 4;
                    app->m_QuadShader.SetUniform1i("mode", app->mode);
                    break;
                case GLFW_KEY_5:
                    app->m_Object.changeShader(5);
                    app->mode = 5;
                    app->m_QuadShader.SetUniform1i("mode", app->mode);
                    break;
                case GLFW_KEY_E:
                    app->m_lights.updateLevel(-1);
                    break;
                case GLFW_KEY_R:
                    app->m_lights.updateLevel(1);
                    break;
                case GLFW_KEY_W:
                    app->exposure *= 2;
                    app->m_QuadShader.SetUniform1f("exposure", app->exposure);
                    break;
                case GLFW_KEY_S:
                    app->exposure *= 0.5;
                    app->m_QuadShader.SetUniform1f("exposure", app->exposure);
                    break;
                case GLFW_KEY_F:
                    if (app->specularOn)
                         app->specularOn = 0;
                    else
                        app->specularOn = 1;
                    app->m_Object.m_Shader->SetUniform1i("specularOn", app->specularOn);
                    break;
                case GLFW_KEY_O:
                    app->kd = std::max(0.5f * app->kd, 0.000001f);
                    app->m_Object.m_Shader->SetUniform1f("kd", app->kd);
                    break;
                case GLFW_KEY_P:
                    app->kd *= 2.0;
                    app->m_Object.m_Shader->SetUniform1f("kd", app->kd);
                    break;
                case GLFW_KEY_K:
                    app->ks = std::max(0.5f * app->ks, 0.000001f);
                    app->m_Object.m_Shader->SetUniform1f("ks", app->ks);
                    break;
                case GLFW_KEY_L:
                    app->ks *= 2.0;
                    app->m_Object.m_Shader->SetUniform1f("ks", app->ks);
                    break;
                case GLFW_KEY_N:
                    if (app->mode == 3)
                    {
                        app->kt = std::max(app->kt - 0.05, 0.0);
                        app->m_Object.m_Shader->SetUniform1f("kt", app->kt);
                    }
                    else if (app->mode == 4)
                    {
                        app->km = std::max(app->km - 0.05, 0.0);
                        app->m_Object.m_Shader->SetUniform1f("km", app->km);
                    }
                    break;
                case GLFW_KEY_M:
                    if (app->mode == 3)
                    {
                        app->kt = std::min(app->kt + 0.05, 1.0);
                        app->m_Object.m_Shader->SetUniform1f("kt", app->kt);
                    }
                    else if (app->mode == 4)
                    {
                        app->km = std::min(app->km + 0.05, 1.0);
                        app->m_Object.m_Shader->SetUniform1f("km", app->km);
                    }
                    break;
                /*case GLFW_KEY_A:
                {
                    app->rotationSpeed = 1.0f;
                }
                case GLFW_KEY_D:
                {
                    app->rotationSpeed = 1.0f;
                }*/
                    

                default:
                    break;
                }
            }
        });
        

    if (!m_Window)
    {
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(m_Window);
    // glfwSwapInterval(1);

    // Initialize GLEW to setup the OpenGL Function pointers
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        exit(-1);
    }

    //char rendererInfo[512] = { 0 };
    //strcpy(rendererInfo, (const char*)glGetString(GL_RENDERER)); // Use strcpy_s on Windows, strcpy on Linux
    //strcat(rendererInfo, " - "); 
    //strcat(rendererInfo, (const char*)glGetString(GL_VERSION)); // Use strcpy_s on Windows, strcpy on Linux
    //glfwSetWindowTitle(m_Window, rendererInfo);
    glfwSetWindowUserPointer(m_Window, this);

    // glfw state
    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // GLFW Events
    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
        {
            glViewport(0, 0, width, height);
            App* app = (App*)glfwGetWindowUserPointer(window);
            app->m_WindowState.width = width;
            app->m_WindowState.height = height;
            // Update Projection matrix on resize
            app->m_Camera->UpdateProjectionMatrix(width, height);
        });

    // Mouse button callback
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(m_Window, [](GLFWwindow * window, double xposIn, double yposIn)
    {
        App* app = (App*)glfwGetWindowUserPointer(window);
        app->newPosition = glm::vec2(xposIn, yposIn);
    });
    
    initBuffers();
    initQuad();

}

void App::initBuffers()
{
    glGenFramebuffers(1, &screenFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);

    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_WindowState.width, m_WindowState.height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_WindowState.width, m_WindowState.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);    
}

void App::initQuad()
{
    float quadVertices[] = 
    {
        // pos   // texCoord
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    m_QuadShader.Load("assets/shaders/quadvert.glsl", "assets/shaders/quadfrag.glsl");
    
}

void App::PrepareScene()
{
    m_Camera = std::make_shared<Camera>(m_WindowState.width, m_WindowState.height);
    m_Object.Load();
    m_Skybox.SetupSkybox(filePath);
    m_lights.setupLights(filePath);
}

void App::Run()
{
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    double currentFrame = glfwGetTime();
    double lastFrame = currentFrame;
    double deltaTime;


    while (!glfwWindowShouldClose(m_Window))
    {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        

        glClearColor(0, 0, 0, 1);
        glClearDepth(1.0f);
        glClearStencil(0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        ProcessInput(deltaTime);


        Render();

        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }
}

void App::Render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);
    glEnable(GL_DEPTH_TEST); 


    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


    m_Skybox.Render(*m_Camera);
    m_Object.Render(m_Camera, pow(2, m_lights.level), specularOn, kd, ks, kt, km);


    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);


    glClear(GL_COLOR_BUFFER_BIT);


    m_QuadShader.Use();
    m_QuadShader.SetUniform1i("screenTexture", 0);
    m_QuadShader.SetUniform1f("exposure", exposure);
    m_QuadShader.SetUniform1i("mode", mode);
    /*int w = (int) (m_WindowState.width);
    //int h = (int)(m_WindowState.height);
    m_QuadShader.SetUniform2i("texSize", w, h);*/
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);

    glTextureBarrier();

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void App::ProcessInput(float deltaTime)
{
    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_Window, true);

    if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        float xoffset = newPosition.x - lastPosition.x;
        float yoffset = newPosition.y - lastPosition.y; //maybe inserve
        m_Camera->ProcessMouseMovement(xoffset, yoffset, deltaTime);
    }
    lastPosition = newPosition;
    if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS) 
    {
        float xoffset = 50.f;
        m_Camera->ProcessMouseMovement(xoffset, 0, deltaTime);
    }
    if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS) 
    {
        float xoffset = -50.f;
        m_Camera->ProcessMouseMovement(xoffset, 0, deltaTime);
    }
    
}