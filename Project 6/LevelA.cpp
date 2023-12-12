#include "LevelA.h"
#include "Utility.h"

#define LEVEL_WIDTH 15
#define LEVEL_HEIGHT 9

unsigned int LEVELA_DATA[] =
{
    9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
    9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7,
    9, 8, 0, 0, 0, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0,
    9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0,
    9, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 7,
    9, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 7,
    9, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    9, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
};

LevelA::~LevelA()
{
    delete [] m_state.enemies;
    delete [] m_state.bullets;
    delete    m_state.player;
    delete    m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void LevelA::initialise()
{
    m_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("assets/tilemap.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELA_DATA, map_texture_id, 1.0f, 16, 7);

    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(2.5f);
    m_state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture("assets/catdog.png");

    // Walking
    m_state.player->m_walking[m_state.player->LEFT] = new int[3]{12, 13, 14};
    m_state.player->m_walking[m_state.player->RIGHT] = new int[3]{21, 22, 23};
    m_state.player->m_walking[m_state.player->UP] = new int[3]{30, 31, 32};
    m_state.player->m_walking[m_state.player->DOWN] = new int[3]{3, 4, 5};

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->DOWN];
    m_state.player->m_animation_time = 0.0f;
    m_state.player->m_animation_frames = 3;
    m_state.player->m_animation_index = 0;
    m_state.player->m_animation_cols = 9;
    m_state.player->m_animation_rows = 8;
    m_state.player->set_height(0.8f);
    m_state.player->set_width(0.8f);

    // Jumping
    m_state.player->m_jumping_power = 6.0f;

    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture("assets/enemy.jpg");

    m_state.enemies = new Entity[ENEMY_COUNT];
    m_state.enemies[0].set_entity_type(ENEMY);
    m_state.enemies[0].set_ai_type(GUARD);
    m_state.enemies[0].set_ai_state(IDLE);
    m_state.enemies[0].m_texture_id = enemy_texture_id;
    m_state.enemies[0].set_position(glm::vec3(9.0f, 0.0f, 0.0f));
    m_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_state.enemies[0].set_speed(1.0f);
    m_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

    // keep 100 spaces for bullets
    m_state.bullets = new Entity[BULLET_COUNT];

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_state.bgm = Mix_LoadMUS("assets/boogieparty.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 15);

    m_state.jump_sfx = Mix_LoadWAV("assets/jump.wav");
    m_state.stage_clear_sfx = Mix_LoadWAV("assets/clear.wav");
    m_state.enemy_killed_sfx = Mix_LoadWAV("assets/kill.wav");
    m_state.death_sfx = Mix_LoadWAV("assets/die.wav");
}

void LevelA::update(float delta_time)
{
    if (m_state.gameover) {
        do_gameover();
        return;
    }

    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
        m_state.enemies[i].update(delta_time, m_state.player, NULL, 0, m_state.map);

    for (int i = 0; i < m_state.bullets_shot; i++)
        m_state.bullets[i].update(delta_time, NULL, m_state.enemies, ENEMY_COUNT, m_state.map);

    if (m_state.player->get_position().x > 14.8f) m_state.next_scene_id = 2;
}

void LevelA::render(ShaderProgram *program)
{
    m_state.map->render(program);
    m_state.player->render(program);

    for (int i = 0; i < ENEMY_COUNT; i++)
        m_state.enemies[i].render(program);

    for (int i = 0; i < m_state.bullets_shot; i++)
        m_state.bullets[i].render(program);
}
