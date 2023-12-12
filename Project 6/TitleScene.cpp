#include "TitleScene.h"
#include "Utility.h"

#define LEVEL_WIDTH 1
#define LEVEL_HEIGHT 1

unsigned int TITLESCENE_DATA[] = {0};

TitleScene::~TitleScene()
{
    delete[] m_state.enemies;
    delete m_state.player;
    delete m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void TitleScene::initialise()
{
    m_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("assets/tilemap.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, TITLESCENE_DATA, map_texture_id, 1.0f, 16, 7);

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
    m_state.player->m_walking[m_state.player->UP] = new int[3]{3, 4, 5};
    // m_state.player->m_walking[m_state.player->DOWN] = new int[4]{0, 1, 2, 3};

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->UP];
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

void TitleScene::update(float delta_time)
{
    if (!m_state.titleScreen)
        m_state.next_scene_id = 1;

    // m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);
}

void TitleScene::render(ShaderProgram *program)
{
    const char FONT_FILEPATH[] = "assets/font1.png";
    GLuint g_font_id = Utility::load_texture(FONT_FILEPATH);

    // glm::vec3 text_position = glm::vec3(1.0f, 1.7f, 0);
    double playerPosX = m_state.player->get_position().x;
    if (playerPosX < 5.0f)
        playerPosX = 5.0f;
    else if (playerPosX > 9.5f)
        playerPosX = 9.5f;

    glm::vec3 text_position = glm::vec3(playerPosX - 2.5f, -2.0f, 0);
    glm::vec3 text_position2 = glm::vec3(playerPosX - 1.5f, -2.7f, 0);
    glm::vec3 text_position3 = glm::vec3(playerPosX - 3.0f, -4.0f, 0);
    glm::vec3 text_position4 = glm::vec3(playerPosX - 3.0f, -4.5f, 0);
    glm::vec3 text_position5 = glm::vec3(playerPosX - 3.0f, -5.0f, 0);
    glm::vec3 text_position6 = glm::vec3(playerPosX - 4.0f, -6.0f, 0);
    Utility::draw_text(program, g_font_id, "Press ENTER", 0.5, 0.01f, text_position);
    Utility::draw_text(program, g_font_id, "to Start", 0.5, 0.01f, text_position2);
    Utility::draw_text(program, g_font_id, "Reach the right to win", 0.3, 0.01f, text_position3);
    Utility::draw_text(program, g_font_id, "Stomp the enemies' heads", 0.3, 0.01f, text_position4);
    Utility::draw_text(program, g_font_id, "Otherwise you die", 0.3, 0.01f, text_position5);
    Utility::draw_text(program, g_font_id, "MORE CATS JUMPING", 0.5, 0.01f, text_position6);
}
