#ifndef SWORD_GROMETRY
#define SWORD_GROMETRY

#include <geometry/BufferGeometry.h>

using namespace std;

class SwordQiGeometry : public BufferGeometry {
public:
    SwordQiGeometry(float width = 1.0, float height = 1.0, float wSegment = 1.0, float hSegment = 1.0) {

        float width_half = width / 2.0f;
        float height_half = height / 2.0f;

        float gridX1 = wSegment + 1.0f;
        float gridY1 = hSegment + 1.0f;

        float segment_width = width / wSegment;
        float segment_height = height / hSegment;

        Vertex vertex;

        // generate Position Normal TexCoords
        Vertex vertex1, vertex2, vertex3;

        //plane 1
        vertex1.Position = glm::vec3(-height/2,-width/2,0.0f);
        vertex2.Position = glm::vec3(height/2, -width/2,0.0f);
        vertex3.Position = glm::vec3(0.0f, 0.0f, -width);

        vertex1.Normal = vertex2.Normal = vertex3.Normal = 
            glm::normalize(glm::cross(vertex2.Position - vertex1.Position, vertex3.Position - vertex1.Position));
        vertex1.TexCoords = glm::vec2(0,0);
        vertex2.TexCoords = glm::vec2(1,0);
        vertex3.TexCoords = glm::vec2(0.5,1);
        
        this->vertices.push_back(vertex1);
        this->vertices.push_back(vertex2);
        this->vertices.push_back(vertex3);

        // plane 2
        vertex1.Position = glm::vec3(height/2,-width/2,0.0f);
        vertex2.Position = glm::vec3(height/2, width/2,0.0f);
        vertex3.Position = glm::vec3(0.0f, 0.0f, -width);

        vertex1.Normal = vertex2.Normal = vertex3.Normal = 
            glm::normalize(glm::cross(vertex2.Position - vertex1.Position, vertex3.Position - vertex1.Position));
        vertex1.TexCoords = glm::vec2(0,0);
        vertex2.TexCoords = glm::vec2(1,0);
        vertex3.TexCoords = glm::vec2(0.5,1);
        

        this->vertices.push_back(vertex1);
        this->vertices.push_back(vertex2);
        this->vertices.push_back(vertex3);

        // plane 3
        vertex1.Position = glm::vec3(height/2,width/2,0.0f);
        vertex2.Position = glm::vec3(-height/2, width/2,0.0f);
        vertex3.Position = glm::vec3(0.0f, 0.0f, -width);

        vertex1.Normal = vertex2.Normal = vertex3.Normal = 
            glm::normalize(-glm::cross(vertex2.Position - vertex1.Position, vertex3.Position - vertex1.Position));
        vertex1.TexCoords = glm::vec2(0,0);
        vertex2.TexCoords = glm::vec2(1,0);
        vertex3.TexCoords = glm::vec2(0.5,1);
        

        this->vertices.push_back(vertex1);
        this->vertices.push_back(vertex2);
        this->vertices.push_back(vertex3);


        // plane 4
        vertex1.Position = glm::vec3(-height/2,width/2,0.0f);
        vertex2.Position = glm::vec3(-height/2, -width/2,0.0f);
        vertex3.Position = glm::vec3(0.0f, 0.0f, -width);

        vertex1.Normal = vertex2.Normal = vertex3.Normal = 
            glm::normalize(glm::cross(vertex2.Position - vertex1.Position, vertex3.Position - vertex1.Position));
        vertex1.TexCoords = glm::vec2(0,0);
        vertex2.TexCoords = glm::vec2(1,0);
        vertex3.TexCoords = glm::vec2(0.5,1);
        

        this->vertices.push_back(vertex1);
        this->vertices.push_back(vertex2);
        this->vertices.push_back(vertex3);

        for(int i = 0; i < 4*3; i+=3){
            this->indices.push_back(i);
            this->indices.push_back(i+1);
            this->indices.push_back(i+2);
        }



        this->setupBuffers();
    }
};

#endif