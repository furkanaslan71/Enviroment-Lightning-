#pragma once
#include "Light.h"


Light::Light() {}

Light::~Light()
{
    if (m_data)
    {
        stbi_image_free(m_data);
    }
}

void Light::setupLights(const char* filePath)
{
    readHDR(filePath);

    createSAT();
    
    //calculate all levels at once and store them in a suitable data structure
    //then alternate between levels with updateLevel
    allLights.resize(8);
    Region region;

    medianCut(0, &region);
    setupLightingUBO(allLights[1]);
    for (auto i : allLights[0])
    {
        std::cout << i.color.x << " " << i.color.y << " " << i.color.z << " " << i.intensity << " " << i.position.x << " " << i.position.y << std::endl;
    }

}

void Light::readHDR(const char* filePath)
{
    int nrChannels;
    try
    {
        //stbi_set_flip_vertically_on_load(true);

        m_data = stbi_loadf(filePath, &width, &height, &nrChannels, 0);

        if (m_data == nullptr)
        {
            std::cerr << "Failed to load HDR image." << std::endl;
            return;
        }

    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::HDR_IMAGE::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
}

void Light::createSAT()
{
    SAT.resize(height);
    for (int i = 0; i < height; i++)
    {
        SAT[i].resize(width, 0.0f);
    }
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = y * width + x;
            float luminance = 0.2125f * m_data[3 * idx] + 0.7154f * m_data[3 * idx + 1] + 0.0721f * m_data[3 * idx + 2];
            luminance = cos(     M_PI/2.0f   *    abs(y - height / 2) / (height/2)    ) * luminance;
            SAT[y][x] = luminance + (x > 0 ? SAT[y][x-1] : 0) + (y > 0 ? SAT[y-1][x] : 0) - (x > 0 && y > 0 ? SAT[y-1][x-1] : 0);
        }
    }
}

void Light::medianCut(int depth, Region* region)
{
    if (depth > MAX_DEPTH)
    {
        return;
    }
    if (depth == 0)
    {
        //this means this is the first so I set the start and end manually
        //except this level, all levels' regions' start and end will be set
        //by the parent region before the medianCut is called for them
        region->start = glm::vec2(0, 0);
        region->end = glm::vec2(width - 1, height - 1);
    }
    int x0 = region->start.x, y0 = region->start.y, x1 = region->end.x, y1 = region->end.y;

    region->energy = SAT[y1][x1]
                    - (x0 > 0 ? SAT[y1][x0 - 1] : 0)
                    - (y0 > 0 ? SAT[y0 - 1][x1] : 0)
                    + (x0 > 0 && y0 > 0 ? SAT[y0 - 1][x0 - 1] : 0);

    float halfEnergy = region->energy / 2.0f;
    //if energy is zero on some rows or columns, there will maybe occur problems
    int halfX = x0;
    
    for ( ; halfX <= x1; halfX++)
    {
        float currentEnergy = SAT[y1][halfX]
                            - (x0 > 0 ? SAT[y1][x0 - 1] : 0)
                            - (y0 > 0 ? SAT[y0 - 1][halfX] : 0)
                            + (x0 > 0 && y0 > 0 ? SAT[y0 - 1][x0 - 1] : 0);
        if (currentEnergy >= halfEnergy)
        {
            break;
        }
    }
    int halfY = y0;
    for (; halfY <= y1; halfY++)
    {
        float currentEnergy = SAT[halfY][x1]
                            - (x0 > 0 ? SAT[halfY][x0 - 1] : 0)
                            - (y0 > 0 ? SAT[y0 - 1][x1] : 0)
                            + (x0 > 0 && y0 > 0 ? SAT[y0 - 1][x0 - 1] : 0);

        if (currentEnergy >= halfEnergy)
        {
            break;
        }
    }
    region->centroid = glm::vec2(halfX, halfY);
    LightSource light;
    light.position = glm::vec2(halfX / float(width), halfY / float(height));

    glm::vec3 colorSum = glm::vec3(0.0f);
    for (int y = y0; y <= y1; y++)
    {
        for (int x = x0; x <= x1; x++)
        {
            int idx = y * width + x;
            float cosWeight = cos(M_PI / 2.0f * abs(y - height / 2) / (height / 2));
            colorSum += cosWeight * glm::vec3(m_data[3 * idx], m_data[3 * idx + 1], m_data[3 * idx + 2]);
        }
    }

    float area = (float)((x1 - x0 + 1) * (y1 - y0 + 1));


    //light.color = (area > 0) ? colorSum / area : glm::vec3(0.0f);
    light.color = colorSum;

    light.intensity = (area > 0) ? area : 0.0f;
    allLights[depth].push_back(light);

    if (depth == MAX_DEPTH)
    {
        return;
    }


    {
    /*for (size_t i = 0; i < width * height * 3; ++i)
        conv[i] = static_cast<unsigned char>(m_data[i] * 255);*/

    // Draw regions 
    
        //glm::vec2 centroid = region->centroid;
        //glm::vec3 color = light.color;
        //color = glm::normalize(color);


        //int x = centroid.x;
        //int y = centroid.y;
        //// draw a cross
        //for (int i = -3; i <= 3; i++)
        //{
        //    conv[(y + i) * width * 3 + x * 3] = 255 * color.r;
        //    conv[(y + i) * width * 3 + x * 3 + 1] = 255 * color.g;
        //    conv[(y + i) * width * 3 + x * 3 + 2] = 255 * color.b;

        //    conv[y * width * 3 + (x + i) * 3] = 0;
        //    conv[y * width * 3 + (x + i) * 3 + 1] = 0;
        //    conv[y * width * 3 + (x + i) * 3 + 2] = 255;
        //}

        //for (int x = x0; x < x1; x++)
        //{
        //    conv[y0 * width * 3 + x * 3] = 0;
        //    conv[y0 * width * 3 + x * 3 + 1] = 0;
        //    conv[y0 * width * 3 + x * 3 + 2] = 255;

        //    conv[(y1) * width * 3 + x * 3] = 0;
        //    conv[(y1) * width * 3 + x * 3 + 1] = 0;
        //    conv[(y1) * width * 3 + x * 3 + 2] = 255;

        //}
        //for (int y = y0; y < y1; y++)
        //{
        //    conv[y * width * 3 + x0 * 3 + 2] = 255;
        //}

        //stbi_write_bmp("test.bmp", width, height, 3, &conv[0]);
    }

    region->LeftSub = new Region();
    region->RightSub = new Region();

    if (x1 - x0 > y1 - y0)
    {
        //longest dimension is x
        

        region->LeftSub->start = glm::vec2(x0, y0);
        region->LeftSub->end = glm::vec2(halfX, y1);

        region->RightSub->start = glm::vec2(halfX + 1, y0);
        region->RightSub->end = glm::vec2(x1, y1);
   
    }
    else
    {
        //longest dimension is y
        

        region->LeftSub->start = glm::vec2(x0, y0);
        region->LeftSub->end = glm::vec2(x1, halfY);

        region->RightSub->start = glm::vec2(x0, halfY + 1);
        region->RightSub->end = glm::vec2(x1, y1);
    }

    medianCut(depth + 1, region->LeftSub);
   

    medianCut(depth + 1, region->RightSub);
    delete region->LeftSub;
    delete region->RightSub;
}

void Light::updateLevel(int num)
{
    //update level chnanges the level of the lights
    if ((level == 1 && num == -1) || (level == MAX_DEPTH && num == 1))
    {
        return;
    }
    level += num;
    setupLightingUBO(allLights[level]);
    std::cout << level << std::endl;
    for (auto i : allLights[level])
    {
        std::cout << i.color.x << " " << i.color.y << " " << i.color.z << " " << i.intensity << " " << i.position.x << " " << i.position.y << std::endl;
    }
}

void Light::setupLightingUBO(const std::vector<LightSource>& lights)
{
    glGenBuffers(1, &uboLights);
    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightSource) * lights.size(), lights.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, UBO_BINDING_POINT, uboLights);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}




