#include "model/ui/UserInterface.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {

    auto load = []() {
        if (!CarModel::instance().load()) return;
        if (!Car::instance().load()) return;
        if (!Manager::instance().load()) return;
        if (!CarManager::instance().load()) return;
    };

    if (Configuration::instance()["loadOnStart"].value.boolean)
        load();

    UserInterface::mainMenu();

    if (Configuration::instance()["saveOnExit"].value.boolean) {
        CarModel::instance().save();
        Car::instance().save();
        Manager::instance().save();
        CarManager::instance().save();
    }

    return 0;
}