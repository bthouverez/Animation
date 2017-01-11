#include <iostream>
#include <chrono>

#include "window.h"

#include "mesh.h"
#include "wavefront.h"  // pour charger un objet au format .obj
#include "orbiter.h"

#include "draw.h"        // pour dessiner du point de vue d'une camera
#include <stdio.h>
#include "image_io.h"
#include "program.h"
#include "uniforms.h"

#include "app.h"        // classe Application a deriver
#include "text.h"

#include "camera.hpp"

// renvoie vrai si la boite englobante est au moins partiellement visible
bool visible( const Transform& mvp, const Point& pmin, const Point& pmax ) {
    int planes[6] = { };
    for(unsigned int i= 0; i < 8; i++) {
        Point p= pmin;
        if(i & 1) p.x= pmax.x;
        if(i & 2) p.y= pmax.y;
        if(i & 4) p.z= pmax.z;
        vec4 h= mvp(vec4(p));
        if(h.x < -h.w) planes[0]++;     // trop a gauche
        if(h.x > h.w) planes[1]++;      // trop a droite
        if(h.y < -h.w) planes[2]++;     // trop bas
        if(h.y > h.w) planes[3]++;      // trop haut
        if(h.z < -h.w) planes[4]++;     // trop pres
        if(h.z > h.w) planes[5]++;      // trop loin
    }
    for(unsigned int i= 0; i < 6; i++)
        if(planes[i] == 8)
            return false;       // la boite englobante n'est pas visible
    return true;
}


class Minecraft : public App
{
public:
    Minecraft() : App(1024, 640) {}

    int init( ){
        // Lecture de la heightmap
        height_map = read_image("src/minecraft/terrain.png");
        int coeff_height = 45;

        // Lecture du shader
        program = read_program("src/minecraft/BRDF.glsl");
        program_print_errors(program);

        cube = read_mesh("data/cube.obj");
        Point pmin, pmax;
        cube.bounds(pmin, pmax);

        // Camera 1 - "Personnage"
        double z = 5+coeff_height*height_map(96,96).r;
        current_pos = Point(96, z, 96);
        cam = b_Camera(current_pos, height_map, coeff_height);
        // Camera 2 - Point de vue exterieur - shadowmap
        cam2.lookat(Point(96, z, 96), 250);
        cam2.rotation(0.0, 90.0);
        lightPos = vec3(cam2.position());

        // Initialisation
        for(int ii = 0; ii < 9; ii++) {
            Mesh m(GL_TRIANGLES);
            mesh.push_back(m);
        }

        // Création d'un mesh par blocs
        for(int ii = 0; ii < height_map.width(); ii++)
        for(int jj = 0; jj < height_map.height(); jj++) {
            vec2 vt(ii, jj);

            int height = floor(coeff_height*height_map(ii, jj).r);
            int obj_size = pmax.x - pmin.x;
            int diff=1;

            // Regarde le voisinage (si il existe) et sauvegarde la plus grande différence pour empiler des cubes et combler les trous
            if(ii+1 < height_map.width())
                diff = std::max(diff, height - (int) floor(coeff_height*height_map(ii+1, jj  ).r));
            if(jj+1 < height_map.height())
                diff = std::max(diff, height - (int) floor(coeff_height*height_map(ii,   jj+1).r));
            if(jj+1 < height_map.height() && ii+1 < height_map.width())
                diff = std::max(diff, height - (int) floor(coeff_height*height_map(ii+1, jj+1).r));
            if(ii-1 > 0)
                diff = std::max(diff, height - (int) floor(coeff_height*height_map(ii-1, jj  ).r));
            if(jj-1 > 0)
                diff = std::max(diff, height - (int) floor(coeff_height*height_map(ii,   jj-1).r));
            if(ii-1 > 0 && jj-1 > 0)
                diff = std::max(diff, height - (int) floor(coeff_height*height_map(ii-1, jj-1).r));
            if(ii+1 < height_map.width() && jj-1 > 0)
                diff = std::max(diff, height - (int) floor(coeff_height*height_map(ii+1, jj-1).r));
            if(ii-1 > 0 && jj+1 < height_map.height())
                diff = std::max(diff, height - (int) floor(coeff_height*height_map(ii-1, jj+1).r));

            for(int cptdiff = 1; cptdiff < diff+1; cptdiff++) {
                // Ajoute un cube
                for(unsigned int kk=0; kk < cube.positions().size(); kk += 3) {
                    // Calcul de l'indice du mesh dans lequel rajouter ce triangle
                    int ind_mesh = floor(ii/64)+3*floor(jj/64);

                    vec3 v1 = cube.positions()[kk];
                    v1=vec3(v1.x+ii*obj_size, v1.y+height-cptdiff*obj_size, v1.z+jj*obj_size);
                    mesh[ind_mesh].texcoord(vt).normal(cube.normals()[kk]).vertex(v1);

                    vec3 v2 = cube.positions()[kk+1];
                    v2=vec3(v2.x+ii*obj_size, v2.y+height-cptdiff*obj_size, v2.z+jj*obj_size);
                    mesh[ind_mesh].texcoord(vt).normal(cube.normals()[kk+1]).vertex(v2);

                    vec3 v3 = cube.positions()[kk+2];
                    v3=vec3(v3.x+ii*obj_size, v3.y+height-cptdiff*obj_size, v3.z+jj*obj_size);
                    mesh[ind_mesh].texcoord(vt).normal(cube.normals()[kk+2]).vertex(v3);
                }
            }
        }

        // Création des boites englobantes
        for(unsigned int ii = 0; ii < mesh.size(); ii++) {
            pmin = Point( 999,  999,  999);
            pmax = Point(-999, -999, -999);
            Mesh* m = &mesh[ii];
            for(unsigned int jj = 0; jj < m->positions().size(); jj++) {
                vec3 point = mesh[ii].positions()[jj];
                pmin.x = point.x < pmin.x ? point.x : pmin.x;
                pmin.y = point.y < pmin.y ? point.y : pmin.y;
                pmin.z = point.z < pmin.z ? point.z : pmin.z;
                pmax.x = point.x > pmax.x ? point.x : pmax.x;
                pmax.y = point.y > pmax.y ? point.y : pmax.y;
                pmax.z = point.z > pmax.z ? point.z : pmax.z;
            }
            bboxes.push_back(pmin);
            bboxes.push_back(pmax);
        }

        // Textures
        ImageData image= read_image_data("src/minecraft/terrain_texture.png");
        GLenum data_format;
        GLenum data_type= GL_UNSIGNED_BYTE;
        if(image.channels == 3) data_format= GL_RGB;
        else data_format= GL_RGBA;

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0,
            GL_RGB, image.width, image.height, 0,
            data_format, data_type, image.buffer());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        // Framebuffer
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);

        // Framebuffer Texture
        glGenTextures(1, &fbo_texture);
        glBindTexture(GL_TEXTURE_2D, fbo_texture);
        glTexImage2D(GL_TEXTURE_2D, 0,
            GL_RGB, image.width, image.height, 0,
            data_format, data_type, image.buffer());
        glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, fbo_texture, 0);

        GLenum buffers[]= { GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(1, buffers);


        // Init des vao, 1 vao par block
        for(int ii = 0; ii < 9; ii++) {
            vaos.push_back(0);
            glGenVertexArrays(1, &vaos[ii]);
            glBindVertexArray(vaos[ii]);
            // vertex buffer
            vertex_buffers.push_back(0);
            glGenBuffers(1, &vertex_buffers[ii]);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[ii]);

            size_t offset = 0;
            size_t size = mesh[ii].vertex_buffer_size() + mesh[ii].texcoord_buffer_size() + mesh[ii].normal_buffer_size();
            glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);

            size= mesh[ii].vertex_buffer_size();
            glBufferSubData(GL_ARRAY_BUFFER, offset, size, mesh[ii].vertex_buffer());
            // configure l'attribut 0, vec3 position

            // configurer l'attribut position, cf declaration dans le vertex shader : in vec3 position;
            ///GLint position= glGetAttribLocation(program, "position");
            ///glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 0, 0);
            ///glEnableVertexAttribArray(position);
            // ou plus simplement, layout dans le shader
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(0);

            // Texcoord
            offset += size;
            size = mesh[ii].texcoord_buffer_size();
            glBufferSubData(GL_ARRAY_BUFFER, offset, size, mesh[ii].texcoord_buffer());
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid *) offset);
            glEnableVertexAttribArray(1);

            // Normale
            offset += size;
            size = mesh[ii].normal_buffer_size();
            glBufferSubData(GL_ARRAY_BUFFER, offset, size, mesh[ii].normal_buffer());
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *) offset);
            glEnableVertexAttribArray(2);

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            mesh[ii].release();
        }
        // Gestion des temps
        glGenQueries(1, &time_query);
        console = create_text();

        // Nettoyage
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
        glClearColor(0.35294117647058826, 0.7725490196078432, 0.9294117647058824, 1.0);

        glClearDepthf(2);
        glDepthFunc(GL_LESS);
        glEnable(GL_DEPTH_TEST);

        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);

        return 0;
    }

    int quit() {
        for(auto m: mesh)
            m.release();
        cube.release();
        return 0;
    }

    int render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int mx, my;
        unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);

        // Déplacement de la vue personnage avec ZQSD
        if(key_state('z')){
            cam.moveForward();
        }
        if(key_state('q')){
            cam.turnLeft();
        }
        if(key_state('s')){
            cam.moveBackward();
        }
        if(key_state('d')){
            cam.turnRight();
        }

        // Déplacement de la source de lumière avec les fleches directionnelles
        if(key_state(SDLK_RIGHT)){
            lightPos = vec3(lightPos.x+5, lightPos.y, lightPos.z);
        }
        if(key_state(SDLK_LEFT)){
            lightPos = vec3(lightPos.x-5, lightPos.y, lightPos.z);
        }
        if(key_state(SDLK_DOWN)){
            lightPos = vec3(lightPos.x, lightPos.y, lightPos.z+5);
        }
        if(key_state(SDLK_UP)){
            lightPos = vec3(lightPos.x, lightPos.y, lightPos.z-5);
        }

        // Rechagement du shader
        if(key_state('r')) {
            clear_key_state('r');
            reload_program(program, "src/minecraft/BRDF.glsl");
            program_print_errors(program);
        }

        // Vue extérieure - vue shadowmap - touche ESPACE
        if(key_state(' ')) {
            if(mb & SDL_BUTTON(1))
                cam2.rotation(mx, my);
            else if(mb & SDL_BUTTON(3))
                cam2.move(mx);
            else if(mb & SDL_BUTTON(2))
                cam2.translation((float) mx / (float) window_width(), (float) my / (float) window_height());

            Transform model = Identity();
            Transform view = cam2.view();
            Transform projection = cam2.projection(window_width(), window_height(), 45);

            //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
            //glViewport(0, 0, height_map.width(), height_map.height());
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Affiche tous les blocs du point de vue de la caméra 2
            for(unsigned int ii = 0; ii < 9; ii++) {
                // gKit style
                glBindVertexArray(vaos[ii]);
                glUseProgram(program);
                glBindTexture(GL_TEXTURE_2D, texture);
                program_uniform(program, "mvpMatrix", projection*view*model);
                program_uniform(program, "modelMatrix", model);
                program_uniform(program, "texture_field", 0);
                program_uniform(program, "light", lightPos);
                program_uniform(program, "light_lux", vec3(1.0, 1.0, 1.0));
                program_uniform(program, "obs", vec3(cam2.position().x, cam2.position().y, cam2.position().z));
                glDrawArrays(GL_TRIANGLES, 0, mesh[ii].vertex_count());
            }
            glBindTexture(GL_TEXTURE_2D, 0);
            glUseProgram(0);
            glBindVertexArray(0);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            return 2;
        }

        // mesure le temps d'execution du draw pour le gpu
        glBeginQuery(GL_TIME_ELAPSED, time_query);
        std::chrono::high_resolution_clock::time_point cpu_start= std::chrono::high_resolution_clock::now();

        Transform model = Identity();
        Transform view = cam.view();
        Transform projection = cam.projection();
        Transform mvp = projection*view*model;

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glViewport(0, 0, window_width(), window_height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int cptvis = 0;
        for (int ii = 0; ii < 9; ii++) {
            // Traite le block uniquement si visible
            if(visible(mvp, bboxes[2*ii], bboxes[2*ii+1])) {
                glBindVertexArray(vaos[ii]);
                glUseProgram(program);
                // texture
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture);

                // Mode opengl
                GLint location= glGetUniformLocation(program, "texture_field");
                glUniform1i(location, 0);

                location= glGetUniformLocation(program, "mvpMatrix");
                glUniformMatrix4fv(location, 1, GL_TRUE, mvp.buffer());

                location= glGetUniformLocation(program, "modelMatrix");
                glUniformMatrix4fv(location, 1, GL_TRUE, model.buffer());

                location= glGetUniformLocation(program, "light");
                glUniform3f(location, lightPos.x, lightPos.y, lightPos.z);

                location= glGetUniformLocation(program, "light_lux");
                glUniform3f(location, 1.0, 1.0, 1.0);

                location= glGetUniformLocation(program, "obs");
                glUniform3f(location, cam.position().x, cam.position().y, cam.position().z);

                location= glGetUniformLocation(program, "m");
                glUniform1f(location, 3.0);

                glDrawArrays(GL_TRIANGLES, 0, mesh[ii].vertex_count());
                cptvis++;
            }
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
        glBindVertexArray(0);

        // Chronomètre et affichage console
        std::chrono::high_resolution_clock::time_point cpu_stop= std::chrono::high_resolution_clock::now();
        long long int cpu_time= std::chrono::duration_cast<std::chrono::nanoseconds>(cpu_stop - cpu_start).count();
        glEndQuery(GL_TIME_ELAPSED);
        std::chrono::high_resolution_clock::time_point wait_start= std::chrono::high_resolution_clock::now();
        GLint64 gpu_time= 0;
        glGetQueryObjecti64v(time_query, GL_QUERY_RESULT, &gpu_time);
        std::chrono::high_resolution_clock::time_point wait_stop= std::chrono::high_resolution_clock::now();
        long long int wait_time= std::chrono::duration_cast<std::chrono::nanoseconds>(wait_stop - wait_start).count();
        clear(console);
        printf(console, 0, 1, "cpu  %02dms %03dus", (int) (cpu_time / 1000000), (int) ((cpu_time / 1000) % 1000));
        printf(console, 0, 2, "gpu  %02dms %03dus", (int) (gpu_time / 1000000), (int) ((gpu_time / 1000) % 1000));
        printf(console, 0, 3, "wait %02dms %03dus", (int) (wait_time / 1000000), (int) ((wait_time / 1000) % 1000));
        printf(console, 0, 4, "FPS %f", 1.0/((cpu_time + gpu_time + wait_time)/1000000000.0));
        printf(console, 0, 5, "Blocks drawn %d / 9", cptvis);
        draw(console, window_width(), window_height());
        return 1;
    }



protected:
    float angle= 0;
    Mesh cube;
    std::vector<Mesh> mesh;
    GLuint program;
    GLuint texture;
    GLuint framebuffer;
    GLuint fbo_texture;
    GLuint time_query;
    Text console;

    /*
     *  Boites englobantes, 2 points à la suite pour chaque mesh
     *  mesh[0] a une bbox définie avec deux points: bboxes[0] et bboxes[1]
     *  mesh[1] a une bbox définie avec deux points: bboxes[2] et bboxes[3]
     *  mesh[N] a une bbox définie avec deux points: bboxes[2*N] et bboxes[2*N+1]
     */
    std::vector<Point> bboxes;
    Image height_map;
    Point current_pos;
    b_Camera cam;
    Orbiter cam2;
    vec3 lightPos;

    std::vector<GLuint> vaos;
    std::vector<GLuint> vertex_buffers;
};


int main( int argc, char **argv ) {
    Minecraft minecraft;
    minecraft.run();
    return 0;
}
