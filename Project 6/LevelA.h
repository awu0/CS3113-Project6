#include "Scene.h"

class LevelA : public Scene {
public:
    int m_number_of_enemies = 6;

    ~LevelA();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    int number_of_enemies_left() override;
};
