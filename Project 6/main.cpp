/**
 * Author: Aaron Wu
 * Assignment: Platformer
 * Date due: 2023-12-02, 11:59pm
 * I pledge that I have completed this assignment without
 * collaborating with anyone else, in conformance with the
 * NYU School of Engineering Policies and Procedures on
 * Academic Misconduct.
 **/
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 15
#define LEVEL1_HEIGHT 9
#define LEVEL_LEFT_EDGE 5.0f
#define LEVEL_RIGHT_EDGE 9.5f
#define LEVELS 4

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "TitleScene.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "Effects.h"

// ––––– CONSTANTS ––––– //
const int WINDOW_WIDTH = 640,
          WINDOW_HEIGHT = 480;

const float BG_RED = 0.1f,
            BG_BLUE = 0.2f,
            BG_GREEN = 0.5f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl",
           FONT_FILEPATH[] = "assets/font1.png";

const float MILLISECONDS_IN_SECOND = 1000.0;

// ––––– GLOBAL VARIABLES ––––– //
Scene *g_current_scene;
TitleScene *g_titleScene;
LevelA *g_levelA;
LevelB *g_levelB;
LevelC *g_levelC;

Effects *g_effects;
Scene *g_levels[LEVELS];

SDL_Window *g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

bool g_is_colliding_bottom = false;

// ––––– GENERAL FUNCTIONS ––––– //
void switch_to_scene(Scene *scene)
{
    g_current_scene = scene;
    g_current_scene->initialise(); // DON'T FORGET THIS STEP!
}

void createBullet() {
    int bullets_shot = g_current_scene->m_state.bullets_shot;
    g_current_scene->m_state.bullets_shot++;

    // g_current_scene->m_state.bullets[bullets_shot] = new Entity();

    g_current_scene->m_state.bullets[bullets_shot].set_entity_type(BULLET);
    g_current_scene->m_state.bullets[bullets_shot].set_position(g_current_scene->m_state.player->get_position());

    int facing = g_current_scene->m_state.player->facing_direction;
    int x = 0;
    int y = 0;
    if (facing == g_current_scene->m_state.player->UP)
        y = 1;
    else if (facing == g_current_scene->m_state.player->DOWN)
        y = -1;
    else if (facing == g_current_scene->m_state.player->LEFT)
        x = -1;
    else if (facing == g_current_scene->m_state.player->RIGHT)
        x = 1;

    g_current_scene->m_state.bullets[bullets_shot].set_movement(glm::vec3(x, y, 0.0f));

    g_current_scene->m_state.bullets[bullets_shot].set_speed(8.0f);
    g_current_scene->m_state.bullets[bullets_shot].m_texture_id = Utility::load_texture("assets/catdog.png");

    // Walking
    g_current_scene->m_state.bullets[bullets_shot].m_walking[g_current_scene->m_state.bullets[bullets_shot].LEFT] = new int[3]{9, 10, 11};
    g_current_scene->m_state.bullets[bullets_shot].m_walking[g_current_scene->m_state.bullets[bullets_shot].RIGHT] = new int[3]{18, 19, 20};
    g_current_scene->m_state.bullets[bullets_shot].m_walking[g_current_scene->m_state.bullets[bullets_shot].UP] = new int[3]{27, 28, 29};
    g_current_scene->m_state.bullets[bullets_shot].m_walking[g_current_scene->m_state.bullets[bullets_shot].DOWN] = new int[3]{0, 1, 2};

    g_current_scene->m_state.bullets[bullets_shot].m_animation_indices = g_current_scene->m_state.bullets[bullets_shot].m_walking[g_current_scene->m_state.player->facing_direction];
    g_current_scene->m_state.bullets[bullets_shot].m_animation_time = 0.0f;
    g_current_scene->m_state.bullets[bullets_shot].m_animation_frames = 3;
    g_current_scene->m_state.bullets[bullets_shot].m_animation_index = 0;
    g_current_scene->m_state.bullets[bullets_shot].m_animation_cols = 9;
    g_current_scene->m_state.bullets[bullets_shot].m_animation_rows = 8;
    g_current_scene->m_state.bullets[bullets_shot].set_height(0.8f);
    g_current_scene->m_state.bullets[bullets_shot].set_width(0.8f);
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("MORE CATS JUMPING AROUND",
                                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                        WINDOW_WIDTH, WINDOW_HEIGHT,
                                        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    g_titleScene = new TitleScene();
    g_levelA = new LevelA();
    g_levelB = new LevelB();
    g_levelC = new LevelC();

    g_levels[0] = g_titleScene;
    g_levels[1] = g_levelA;
    g_levels[2] = g_levelB;
    g_levels[3] = g_levelC;

    // starting scene is title screen
    switch_to_scene(g_levels[0]);

    g_effects = new Effects(g_projection_matrix, g_view_matrix);
    g_effects->start(SHRINK, 7.0f);
}

void process_input()
{
    // VERY IMPORTANT: If nothing is pressed, we don't want to go anywhere
    g_current_scene->m_state.player->set_movement(glm::vec3(0.0f));

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        // End game
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_q:
                // Quit the game with a keystroke
                g_game_is_running = false;
                break;

            case SDLK_SPACE:
                // Shoot
                if (g_current_scene->m_state.bullets_shot < g_current_scene->m_number_of_bullets && g_current_scene->m_state.shooting_allowed) {
                    createBullet();
                    Mix_PlayChannel(-1, g_current_scene->m_state.jump_sfx, 0);
                }
                break;

            case SDLK_RETURN:
                if (g_current_scene == g_titleScene)
                    g_current_scene->m_state.titleScreen = false;
                break;

            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])
    {
        g_current_scene->m_state.player->move_left();
        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->LEFT];
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        g_current_scene->m_state.player->move_right();
        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->RIGHT];
    }
    else if (key_state[SDL_SCANCODE_UP])
    {
        g_current_scene->m_state.player->move_up();
        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->UP];
    }
    else if (key_state[SDL_SCANCODE_DOWN])
    {
        g_current_scene->m_state.player->move_down();
        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->DOWN];
    }

    if (glm::length(g_current_scene->m_state.player->get_movement()) > 1.0f)
    {
        g_current_scene->m_state.player->set_movement(glm::normalize(g_current_scene->m_state.player->get_movement()));
    }
}

void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }

    if (g_current_scene->m_state.gameover)
        return;

    while (delta_time >= FIXED_TIMESTEP)
    {
        g_current_scene->update(FIXED_TIMESTEP);
        g_effects->update(FIXED_TIMESTEP);

        // check enemy collisions
        EntityType collided_with = g_current_scene->m_state.player->collided_with;

        // if (g_is_colliding_bottom == false && g_current_scene->m_state.player->m_collided_bottom) g_effects->start(SHAKE, 1.0f);

        if (collided_with == ENEMY)
        {
            g_current_scene->m_state.gameover = true;
            g_current_scene->m_state.won = false;
            Mix_PlayChannel(-1, g_current_scene->m_state.death_sfx, 0);
        }

        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;

    g_view_matrix = glm::mat4(1.0f);

    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->m_state.player->get_position().x, -g_current_scene->m_state.player->get_position().y, 0));

    g_view_matrix = glm::translate(g_view_matrix, g_effects->m_view_offset);
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(g_shader_program.get_program_id());
    g_current_scene->render(&g_shader_program);
    g_effects->render();

    GLuint g_font_id = Utility::load_texture(FONT_FILEPATH);

    double playerPosX = g_current_scene->m_state.player->get_position().x;
    double playerPosY = g_current_scene->m_state.player->get_position().y;

    glm::vec3 text_position = glm::vec3(playerPosX - 4.7f, playerPosY + 3.5f, 0);
    std::string bulletNum = std::to_string(g_current_scene->m_number_of_bullets - g_current_scene->m_state.bullets_shot);
    Utility::draw_text(&g_shader_program, g_font_id, "Bullets: " + bulletNum, 0.3, 0.005f, text_position);

    std::string enemiesNum = std::to_string(g_current_scene->number_of_enemies_left());
    glm::vec3 text_position2 = glm::vec3(playerPosX - 4.7f, playerPosY + 3.1f, 0);
    Utility::draw_text(&g_shader_program, g_font_id, "Enemies left: " + enemiesNum, 0.3, 0.005f, text_position2);

    // game over
    if (g_current_scene->m_state.gameover && !g_current_scene->m_state.won)
    {
        glm::vec3 text_position = glm::vec3(playerPosX - 1.0f, playerPosY, 0);
        glm::vec3 text_position2 = glm::vec3(playerPosX - 1.0f, playerPosY - 0.5f, 0);
        Utility::draw_text(&g_shader_program, g_font_id, "GAME OVER", 0.5, 0.01f, text_position);
        Utility::draw_text(&g_shader_program, g_font_id, "YOU LOST", 0.5, 0.01f, text_position2);
    }
    if (g_current_scene->m_state.won)
    {
        glm::vec3 text_position = glm::vec3(playerPosX - 7.0f, playerPosY, 0);
        Utility::draw_text(&g_shader_program, g_font_id, "YOU WON", 0.5, 0.01f, text_position);
    }

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();

    delete g_levelA;
    delete g_levelB;
    delete g_effects;
}

// ––––– DRIVER GAME LOOP ––––– //
int main(int argc, char *argv[])
{
    initialise();

    while (g_game_is_running)
    {
        process_input();
        update();

        if (g_current_scene->m_state.next_scene_id >= 0)
        {
            switch_to_scene(g_levels[g_current_scene->m_state.next_scene_id]);
            g_effects->start(SHRINK, 7.0f);
            Mix_PlayChannel(-1, g_current_scene->m_state.stage_clear_sfx, 0);
        }

        render();
    }

    shutdown();
    return 0;
}
