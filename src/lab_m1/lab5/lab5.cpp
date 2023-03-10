#include "lab_m1/lab5/lab5.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab5::Lab5()
{
    orthoLeft = -8.0f;
    orthoRight = 8.0f;
    orthoUp = 4.5f;
    orthoDown = -4.5;
    zfar = 200.0f;
    znear = 0.01f;
}
/* Divides the segment [A,B] into several segments */
void Lab5::points_between(VertexFormat Point_A, VertexFormat Point_B, vector<VertexFormat>& res, int point_num) {
    float interval_X = (Point_B.position.x - Point_A.position.x) / (point_num + 1);
    float interval_Z = (Point_B.position.z - Point_A.position.z) / (point_num + 1);
    for (int j = 0; j <= point_num; j++) {
        float newX = Point_A.position.x + interval_X * j;
        float newZ = Point_A.position.z + interval_Z * j;
        res.push_back(VertexFormat(glm::vec3(newX, 0, newZ), Point_A.color));
    }
    res.push_back(Point_B);
}

/* Creates a square of size x consisting of (k+1)^2 squares . This square represents the grass*/
Mesh* Lab5::create_grass(float x, int k, glm::vec3 color, std::string id) {
    VertexFormat Point_O = VertexFormat(glm::vec3(-x / 2, -0.01f, -x / 2), color);
    VertexFormat Point_A = VertexFormat(glm::vec3(x / 2, -0.01f, -x / 2), color);
    VertexFormat Point_B = VertexFormat(glm::vec3(-x / 2, -0.01f, x / 2), color);

    vector<VertexFormat> vec_OX;
    vector<VertexFormat> vec_OZ;
    vector<VertexFormat> vertices;
    points_between(Point_O, Point_A, vec_OX, k);
    points_between(Point_O, Point_B, vec_OZ, k);

    Mesh* grass = new Mesh(id);
    std::vector<unsigned int> indices;

    //Create each square consisting of 2 right triangles.
    for (int i = 1; i < vec_OZ.size(); i++) {
        vertices.push_back(VertexFormat(glm::vec3(vec_OX[0].position.x, -0.01f, vec_OZ[i - 1].position.z), color));
        vertices.push_back(VertexFormat(glm::vec3(vec_OX[0].position.x, -0.01f, vec_OZ[i].position.z), color));
        for (int j = 1; j < vec_OX.size(); j++) {
            vertices.push_back(VertexFormat(glm::vec3(vec_OX[j].position.x, -0.01f, vec_OZ[i - 1].position.z), color));
            vertices.push_back(VertexFormat(glm::vec3(vec_OX[j].position.x, -0.01f, vec_OZ[i].position.z), color));
            indices.push_back(vertices.size() - 4);
            indices.push_back(vertices.size() - 2);
            indices.push_back(vertices.size() - 1);
            indices.push_back(vertices.size() - 4);
            indices.push_back(vertices.size() - 3);
            indices.push_back(vertices.size() - 1);
        }
    }
    grass->SetDrawMode(GL_TRIANGLES);
    grass->InitFromData(vertices, indices);
    return grass;
}

/* Create a cube of size 1 */
Mesh* Lab5::create_cube(std::string id, glm::vec3 color)
{

    std::vector<VertexFormat> vertices =
    { VertexFormat(glm::vec3(-1, -1, -1), color),
    VertexFormat(glm::vec3(-1, -1, 1), color),
    VertexFormat(glm::vec3(1, -1, 1), color),
    VertexFormat(glm::vec3(1, -1, -1), color),

    VertexFormat(glm::vec3(-1, 1, -1), color),
    VertexFormat(glm::vec3(-1, 1, 1), color),
    VertexFormat(glm::vec3(1, 1, 1), color),
    VertexFormat(glm::vec3(1, 1, -1), color) };


    std::vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0,

    4, 5, 6, 6, 7, 4,

    0, 4, 7, 7, 3, 0,

    1, 5, 6, 6, 2, 1,

    1, 5, 4, 4, 0, 1,

    3, 7, 6, 6, 2, 3 };

    Mesh* cube = new Mesh(id);

    cube->InitFromData(vertices, indices);
    return cube;
}


/*creates 2 vectors whose segments are almost parallel to the segments in the original vector and each point is moved 
almost equal distance from the points in the original vector.*/
void Lab5::out_inner_vertices(
    vector<VertexFormat>& red_vertices,
    vector<VertexFormat>& blue_vertices,
    float road_size,
    vector<VertexFormat> vertex,
    glm::vec3 color
    ) {
    glm::vec3 up = glm::vec3(0, 1, 0);
    for (int i = 1; i < vertex.size() - 1; i++) {
        glm::vec3 D = vertex[i].position - vertex[i + 1].position;
        glm::vec3 P = glm::cross(D, up);
        P = glm::normalize(P);
        glm::vec3 red = vertex[i].position + road_size * P;
        glm::vec3 blue = vertex[i].position - road_size * P;
        VertexFormat R = VertexFormat(red, vertex[0].color);
        VertexFormat B = VertexFormat(blue, vertex[0].color);
        red_vertices.push_back(R);
        blue_vertices.push_back(B);
    }

};


/*Creates a new discretized vector. Between every 2 points in the vector, a finite number of points are created, 
thus discretizing the vector.*/
vector<VertexFormat> Lab5::create_more_points(
    vector<VertexFormat> vertices,
    int k
    ) {
    vector<VertexFormat> res;
    for (int i = 0; i < vertices.size() - 1; i++) {
        points_between(vertices[i], vertices[i + 1], res, k);

    }
    points_between(vertices[vertices.size() - 1], vertices[0], res, k);
    return res;
}

/*Just like the out_inner_vertices function, it creates a vector identical to the initial vector moved to a 
distance between the initial vector and the inner/out vector. The points of this vector will be the points on
which each bot will move. */
vector<vector<VertexFormat>> Lab5::create_bots_routes(
    vector<VertexFormat> vertex,
    int routes_nr,
    float road_size,
    int k,
    glm::vec3 color
) {
    vector<vector<VertexFormat>> res = vector<vector<VertexFormat>>(routes_nr);
    //Always creates an even number of routes
    if (routes_nr % 2 == 1)
        routes_nr++;
    float delta_road_size = road_size / (routes_nr + 1);

    for (int i = 0; i < routes_nr; i += 2) {
        out_inner_vertices(res[i], res[i + 1], ((i + 1) - (2 * i)) * delta_road_size, vertex, color);
        res[i] = create_more_points(res[i], k);
        res[i + 1] = create_more_points(res[i + 1], k);
    }
    return res;
}

/* Apply a scaling factor to bot routes. */
void Lab5::bot_roads_scale(float scale, vector<vector<VertexFormat>>& bots_roads,glm::vec3 color) {
    for (int i = 0; i < bots_roads.size(); i++) {
        for (int j = 0; j < bots_roads[i].size(); j++) {
            bots_roads[i][j] = VertexFormat(bots_roads[i][j].position * scale, color);
        }
    }
}


/* Check if a triangle can be built from the 3 given points. (if two or more points overlap, a triangle cannot be constructed) */
bool Lab5::check_triangles_points(vector<glm::vec3> triangle) {
    if (triangle[0] == triangle[1] || triangle[0] == triangle[2] || triangle[1] == triangle[2])
        return false;
    return true;
}

/* Creates triangles between the inner and outer vector points */
vector<vector<glm::vec3>> Lab5::create_triangles(
    vector<VertexFormat> red_vertices,
    vector<VertexFormat> blue_vertices
) {
    vector<vector<glm::vec3>> triangles;
    vector<glm::vec3> first_triangle;
    first_triangle.push_back(red_vertices[0].position);
    first_triangle.push_back(blue_vertices[0].position);
    first_triangle.push_back(blue_vertices[1].position);
    if(check_triangles_points(first_triangle))
        triangles.push_back(first_triangle);

    for (int i = 1; i < red_vertices.size() - 1; i++) {
        vector<glm::vec3> down_triangle;
        down_triangle.push_back(red_vertices[i].position);
        down_triangle.push_back(red_vertices[i - 1].position);
        down_triangle.push_back(blue_vertices[i].position);
        if (check_triangles_points(down_triangle))
            triangles.push_back(down_triangle);
        vector<glm::vec3> up_triangle;
        up_triangle.push_back(red_vertices[i].position);
        up_triangle.push_back(blue_vertices[i].position);
        up_triangle.push_back(blue_vertices[i + 1].position);
        if(check_triangles_points(up_triangle))
            triangles.push_back(up_triangle);
    }
    vector<glm::vec3> last_triangle;
    last_triangle.push_back(red_vertices[0].position);
    last_triangle.push_back(blue_vertices[0].position);
    last_triangle.push_back(red_vertices[red_vertices.size() - 1].position);
    if(check_triangles_points(last_triangle))
        triangles.push_back(last_triangle);
    return triangles;
}

Mesh* Lab5::create_road(
    vector<VertexFormat>& red_vertices,
    vector<VertexFormat>& blue_vertices,
    vector<VertexFormat>& vertex,
    vector<vector<glm::vec3>>& triangles,
    glm::vec3 color,
    bool fill
) {
    vertex = {
        VertexFormat(glm::vec3(-3,0,3),color),//A
        VertexFormat(glm::vec3(-2,0,4),color),//B
        VertexFormat(glm::vec3(-1,0,4),color),//C
        VertexFormat(glm::vec3(0,0,4),color),//D
        VertexFormat(glm::vec3(1,0,4),color),//E
        VertexFormat(glm::vec3(2,0,4),color),//F
        VertexFormat(glm::vec3(3,0,4),color),//G
        VertexFormat(glm::vec3(4,0,4),color),//H
        VertexFormat(glm::vec3(5,0,3),color),//I
        VertexFormat(glm::vec3(6,0,3),color),//J
        VertexFormat(glm::vec3(7,0,3),color),//K
        VertexFormat(glm::vec3(8,0,2),color),//L
        VertexFormat(glm::vec3(8,0,1),color),//M
        VertexFormat(glm::vec3(7,0,0),color),//N
        VertexFormat(glm::vec3(7,0,-1),color),//O
        VertexFormat(glm::vec3(6,0,-2),color),//P
        VertexFormat(glm::vec3(5,0,-2),color),//Q
        VertexFormat(glm::vec3(4,0,-2),color),//R
        VertexFormat(glm::vec3(3,0,-1),color),//S
        VertexFormat(glm::vec3(2,0,0),color),//T
        VertexFormat(glm::vec3(1,0,1),color),//U
        VertexFormat(glm::vec3(0,0,1),color),//V
        VertexFormat(glm::vec3(-1,0,0),color),//W
        VertexFormat(glm::vec3(-2,0,-1),color),//Z
        VertexFormat(glm::vec3(-3,0,-2),color),//A1
        VertexFormat(glm::vec3(-4,0,-2),color),//B1
        VertexFormat(glm::vec3(-5,0,-1),color),//C1
        VertexFormat(glm::vec3(-6,0,0),color),//D1
        VertexFormat(glm::vec3(-6,0,1),color),//E1
        VertexFormat(glm::vec3(-5,0,2),color),//F1
        VertexFormat(glm::vec3(-4,0,3),color),//G1
        VertexFormat(glm::vec3(-3,0,3),color),//A
        VertexFormat(glm::vec3(-2,0,4),color),//B
    };
    vector<VertexFormat> aux = vertex;
    out_inner_vertices(red_vertices, blue_vertices, road_size, aux,color);
    Mesh* road = new Mesh("road");
    std::vector<unsigned int> indices;
   

    if (!fill) {
        for (int i = 0; i < red_vertices.size(); i++) {
            vertex.push_back(red_vertices[i]);
        }

        for (int i = 0; i < blue_vertices.size(); i++) {
            vertex.push_back(blue_vertices[i]);
        }
        for (int i = 0; i < vertex.size(); i++) {
            indices.push_back(i);
        }
        road->SetDrawMode(GL_LINE_LOOP);
        road->InitFromData(vertex, indices);
    }
    else {
        vector<VertexFormat> aux;
        red_vertices = create_more_points(red_vertices, discretization_road);
        blue_vertices = create_more_points(blue_vertices, discretization_road);
        triangles = create_triangles(red_vertices, blue_vertices);
        for (int i = 0; i < red_vertices.size(); i++) {
            aux.push_back(red_vertices[i]);
            aux.push_back(blue_vertices[i]);
        }
        for (int i = 0; i < aux.size(); i++) {
            indices.push_back(i);
        }
        road->SetDrawMode(GL_TRIANGLE_STRIP);
        road->InitFromData(aux, indices);

    }
    
    return road;

}

/* The direction of the vector after points a and b */
float Lab5::vec_angle(glm::vec3 a, glm::vec3 b) {
    float delta_X = b.x - a.x;
    float delta_Y = b.z - a.z;
    return -atan2(delta_Y, delta_X) + (M_PI_2);
}

/* Randomly choose the initial positions of the bots */
vector<int> Lab5::create_bots_indices_position(vector<vector<VertexFormat>> bots_roads) {
    vector<int> res;
    for (int i = 0; i < bots_roads.size(); i++) {
        res.push_back(rand() % (bots_roads[i].size() - 1));
    }
    return res;
}

/* Check if player and bot hitboxes intersect. */
bool Lab5::intersect(glm::vec3 player_position, glm::vec3 bot_position) {
    float distance = sqrt(
        (player_position.x - bot_position.x) * (player_position.x - bot_position.x) +
        (player_position.y - bot_position.y) * (player_position.y - bot_position.y) +
        (player_position.z - bot_position.z) * (player_position.z - bot_position.z)
    );
    return distance < sphere_radius - 0.8f;
}


/* Create the trees at a distance slightly greater than that of inner/outer */
vector<glm::mat4> Lab5::create_trees() {
    vector<VertexFormat> out_trees, inner_trees;
    vector<glm::mat4> trees_model_matrix;
    out_inner_vertices(inner_trees,out_trees, 1.5 * road_size, vertex,vertex[0].color);
    for (int i = 0; i < out_trees.size(); i++) {
        
        glm::mat4 modelMatrix_tree = glm::mat4(1);
        modelMatrix_tree = glm::translate(modelMatrix_tree, glm::vec3(out_trees[i].position.x * road_scale,3 * stalk_size, out_trees[i].position.z * road_scale));
        modelMatrix_tree = glm::scale(modelMatrix_tree, glm::vec3(stalk_size, 3 * stalk_size, stalk_size));

        glm::mat4 modelMatrix_leaf = glm::mat4(1);
        modelMatrix_leaf = glm::translate(modelMatrix_leaf, glm::vec3(out_trees[i].position.x * road_scale, 7 * stalk_size, out_trees[i].position.z * road_scale));
        modelMatrix_leaf = glm::scale(modelMatrix_leaf, glm::vec3(3 * stalk_size, 3 * stalk_size, 3 * stalk_size));

        trees_model_matrix.push_back(modelMatrix_tree);
        trees_model_matrix.push_back(modelMatrix_leaf);
    }

    for (int i = 0; i < inner_trees.size(); i++) {
        glm::mat4 modelMatrix_tree = glm::mat4(1);
        modelMatrix_tree = glm::translate(modelMatrix_tree, glm::vec3(inner_trees[i].position.x * road_scale, 3 * stalk_size, inner_trees[i].position.z * road_scale));
        modelMatrix_tree = glm::scale(modelMatrix_tree, glm::vec3(stalk_size, 3 * stalk_size, stalk_size));

        glm::mat4 modelMatrix_leaf = glm::mat4(1);
        modelMatrix_leaf = glm::translate(modelMatrix_leaf, glm::vec3(inner_trees[i].position.x * road_scale, 7 * stalk_size, inner_trees[i].position.z * road_scale));
        modelMatrix_leaf = glm::scale(modelMatrix_leaf, glm::vec3(3 * stalk_size, 3 * stalk_size, 3 * stalk_size));

        trees_model_matrix.push_back(modelMatrix_tree);
        trees_model_matrix.push_back(modelMatrix_leaf);
    }



    return trees_model_matrix;

}

Lab5::~Lab5()
{
}

void Lab5::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::mat4 prj_mat, glm::mat4 view_mat)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // TODO(student): Get shader location for uniform mat4 "Model"
    int location = glGetUniformLocation(shader->GetProgramID(), "Model");

    // TODO(student): Set shader uniform "Model" to modelMatrix
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // TODO(student): Get shader location for uniform mat4 "View"
    int viewLocation = glGetUniformLocation(shader->GetProgramID(), "View");

    // TODO(student): Set shader uniform "View" to viewMatrix
    glm::mat4 viewMatrix = view_mat;
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // TODO(student): Get shader location for uniform mat4 "Projection"
    int projectionLocation = glGetUniformLocation(shader->GetProgramID(), "Projection");

    // TODO(student): Set shader uniform "Projection" to projectionMatrix
    glm::mat4 projectionMatrix = prj_mat;
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    location = glGetUniformLocation(shader->GetProgramID(), "Car");
    glm::vec4 car_position = projectionMatrix * viewMatrix * modelMatrix_car * glm::vec4(0, 0, 0, 1);
    glUniform4fv(location, 1, glm::value_ptr(car_position));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}




void Lab5::Init()
{
    renderCameraTarget = false;
    fov = 45;
    //projectionMatrix = glm::perspective(fov, 2.f, 2.f, 200.0f);
    camera = new implemented::Camera();
    camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    position = glm::vec3(1, 0.05f, 2.5f);
    camera->RotateFirstPerson_OY(M_PI / 2);
    angle += M_PI / 2;
    camera->TranslateUpward(-1.6);
    camera->TranslateRight(2);
    camera->TranslateForward(0.5);
    

    Mesh* car = create_cube("box", glm::vec3(0, 0, 1));
    AddMeshToList(car);

    Mesh* bot = create_cube("bot", glm::vec3(1, 0, 0));
    AddMeshToList(bot);

    Mesh* road = create_road(red_vertices, blue_vertices,vertex,triangles, glm::vec3(1,1,1), true);
    AddMeshToList(road);

    Mesh* grass = create_grass(50, 500, glm::vec3(0, 0.5f, 0),"grass");
    AddMeshToList(grass);

    Mesh* grass_minimap = create_grass(50, 500, glm::vec3(0, 0, 0), "grass_minimap");
    AddMeshToList(grass_minimap);


    Mesh* tree = create_cube("tree", NormalizedRGB(105, 75, 55));
    AddMeshToList(tree);

    Mesh* leaf = create_cube("leaf", NormalizedRGB(209, 76, 3));
    AddMeshToList(leaf);

    trees_model_matrix = create_trees();

    bots_roads = create_bots_routes(vertex, bots_number, road_size, discretization_road,glm::vec3(1,0,0));
    bot_roads_scale(road_scale, bots_roads,glm::vec3(1,0,0));
    vector_indices = create_bots_indices_position(bots_roads);

    {
        Shader* shader = new Shader("Shader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "lab5", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "lab5", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, znear, zfar);
    isOrtho = false;
    isperspective = true;

}


void Lab5::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glm::vec3 sky = NormalizedRGB(135, 206, 235);
    glClearColor(sky[0], sky[1], sky[2], 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Lab5::Update(float deltaTimeSeconds)
{

    glm::mat4 roadMatrix = glm::mat4(1);
    roadMatrix = glm::scale(roadMatrix, glm::vec3(road_scale));
    RenderSimpleMesh(meshes["road"], shaders["Shader"],roadMatrix,projectionMatrix, camera->GetViewMatrix());
    RenderSimpleMesh(meshes["grass"], shaders["Shader"], glm::mat4(1), projectionMatrix, camera->GetViewMatrix());

    modelMatrix_car = glm::mat4(1);
    modelMatrix_car = glm::translate(modelMatrix_car, camera->GetTargetPosition());
    modelMatrix_car = glm::rotate(modelMatrix_car, angle, glm::vec3(0,1,0));
    modelMatrix_car = glm::scale(modelMatrix_car, glm::vec3(car_size,car_size,2*car_size));
    RenderSimpleMesh(meshes["box"], shaders["Shader"], modelMatrix_car, projectionMatrix, camera->GetViewMatrix());


    if (bot_index_position == bots_roads[0].size() - 1)
        bot_index_position = 0;
    
    float bot_angle;
    glm::vec3 a, b;
    for (int i = 0; i < bots_number; i++) {  
        if (vector_indices[i] == bots_roads[i].size()) {
            vector_indices[i] = 0;
        }
        if (vector_indices[i] == 0) {
            a = bots_roads[i][vector_indices[i]].position;
            b = bots_roads[i][vector_indices[vector_indices.size() - 1]].position;
        }
        else {
            a = bots_roads[i][vector_indices[i]].position;
            b = bots_roads[i][vector_indices[i] - 1].position;
        }
        bot_angle = vec_angle(a, b);
        glm::mat4 modelMatrix_bot = glm::mat4(1);
        glm::vec3 bot_translated_y = glm::vec3(bots_roads[i][vector_indices[i]].position.x, 0.05f, bots_roads[i][vector_indices[i]].position.z);
        modelMatrix_bot = glm::translate(modelMatrix_bot, bot_translated_y);
        modelMatrix_bot = glm::rotate(modelMatrix_bot, bot_angle, glm::vec3(0, 1, 0));
        modelMatrix_bot = glm::scale(modelMatrix_bot, glm::vec3(car_size, car_size, 2 * car_size));
        RenderSimpleMesh(meshes["bot"], shaders["Shader"], modelMatrix_bot, projectionMatrix, camera->GetViewMatrix());
        vector_indices[i]++;
    }
    bot_index_position++;

    for (int i = 0; i < trees_model_matrix.size(); i += 2) {
        RenderSimpleMesh(meshes["tree"], shaders["Shader"], trees_model_matrix[i], projectionMatrix, camera->GetViewMatrix());
        RenderSimpleMesh(meshes["leaf"], shaders["Shader"], trees_model_matrix[i + 1], projectionMatrix, camera->GetViewMatrix());
    }

    glViewport(0, 0, 300, 300);

    {//cout << "Pozitia: " << camera->GetTargetPosition() << endl;
        glm::mat4 projectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoDown, orthoUp, 0.0f, 200.0f);
        //glm::mat4 view = glm::lookAt(glm::vec3(camera->GetTargetPosition().x, 1, camera->GetTargetPosition().z), glm::vec3(camera->GetTargetPosition().x, 1, camera->GetTargetPosition().z) + glm::vec3(0,-1,0),glm::vec3(1,0,0));
        glm::mat4 view = glm::lookAt(glm::vec3(camera->GetTargetPosition().x, 10, camera->GetTargetPosition().z),
            glm::vec3(camera->GetTargetPosition().x, 10, camera->GetTargetPosition().z) +
            glm::vec3(0, -1, 0), glm::vec3(1, 0, 0));
        glm::mat4 roadMatrix = glm::mat4(1);
        roadMatrix = glm::scale(roadMatrix, glm::vec3(road_scale));
        RenderSimpleMesh(meshes["road"], shaders["Shader"], roadMatrix, projectionMatrix, view);
        RenderSimpleMesh(meshes["grass_minimap"], shaders["Shader"], glm::mat4(1), projectionMatrix, view);

        modelMatrix_car = glm::mat4(1);
        //modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix_car = glm::translate(modelMatrix_car, camera->GetTargetPosition());
        modelMatrix_car = glm::rotate(modelMatrix_car, angle, glm::vec3(0, 1, 0));
        modelMatrix_car = glm::scale(modelMatrix_car, glm::vec3(car_size, car_size, 2 * car_size));
        RenderSimpleMesh(meshes["box"], shaders["Shader"], modelMatrix_car, projectionMatrix, view);


        if (bot_index_position == bots_roads[0].size() - 1)
            bot_index_position = 0;

        float bot_angle;
        glm::vec3 a, b;
        for (int i = 0; i < bots_number; i++) {
            if (vector_indices[i] == bots_roads[i].size()) {
                vector_indices[i] = 0;
            }
            if (vector_indices[i] == 0) {
                a = bots_roads[i][vector_indices[i]].position;
                b = bots_roads[i][vector_indices[vector_indices.size() - 1]].position;
            }
            else {
                a = bots_roads[i][vector_indices[i]].position;
                b = bots_roads[i][vector_indices[i] - 1].position;
            }
            bot_angle = vec_angle(a, b);
            glm::mat4 modelMatrix_bot = glm::mat4(1);
            glm::vec3 bot_translated_y = glm::vec3(bots_roads[i][vector_indices[i]].position.x, 0.05f, bots_roads[i][vector_indices[i]].position.z);
            modelMatrix_bot = glm::translate(modelMatrix_bot, bot_translated_y);
            modelMatrix_bot = glm::rotate(modelMatrix_bot, bot_angle, glm::vec3(0, 1, 0));
            modelMatrix_bot = glm::scale(modelMatrix_bot, glm::vec3(car_size, car_size, 2 * car_size));
            RenderSimpleMesh(meshes["bot"], shaders["Shader"], modelMatrix_bot, projectionMatrix, view);
            vector_indices[i]++;
        }
        bot_index_position++;

        for (int i = 0; i < trees_model_matrix.size(); i += 2) {
            RenderSimpleMesh(meshes["tree"], shaders["Shader"], trees_model_matrix[i], projectionMatrix, view);
            RenderSimpleMesh(meshes["leaf"], shaders["Shader"], trees_model_matrix[i + 1], projectionMatrix, view);
        }
    }

}


void Lab5::FrameEnd()
{
    //DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

bool Lab5::SameSide(glm::vec3 p1, glm::vec3 p2, glm::vec3 a, glm::vec3 b) {
    glm::vec3 cp1 = glm::cross(b - a, p1 - a);
    glm::vec3 cp2 = glm::cross(b - a, p2 - a);
    if (glm::dot(cp1, cp2) >= 0)
        return true;
    else
        return false;
}

bool Lab5::PointInTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    if (SameSide(p, a, b, c) && SameSide(p, b, a, c) && SameSide(p, c, a, b))
        return true;
    else
        return false;
    
}


void Lab5::OnInputUpdate(float deltaTime, int mods)
{
    // move the camera only if MOUSE_RIGHT button is pressed

    float cameraSpeed = 2.f;

        if (window->KeyHold(GLFW_KEY_W)) {
            // TODO(student): Translate the camera forward
            bool on_road = false;
            glm::vec3 target_position = glm::vec3(camera->GetTargetPosition().x, 0, camera->GetTargetPosition().z);
            glm::mat4 model = glm::translate(glm::mat4(1), target_position);
            model = glm::rotate(model, angle, glm::vec3(0, 1, 0));
            model = glm::translate(model, glm::vec3(0, 0, -deltaTime * cameraSpeed));
            target_position = model * glm::vec4(0, 0, 0, 1);

            for (int i = 0; i < triangles.size(); i++) {
                if (PointInTriangle(target_position, triangles[i][0] * road_scale, triangles[i][1] * road_scale, triangles[i][2] * road_scale)) {
                    on_road = true;
                    break;
                }

            }
            if (on_road) {
                bool bot_hit = false;
                for (int i = 0; i < vector_indices.size(); i++) {
                    if (intersect(camera->GetTargetPosition(), bots_roads[i][vector_indices[i] - 1].position)) {
                        bot_hit = true;
                        break;
                    }

                }
                if(!bot_hit)
                    camera->MoveForward(deltaTime * cameraSpeed);
            }
                
            
            
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            // TODO(student): Translate the camera to the left
            angle += deltaTime * cameraSpeed;
            camera->RotateThirdPerson_OY(deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            // TODO(student): Translate the camera backward
            //camera->MoveForward(-deltaTime * cameraSpeed);

            bool on_road = false;
            glm::vec3 target_position = glm::vec3(camera->GetTargetPosition().x, 0, camera->GetTargetPosition().z);
            glm::mat4 model = glm::translate(glm::mat4(1), target_position);
            model = glm::rotate(model, angle, glm::vec3(0, 1, 0));
            model = glm::translate(model, glm::vec3(0, 0, deltaTime * cameraSpeed));
            target_position = model * glm::vec4(0, 0, 0, 1);

            for (int i = 0; i < triangles.size(); i++) {
                if (PointInTriangle(target_position, triangles[i][0] * road_scale, triangles[i][1] * road_scale, triangles[i][2] * road_scale)) {
                    on_road = true;
                    break;
                }

            }
            if (on_road) {
                bool bot_hit = false;
                for (int i = 0; i < vector_indices.size(); i++) {
                    if (intersect(camera->GetTargetPosition(), bots_roads[i][vector_indices[i] - 1].position)) {
                        bot_hit = true;
                        break;
                    }

                }
                if (!bot_hit)
                    camera->MoveForward(-deltaTime * cameraSpeed);
            }
        }

        if (window->KeyHold(GLFW_KEY_D)) {
            // TODO(student): Translate the camera to the right
            angle -= deltaTime * cameraSpeed;
            camera->RotateThirdPerson_OY(-deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            // TODO(student): Translate the camera downward
            if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
                camera->TranslateUpward(-deltaTime * cameraSpeed);
            }
        }

        if (window->KeyHold(GLFW_KEY_E)) {
            // TODO(student): Translate the camera upward
            if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
                camera->TranslateUpward(deltaTime * cameraSpeed);
            }
        }
        if (window->KeyHold(GLFW_KEY_P)) {
            isOrtho = false;
            isperspective = true;
            cout << isperspective <<endl;
            projectionMatrix = glm::perspective(fov, window->props.aspectRatio, 0.01f, 200.0f);
        }

        if (window->KeyHold(GLFW_KEY_M) && isperspective) {
            fov+=deltaTime;
            projectionMatrix = glm::perspective(fov, window->props.aspectRatio, 0.01f, 200.0f);
        }
        // FOV normal
        if (window->KeyHold(GLFW_KEY_N) && isperspective)
        {
            fov-=deltaTime;
            projectionMatrix = glm::perspective(fov, window->props.aspectRatio, 0.01f, 200.0f);
        }

        if (window->KeyHold(GLFW_KEY_O)) {
            projectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoDown, orthoUp, 0.01f, 200.0f);
            isOrtho = true;
            isperspective = false;
        }
        // increase height
        if (window->KeyHold(GLFW_KEY_DOWN) && isOrtho) {
            orthoUp -= 2 * deltaTime;
            projectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoDown, orthoUp, 0.01f, 200.0f);
        }

        // increase length
        if (window->KeyHold(GLFW_KEY_RIGHT) && isOrtho) {
            orthoLeft -= 2 * deltaTime;
            projectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoDown, orthoUp, 0.01f, 200.0f);
        }

        // decrease length
        if (window->KeyHold(GLFW_KEY_LEFT) && isOrtho) {
            orthoLeft += 2 * deltaTime;
            projectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoDown, orthoUp, 0.01f, 200.0f);
        }
        if (window->KeyHold(GLFW_KEY_UP) && isOrtho) {
            orthoUp += 2 * deltaTime;
            projectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoDown, orthoUp, 0.01f, 200.0f);
        }
       

    // TODO(student): Change projection parameters. Declare any extra
    // variables you might need in the class header. Inspect this file
    // for any hardcoded projection arguments (can you find any?) and
    // replace them with those extra variables.

}


void Lab5::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_T)
    {
        renderCameraTarget = !renderCameraTarget;
    }
    // TODO(student): Switch projections

}


void Lab5::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab5::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0) {
            renderCameraTarget = false;
            // TODO(student): Rotate the camera in first-person mode around
            // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
            // variables for setting up the rotation speed.
            camera->RotateFirstPerson_OY(-deltaX * sensivityOX);
            camera->RotateFirstPerson_OX(-deltaY * sensivityOY);
        }

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
            renderCameraTarget = true;
            // TODO(student): Rotate the camera in third-person mode around
            // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
            // variables for setting up the rotation speed.
            camera->RotateThirdPerson_OY(-deltaX * sensivityOX);
            camera->RotateThirdPerson_OX(-deltaY * sensivityOY);
        }
    }
}


void Lab5::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab5::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab5::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Lab5::OnWindowResize(int width, int height)
{
}
