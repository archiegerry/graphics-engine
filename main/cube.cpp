#include "cube.hpp"


SimpleMeshData make_cube(Vec3f aColor, Mat44f aPreTransform)
{
    std::vector<Vec3f> pos;
    std::vector<Vec3f> normals;

    // Define vertices for a cube
    std::vector<Vec3f> vertices = {
        {0.5f, 0.5f, 0.5f},   // Vertex 0 (Front-Top-Right)
        {-0.5f, 0.5f, 0.5f},  // Vertex 1 (Front-Top-Left)
        {-0.5f, -0.5f, 0.5f}, // Vertex 2 (Front-Bottom-Left)
        {0.5f, -0.5f, 0.5f},  // Vertex 3 (Front-Bottom-Right)
        {0.5f, 0.5f, -0.5f},  // Vertex 4 (Back-Top-Right)
        {-0.5f, 0.5f, -0.5f}, // Vertex 5 (Back-Top-Left)
        {-0.5f, -0.5f, -0.5f},// Vertex 6 (Back-Bottom-Left)
        {0.5f, -0.5f, -0.5f}  // Vertex 7 (Back-Bottom-Right)
    };

    // Define indices for the cube's triangles
    std::vector<std::vector<int>> indices = {
        {0, 1, 2, 2, 3, 0}, // Front face
        {4, 0, 3, 3, 7, 4}, // Right face
        {5, 4, 7, 7, 6, 5}, // Back face
        {1, 5, 6, 6, 2, 1}, // Left face
        {4, 5, 1, 1, 0, 4}, // Top face
        {3, 2, 6, 6, 7, 3}  // Bottom face
    };

    std::vector<Vec3f> faceNormals = {
        {0.f, 0.f, 1.f},   // Front face normal
        {1.f, 0.f, 0.f},   // Right face normal
        {0.f, 0.f, -1.f},  // Back face normal
        {-1.f, 0.f, 0.f},  // Left face normal
        {0.f, 1.f, 0.f},   // Top face normal
        {0.f, -1.f, 0.f}   // Bottom face normal
    };

    // Determine positions and normals
    for (int faceCounter = 0; faceCounter < indices.size(); faceCounter++) { 
        const auto& face = indices[faceCounter]; 
        for (int i = 0; i < face.size(); i++) { 
            pos.emplace_back(vertices[face[i]]); 
            normals.emplace_back(faceNormals[faceCounter]); 
        }
    }

    // Apply matrix pretransform
    for (size_t i = 0; i < pos.size(); ++i) {
        Vec4f p4{ pos[i].x, pos[i].y, pos[i].z, 1.f };  
        Vec4f t = aPreTransform * p4; 
        t /= t.w; 
        pos[i] = Vec3f{ t.x, t.y, t.z }; 

        // Transform normals (ignore translation)
        Vec4f n4{ normals[i].x, normals[i].y, normals[i].z, 1.f }; 
        Vec4f tn = aPreTransform * n4; 
        normals[i] = normalize(Vec3f{ tn.x, tn.y, tn.z }); 
    }

    SimpleMeshData cube;
    cube.positions = pos; // Set positions for the cube
    cube.colors = std::vector<Vec3f>(pos.size(), aColor);
    cube.normals = normals;
    return cube;
}
    

