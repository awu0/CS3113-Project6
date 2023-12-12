#include "Scene.h"

void Scene::do_gameover() {
    initialise();

    m_state.gameover = false;
}

int Scene::number_of_enemies_left() {
    return 0;
}