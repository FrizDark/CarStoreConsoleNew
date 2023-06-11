#include "model/ui/UserInterface.h"
#include <iostream>
using namespace std;

string getId() {
    static random_generator gen;
    return to_string(gen());
}

vector<string> generateIds(int count) {
    vector<string> ids;
    ids.reserve(count);
    for (int i = 0; i < count; ++i) {
        ids.emplace_back(getId());
    }
    return ids;
}

void createModels() {
    CarModel& modelsInstance = CarModel::instance();
    auto ids = generateIds(5);
    string marks[] = {"Volvo", "Toyota", "BMW", "Volvo", "Volvo"};
    string models[] = {"S1", "R2", "M4", "911", "911"};
    string types[] = {"Sedan", "Jeep", "Sedan", "Sport", "Jeep"};
    vector<CarModelClass> v(5);
    modelsInstance.clear();
    for (int i = 0; i < 5; ++i) {
        v[i]["id"] = ids[i];
        v[i]["mark"] = marks[i];
        v[i]["model"] = models[i];
        v[i]["type"] = types[i];
        modelsInstance.add(v[i].clone());
    }
    modelsInstance.save();
}

void createCars() {
    CarModel& modelsInstance = CarModel::instance();
    Car& carsInstance = Car::instance();
    auto modelsList = modelsInstance.filter([](const CarModelClass*){ return true; });
    vector<const CarModelClass*> models(modelsList.cbegin(), modelsList.cend());
    auto ids = generateIds(3);
    string colors[] = {"Blue", "Red", "Green"};
    double prices[] = {999, 9129, 1234};
    vector<CarClass> v(3);
    carsInstance.clear();
    for (int i = 0; i < 3; ++i) {
        v[i]["id"] = ids[i];
        v[i]["color"] = colors[i];
        v[i]["price"] = prices[i];
        v[i]["model_id"] = (*models[i + 1])["id"];
        v[i]["mark"] = (*models[i + 1])["mark"];
        v[i]["model"] = (*models[i + 1])["model"];
        carsInstance.add(v[i].clone());
    }
    carsInstance.save();
}

void createManagers() {
    Manager& managersInstance = Manager::instance();
    auto ids = generateIds(2);
    int ages[] = {58, 35};
    string cities[] = {"Kharkiv", "Kyiv"};
    string firstnames[] = {"Vladimir", "Alexander"};
    string lastnames[] = {"Ivanov", "Ogurtsov"};
    string phones[] = {"+3801234567", "+3807654321"};
    vector<ManagerClass> v(2);
    managersInstance.clear();
    for (int i = 0; i < 2; ++i) {
        v[i]["id"] = ids[i];
        v[i]["age"] = ages[i];
        v[i]["city"] = cities[i];
        v[i]["firstname"] = firstnames[i];
        v[i]["lastname"] = lastnames[i];
        v[i]["phone"] = phones[i];
        managersInstance.add(v[i].clone());
    }
    managersInstance.save();
}

void createCarManagers() {
    auto carsList = Car::instance().filter([](const CarClass*){ return true; });
    auto managersList = Manager::instance().filter([](const ManagerClass*){ return true; });
    vector<const CarClass*> cars(carsList.cbegin(), carsList.cend());
    vector<const ManagerClass*> managers(managersList.cbegin(), managersList.cend());
    pair<int, int> pairs[] = {make_pair(0, 0), make_pair(1, 1), make_pair(2, 1)};
    vector<CarManagerClass> v(3);
    CarManager::instance().clear();
    for (int i = 0; i < 3; i++) {
        auto& car = *cars[pairs[i].first];
        auto& manager = *managers[pairs[i].second];
        v[i]["car_id"] = car["id"];
        v[i]["mark"] = car["mark"];
        v[i]["model"] = car["model"];
        v[i]["manager_id"] = manager["id"];
        v[i]["lastname"] = manager["lastname"];
        v[i]["city"] = manager["city"];
        v[i]["phone"] = manager["phone"];
        CarManager::instance().add(v[i].clone());
    }
    CarManager::instance().save();
}

int main(int argc, char *argv[]) {

//    createModels();
//    createCars();
//    createManagers();
//    createCarManagers();

    CarModel::instance().load();
    Car::instance().load();
    Manager::instance().load();
    CarManager::instance().load();

    UserInterface::mainMenu();

    return 0;
}