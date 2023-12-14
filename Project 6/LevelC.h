#include "Scene.h"

class LevelC : public Scene
{
public:
    int m_number_of_enemies = 5;

    ~LevelC();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    int number_of_enemies_left() override;
};
