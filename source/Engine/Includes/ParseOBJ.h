#pragma once
#include <vector>
#include "Vertex.h"
#include "DataTypes.h"

inline std::vector<Vertex> parseOBJString(const std::string& objString) {
    std::vector<Vec3f> positions;
    std::vector<Vec3f> normals;
    std::vector<Vec2f> texcoords;
    std::vector<Vertex> vertices;

    std::istringstream stream(objString);
    std::string line;

    while (std::getline(stream, line)) {
        std::istringstream linestream(line);
        std::string type;
        linestream >> type;

        if (type == "v") {
            float x, y, z;
            linestream >> x >> y >> z;
            positions.push_back({x, y, z});
        } else if (type == "vt") {
            float u, v;
            linestream >> u >> v;
            texcoords.push_back({u, v});
        } else if (type == "vn") {
            float nx, ny, nz;
            linestream >> nx >> ny >> nz;
            normals.push_back({nx, ny, nz});
        } else if (type == "f") {
            std::string vertStr;
            for (int i = 0; i < 3; ++i) {
                linestream >> vertStr;
                int vIdx = 0, tIdx = 0, nIdx = 0;
                
                #pragma warning(disable : 4996) // For MSVC: disables "unsafe" CRT function warnings
                sscanf(vertStr.c_str(), "%d/%d/%d", &vIdx, &tIdx, &nIdx);

                Vertex v{};
                if (vIdx)  v.xyz = positions[vIdx - 1];
                if (nIdx)  v.nxyz = normals[nIdx - 1];
                if (tIdx) {
                    v.u = texcoords[tIdx - 1].x;
                    v.v = texcoords[tIdx - 1].y;
                }

                v.rgb = Vec3f{1.0f, 1.0f, 1.0f}; // Default white
                v.a = 1.0f;

                vertices.push_back(v);
            }
        }
    }

    return vertices;
}