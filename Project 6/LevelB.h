#include "Scene.h"

class LevelB : public Scene {
public:
    int m_number_of_enemies = 6;

    ~LevelB();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    int number_of_enemies_left() override;
};
