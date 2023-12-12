#include "Scene.h"

class LevelB : public Scene {
public:
    int ENEMY_COUNT = 1;
    int BULLET_COUNT = m_number_of_bullets;

    ~LevelB();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    int number_of_enemies_left() override;
};
