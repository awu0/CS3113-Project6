#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Entity.h"
#include "Map.h"

struct GameState
{
    Map    *map;
    Entity *player;
    Entity *enemies;
    Entity *bullets;

    Mix_Music *bgm;
    Mix_Chunk *jump_sfx;
    Mix_Chunk *stage_clear_sfx;
    Mix_Chunk *enemy_killed_sfx;
    Mix_Chunk *death_sfx;

    int next_scene_id;

    bool shooting_allowed = true;
    int bullets_shot = 0;

    bool gameover = false;
    bool won = false;
    bool titleScreen = true;
};

class Scene {
public:
    int m_number_of_enemies = 1;
    int m_number_of_bullets = 120;

    GameState m_state;

    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram *program) = 0;

    GameState const get_state() const { return m_state; }

    void do_gameover(); // restart from beginning of scene
    virtual int number_of_enemies_left();
};
