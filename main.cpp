#include <SDL2/SDL.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>


struct vec3 {
    float x, y, z;
};

struct triangle {
    vec3 v[3];
};

struct mesh {
    std::vector<triangle> msh;


bool LoadOBJFile(std::string filename) {
    std::ifstream f(filename);
    if (!f.is_open()){
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }
    std::vector<vec3> verts;

    while (!f.eof()) {
        std::string line;
        std::getline(f, line);

        std::istringstream s(line);
        char ignore;

        
        if (line[0] == 'v' && line[1] == ' ') {
            vec3 vrts;
            s >> ignore >> vrts.x >> vrts.y >> vrts.z;
            verts.push_back(vrts);
        }

        if (line[0] == 'f' && (line[1] == ' ' || line[1] == '\t')) {
            int f[3];
            s >> ignore >> f[0] >> f[1] >> f[2];

            
            msh.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
        }
    }
    std::cout << "Loaded " << verts.size() << " vertices and " << msh.size() << " triangles." << std::endl;

    return true;
}

};

struct mat4x4 {
    float m[4][4] = { 0 };

    
};

class Engine3D {
public:
    Engine3D() : fNormTime(0.0f), CONSTCamera{ 0.0f, 0.0f, 0.0f } {

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            exit(1);
        }

        window = SDL_CreateWindow("3D Construct", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
        if (window == nullptr) {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            SDL_Quit();
            exit(1);
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == nullptr) {
            std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            SDL_Quit();
            exit(1);
        }

        float fArti = 0.1f;
        float fToli = 1000.0f;
        float fFov = 90.0f;
        float fApratio = 600.0f / 800.0f;
        float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

        matProj.m[0][0] = fApratio * fFovRad;
        matProj.m[1][1] = fFovRad;
        matProj.m[2][2] = fToli / (fToli - fArti);
        matProj.m[3][2] = (-fToli * fArti) / (fToli - fArti);
        matProj.m[2][3] = 1.0f;
        matProj.m[3][3] = 0.0f;

        RenderMesh();
    }

    ~Engine3D() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void Run() {
        bool quit = false;
        SDL_Event e;

        while (!quit) {
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                }
            }

            Update();
            Render();
            SDL_Delay(16); // 60 FPS
        }
    }

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    mesh meshRender;
    mat4x4 matProj;
    vec3 CONSTCamera;
    float fNormTime;


    //Hihi copied this function. Not writing or even try to understand this no no no
    void DrawFilledTriangle(SDL_Renderer* renderer, vec3 v1, vec3 v2, vec3 v3) {
    
    if (v1.y > v2.y) std::swap(v1, v2);
    if (v1.y > v3.y) std::swap(v1, v3);
    if (v2.y > v3.y) std::swap(v2, v3);

    int totalHeight = (int)(v3.y - v1.y);
    for (int i = 0; i < totalHeight; i++) {
        bool secondHalf = i > (v2.y - v1.y) || v2.y == v1.y;
        int segmentHeight = secondHalf ? (int)(v3.y - v2.y) : (int)(v2.y - v1.y);
        float alpha = (float)i / totalHeight;
        float beta = (float)(i - (secondHalf ? (v2.y - v1.y) : 0)) / segmentHeight;

        vec3 A = {
            v1.x + (v3.x - v1.x) * alpha,
            v1.y + (v3.y - v1.y) * alpha,
            0
        };
        vec3 B = secondHalf
            ? vec3{ v2.x + (v3.x - v2.x) * beta, v2.y + (v3.y - v2.y) * beta, 0 }
            : vec3{ v1.x + (v2.x - v1.x) * beta, v1.y + (v2.y - v1.y) * beta, 0 };

        if (A.x > B.x) std::swap(A, B);

        for (int j = (int)A.x; j <= (int)B.x; j++) {
            SDL_RenderDrawPoint(renderer, j, (int)(v1.y + i));
        }
    }
}


    void RenderMesh() {
        
        /*/
        //Cube vertices
        meshRender.msh.push_back({ { {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f} } });
        meshRender.msh.push_back({ { {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f} } });
        meshRender.msh.push_back({ { {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f} } });
        meshRender.msh.push_back({ { {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f} } });
        meshRender.msh.push_back({ { {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f} } });
        meshRender.msh.push_back({ { {1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} } });
        meshRender.msh.push_back({ { {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f} } });
        meshRender.msh.push_back({ { {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f} } });
        meshRender.msh.push_back({ { {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f} } });
        meshRender.msh.push_back({ { {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f} } });
        meshRender.msh.push_back({ { {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f} } });
        meshRender.msh.push_back({ { {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} } });
        /*/
        meshRender.LoadOBJFile("VideoShip.obj");
    }

    void MultiplyMatrixVector(const vec3& i, vec3& o, const mat4x4& m) {
        o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
        o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
        o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
        float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

        if (w != 0.0f) {
            o.x /= w; o.y /= w; o.z /= w;
        }
    }

    void Update(){
        fNormTime += 0.01f; 
    }

    void Render() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        mat4x4 matRotZ, matRotX;

        matRotZ.m[0][0] = cosf(fNormTime);
        matRotZ.m[0][1] = sinf(fNormTime);
        matRotZ.m[1][0] = -sinf(fNormTime);
        matRotZ.m[1][1] = cosf(fNormTime);
        matRotZ.m[2][2] = 1;
        matRotZ.m[3][3] = 1;

        matRotX.m[0][0] = 1;
        matRotX.m[1][1] = cosf(fNormTime * 0.5f);
        matRotX.m[1][2] = sinf(fNormTime * 0.5f);
        matRotX.m[2][1] = -sinf(fNormTime * 0.5f);
        matRotX.m[2][2] = cosf(fNormTime * 0.5f);
        matRotX.m[3][3] = 1;

        for (auto& tri : meshRender.msh) {
            triangle triProj, triTrans, triRotaZ, triRotaX;

            for (int i = 0; i < 3; i++) {
                MultiplyMatrixVector(tri.v[i], triRotaZ.v[i], matRotZ);
            }

            for (int i = 0; i < 3; i++) {
                MultiplyMatrixVector(triRotaZ.v[i], triRotaX.v[i], matRotX);
            }

            triTrans = triRotaX;
            for (int i = 0; i < 3; i++) {
                triTrans.v[i].z += 8.0f;  
            }

            vec3 norm, lne1, lne2;

            lne1.x = triTrans.v[1].x - triTrans.v[0].x;
            lne1.y = triTrans.v[1].y - triTrans.v[0].y;
            lne1.z = triTrans.v[1].z - triTrans.v[0].z;

            lne2.x = triTrans.v[2].x - triTrans.v[0].x;
            lne2.y = triTrans.v[2].y - triTrans.v[0].y;
            lne2.z = triTrans.v[2].z - triTrans.v[0].z;

            norm.x = lne1.y * lne2.z - lne1.z * lne2.y;
            norm.y = lne1.z * lne2.x - lne1.x * lne2.z;
            norm.z = lne1.x * lne2.y - lne1.y * lne2.x;

            float length = sqrtf(norm.x * norm.x + norm.y * norm.y + norm.z * norm.z);

            norm.x /= length; norm.y /= length; norm.z /= length;

            vec3 cameraRay;

            cameraRay.x = triTrans.v[0].x - CONSTCamera.x;
            cameraRay.y = triTrans.v[0].y - CONSTCamera.y;
            cameraRay.z = triTrans.v[0].z - CONSTCamera.z;

            float dotProduct = norm.x * cameraRay.x + norm.y * cameraRay.y + norm.z * cameraRay.z;

            if (dotProduct < 0.0f) {
                vec3 light_direct={ 0.0f, 0.0f, -1.0f};
            float lengthL= sqrtf(light_direct.x * light_direct.x + light_direct.y * light_direct.y + light_direct.z * light_direct.z);
            light_direct.x /= lengthL; light_direct.y /= lengthL; light_direct.z /= lengthL;

            float lightIntensity = norm.x * light_direct.x + norm.y * light_direct.y + norm.z * light_direct.z;

            if (lightIntensity < 0.0f) lightIntensity = 0.0f;
            if (lightIntensity > 1.0f) lightIntensity = 1.0f;

            
            SDL_Color lightColor = { 0, 0, 255};

            SDL_Color finalColor;
           finalColor.r = static_cast<Uint8>(lightColor.r * lightIntensity);
           finalColor.g = static_cast<Uint8>(lightColor.g * lightIntensity);
           finalColor.b = static_cast<Uint8>(lightColor.b * lightIntensity);


                for (int i = 0; i < 3; i++) {
                    MultiplyMatrixVector(triTrans.v[i], triProj.v[i], matProj);
                }

                for (int i = 0; i < 3; i++) {
                    triProj.v[i].x += 1.0f; triProj.v[i].y += 1.0f;
                    triProj.v[i].x *= 0.5f * 800;
                    triProj.v[i].y *= 0.5f * 600;
                }

                SDL_SetRenderDrawColor(renderer, finalColor.r, finalColor.g, finalColor.b, 255);
                DrawFilledTriangle(renderer, triProj.v[0], triProj.v[1], triProj.v[2]);
                /*/ 
                SDL_RenderDrawLine(renderer,
                    (int)triProj.v[0].x, (int)triProj.v[0].y,
                    (int)triProj.v[1].x, (int)triProj.v[1].y);
                SDL_RenderDrawLine(renderer,
                    (int)triProj.v[1].x, (int)triProj.v[1].y,
                    (int)triProj.v[2].x, (int)triProj.v[2].y);
                SDL_RenderDrawLine(renderer,
                    (int)triProj.v[2].x, (int)triProj.v[2].y,
                    (int)triProj.v[0].x, (int)triProj.v[0].y);
                /*/
            }
        }

        SDL_RenderPresent(renderer);
    }
};

int main() {
    Engine3D engine;
    engine.Run();
    return 0;
}

