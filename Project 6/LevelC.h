#include "Scene.h"

class LevelC : public Scene
{
public:
    int ENEMY_COUNT = 1;
    int BULLET_COUNT = 100; // keep only 100 bullets in memory

    ~LevelC();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};
