#include "Scene.h"

class TitleScene : public Scene
{
public:
    int ENEMY_COUNT = 1;

    ~TitleScene();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};
