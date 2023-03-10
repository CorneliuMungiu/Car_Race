#pragma once

#include "components/simple_scene.h"
#include "lab_m1/lab5/lab_camera.h"
#include <vector>

using namespace std;


namespace m1
{
    class Lab5 : public gfxc::SimpleScene
    {
    public:
        Lab5();
        ~Lab5();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        //void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;
        void Lab5::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::mat4 prj_mat, glm::mat4 view_mat);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;



    protected:
        implemented::Camera* camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;
        glm::mat4 oldprojectionMatrix;
        float fov;
        bool isOrtho = false;
        bool isperspective = false;
        float orthoRight = 8.0f, orthoUp = 4.5f, orthoDown = -4.5f, orthoLeft = -8.0f;
        // TODO(student): If you need any other class variables, define them here.
        float zfar;
        float znear;

        vector<VertexFormat> red_vertices;
        vector<VertexFormat> blue_vertices;
        vector<VertexFormat> vertex;
        glm::vec3 position;
        float car_size = 0.05f;
        float road_scale = 2.5f;
        glm::vec3 forward;
        vector<vector<glm::vec3>> triangles;
        float angle = 0;
        float car_speed = 0;
        int discretization_road = 200;
        float road_size = 0.2f;
        int bots_number = 20;
        int bot_index_position = 100;
        vector<vector<VertexFormat>> bots_roads;
        vector<int> vector_indices;
        int sphere_radius = 1.f;
        
        float stalk_size = 0.05f;
        vector<glm::mat4> trees_model_matrix;

        //TODO: Check this
        glm::mat4 modelMatrix_car = glm::mat4(1);
        float scale_factor = 0.01f;



        Mesh* create_road(
            vector<VertexFormat>& red_vertices,
            vector<VertexFormat>& blue_vertices,
            vector<VertexFormat>& vertex,
            vector<vector<glm::vec3>>& triangles,
            glm::vec3 color,
            bool fill
        );

        void out_inner_vertices(
            vector<VertexFormat>& red_vertices,
            vector<VertexFormat>& blue_vertices,
            float road_size,
            vector<VertexFormat> vertex,
            glm::vec3 color
        );

        vector<VertexFormat> create_more_points(
            vector<VertexFormat> vertices,
            int k
        );

        vector<vector<glm::vec3>> create_triangles(
            vector<VertexFormat> red_vertices,
            vector<VertexFormat> blue_vertices
        );
        bool Lab5::SameSide(glm::vec3 p1, glm::vec3 p2, glm::vec3 a, glm::vec3 b);
        bool Lab5::PointInTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);
        bool check_triangles_points(vector<glm::vec3> triangle);
        Mesh* Lab5::create_grass(float x, int k, glm::vec3 color, std::string id);
        void Lab5::points_between(VertexFormat Point_A, 
                                  VertexFormat Point_B, 
                                  vector<VertexFormat>& res,
                                  int point_num);

        vector<vector<VertexFormat>> Lab5::create_bots_routes(
            vector<VertexFormat> vertex,
            int routes_nr,
            float road_size,
            int k,
            glm::vec3 color
        );
        void Lab5::bot_roads_scale(float scale, vector<vector<VertexFormat>>& bots_roads,glm::vec3 color);
        float Lab5::vec_angle(glm::vec3 a, glm::vec3 b);
        vector<int> Lab5::create_bots_indices_position(vector<vector<VertexFormat>> bots_roads);
        bool Lab5::intersect(glm::vec3 player_position, glm::vec3 bot_position);
        Mesh* Lab5::create_cube(std::string id, glm::vec3 color);
        vector<glm::mat4> Lab5::create_trees();
        

    };
}   // namespace m1
