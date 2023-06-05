#include "UserInterface.h"

bool UserInterface::findElement(list<Object*> items, Object* item) {
    return std::any_of(items.begin(), items.end(), [item](const auto& i) {
        return *item == *i;
    });
}

ElementValue UserInterface::askForValue(const string& title, ElementType type) {
    string s;
    while (true) {
        cout << title;
        switch (type) {
            case et_boolean:
                cout << " (y/n): ";
                cin >> s; cin.get();
                if (s == "y" || s == "n") {
                    return s == "y";
                }
                showBanner(B_TRY_AGAIN);
                break;
            case et_number:
                cout << " (number): ";
                cin >> s; cin.get();
                try {
                    return stod(s);
                } catch (...) {}
                showBanner(B_TRY_AGAIN);
                break;
            case et_string:
                cout << ": ";
                getline(cin, s);
                if (!s.empty()) {
                    return s;
                }
                showBanner(B_TRY_AGAIN);
                break;
            default: return {};
        }
    }
}

void UserInterface::showBanner(UserInterface::Banner type) {
    switch (type) {
        case B_SUCCESS:
            cout << "### #   SUCCESS   # ###" << endl;
            break;
        case B_NOT_FOUND:
            cout << "### #   NOT FOUND   # ###" << endl;
            break;
        case B_TRY_AGAIN:
            cout << "### #   TRY AGAIN   # ###" << endl;
            break;
        case B_SEARCH:
            cout << "### #   SEARCH   # ###" << endl;
            break;
        case B_CREATE:
            cout << "### #   CREATE   # ###" << endl;
            break;
        case B_USED_IN:
            cout << "### #   USED IN   # ###" << endl;
            break;
        case B_ALREADY_EXISTS:
            cout << "### #   ALREADY EXISTS   # ###" << endl;
            break;
    }
}

void UserInterface::printMenu(
        const std::string &title,
        const vector<pair<string, std::function<void()>>> &actions,
        const string& exitName) {
    bool run = true;
    string input;
    size_t maxWidth = title.size();

    for (const auto& i: actions) {
        if (i.first.size() + 6 > maxWidth) {
            maxWidth = i.first.size() + 6;
        }
    }

    auto line = "|" + string(maxWidth + 2, '-') + "|";
    auto format = [maxWidth](std::string value) {
        if (maxWidth > value.size()) value.append(maxWidth - value.size(), ' ');
        return value;
    };

    while (run) {
        system("clear");
        cout << line << endl;
        cout << "| " << format(title) << " |" << endl;
        cout << line << endl;
        for (size_t i = 0; i < actions.size(); ++i) {
            cout << "| " << format((i + 1 > 9 ? "" : " ") + to_string(i + 1) + " | " + actions[i].first) << " |" << endl;
        }
        cout << "| " << format(" 0 | " + exitName) << " |" << endl;
        cout << line << endl;
        cout << "> "; cin >> input;
        cin.get();
        system("clear");
        int in = -1;
        try {
            in = std::stoi(input);
        } catch (...) {
            showBanner(B_TRY_AGAIN);
            cout << "### #   PRESS ANY KEY TO CONTINUE   # ###";
            cin.get();
            continue;
        }
        if (in == 0) run = false;
        else if (in > 0 && in <= actions.size()) {
            actions[in - 1].second();
            cout << "### #   PRESS ANY KEY TO CONTINUE   # ###";
            cin.get();
        } else {
            showBanner(B_TRY_AGAIN);
        }
    }

}

template <typename T>
void UserInterface::printTable(const list<T *> &values, const vector<std::string> &ignoreFields) {
    auto fields = values.front()->fields();
    auto format = [](std::string value, size_t width) {
        if (width > value.size()) value.append(width - value.size(), ' ');
        return value;
    };
    map<std::string, size_t> columns;
    string line = "|";
    for (const auto& field: fields) {
        if (contains(ignoreFields, field.first)) continue;
        for (const auto& value: values) {
            if (!columns.contains(field.first)) {
                columns[field.first] = field.second.name.size();
            }
            if (columns[field.first] < (*value)[field.first].toString().size()) {
                columns[field.first] = (*value)[field.first].toString().size();
            }
        }
        line += string(columns[field.first] + 2, '-') + "|";
    }
    cout << line << endl;
    cout << "| ";
    for (const auto& field: fields) {
        if (contains(ignoreFields, field.first)) continue;
        cout << format(field.second.name, columns[field.first]) << " | ";
    }
    cout << endl;
    cout << line << endl;
    for (const auto& value: values) {
        cout << "| ";
        for (const auto& field: fields) {
            if (contains(ignoreFields, field.first)) continue;
            cout << format((*value)[field.first].toString(), columns[field.first]) << " | ";
        }
        cout << endl;
    }
    cout << line << endl;
}

void UserInterface::printTable(const BasicSerializable* table, const vector<std::string> &ignoreFields) {
    auto elements = table->elements();
    printTable<Object>({elements.begin(), elements.end()}, ignoreFields);
}

void UserInterface::carModelMenu() {
    vector<string> ignoreFields = {"id"};
    string title = "Car model";
    vector<pair<string, std::function<void()>>> actions;
    actions.emplace_back("Add", [ignoreFields](){
        auto model = addEngine<CarModelClass>(ignoreFields);
        if (model == nullptr) return;
        if (!findElement(CarModel::instance().elements(), model)) {
            (*model)["id"] = generateId();
            CarModel::instance().add(model);
            showBanner(B_SUCCESS);
        } else {
            delete model;
            showBanner(B_ALREADY_EXISTS);
        }
    });
    actions.emplace_back("Delete", [ignoreFields](){
        auto item = searchEngine<CarModel, CarModelClass>(ignoreFields);
        if (item == nullptr) return;
        auto uses = Car::instance().filter([item](const CarClass* i) {
            return (*i)["model_id"].toString() == (*item)["id"].toString();
        });
        if (!uses.empty()) {
            showBanner(B_USED_IN);
            printTable(uses, {"id", "model_id"});
            return;
        }
        CarModel::instance().remove(item);
        showBanner(B_SUCCESS);
    });
    actions.emplace_back("Recreate", [ignoreFields](){
        auto item = searchEngine<CarModel, CarModelClass>(ignoreFields);
        if (item == nullptr) return;
        string id = (*item)["id"].toString();
        auto newItem = addEngine<CarModelClass>(ignoreFields);
        if (newItem == nullptr) return;
        if (findElement(CarModel::instance().elements(), newItem)) {
            delete newItem;
            showBanner(B_ALREADY_EXISTS);
            return;
        }
        *item = *newItem;
        delete newItem;
        (*item)["id"] = id;
        for (const auto& car: Car::instance().elements()) {
            if ((*car)["model_id"] != id) continue;
            (*car)["mark"] = (*item)["mark"];
            for (const auto& carManager: CarManager::instance().elements()) {
                if ((*carManager)["car_id"] != (*car)["id"]) continue;
                (*carManager)["mark"] = (*car)["mark"];
            }
        }
        showBanner(B_SUCCESS);
    });
    actions.emplace_back("Show", [ignoreFields](){
        printTable(&CarModel::instance(), ignoreFields);
    });
    actions.emplace_back("Search", [ignoreFields](){
        auto item = searchEngine<CarModel, CarModelClass>(ignoreFields);
        if (item != nullptr) printTable<CarModelClass>({item}, ignoreFields);
    });
    printMenu(title, actions, "Back");
}

void UserInterface::carMenu() {
    vector<string> ignoreFields = {"id", "model_id"};
    string title = "Car";
    vector<pair<string, std::function<void()>>> actions;
    actions.emplace_back("Add", [](){
        auto model = searchEngine<CarModel, CarModelClass>({"id"});
        if (model == nullptr) return;
        auto car = addEngine<CarClass>({"id", "model_id", "mark"});
        if (car == nullptr) return;
        (*car)["model_id"] = (*model)["id"];
        (*car)["mark"] = (*model)["mark"];
        if (!findElement(Car::instance().elements(), car)) {
            (*car)["id"] = generateId();
            Car::instance().add(car);
            showBanner(B_SUCCESS);
        } else {
            delete car;
            showBanner(B_ALREADY_EXISTS);
        }
    });
    actions.emplace_back("Delete", [ignoreFields](){
        auto item = searchEngine<Car, CarClass>(ignoreFields);
        if (item == nullptr) return;
        auto uses = CarManager::instance().filter([item](const CarManagerClass* i) {
            return (*i)["car_id"].toString() == (*item)["id"].toString();
        });
        if (!uses.empty()) {
            showBanner(B_USED_IN);
            printTable(uses, {"car_id", "manager_id"});
            return;
        }
        Car::instance().remove(item);
        showBanner(B_SUCCESS);
    });
    actions.emplace_back("Recreate", [ignoreFields](){
        auto item = searchEngine<Car, CarClass>(ignoreFields);
        if (item == nullptr) return;
        string id = (*item)["id"].toString();
        auto model = searchEngine<CarModel, CarModelClass>({"id"});
        if (model == nullptr) return;
        auto newItem = addEngine<CarClass>({"id", "model_id", "mark"});
        if (newItem == nullptr) return;
        (*newItem)["model_id"] = (*model)["id"];
        (*newItem)["mark"] = (*model)["mark"];
        if (findElement(Car::instance().elements(), newItem)) {
            delete newItem;
            showBanner(B_ALREADY_EXISTS);
            return;
        }
        *item = *newItem;
        delete newItem;
        (*item)["id"] = id;
        for (const auto& carManager: CarManager::instance().elements()) {
            if ((*carManager)["car_id"] != id) continue;
            (*carManager)["mark"] = (*item)["mark"];
        }
        showBanner(B_SUCCESS);
    });
    actions.emplace_back("Show", [ignoreFields](){
        printTable(&Car::instance(), ignoreFields);
    });
    actions.emplace_back("Search", [ignoreFields](){
        auto item = searchEngine<Car, CarClass>(ignoreFields);
        if (item != nullptr) printTable<CarClass>({item}, ignoreFields);
    });
    printMenu(title, actions, "Back");
}

void UserInterface::managerMenu() {
    vector<string> ignoreFields = {"id"};
    string title = "Car model";
    vector<pair<string, std::function<void()>>> actions;
    actions.emplace_back("Add", [ignoreFields](){
        auto manager = addEngine<ManagerClass>(ignoreFields);
        if (manager == nullptr) return;
        if (!findElement(Manager::instance().elements(), manager)) {
            (*manager)["id"] = generateId();
            Manager::instance().add(manager);
            showBanner(B_SUCCESS);
        } else {
            delete manager;
            showBanner(B_ALREADY_EXISTS);
        }
    });
    actions.emplace_back("Delete", [ignoreFields](){
        auto item = searchEngine<Manager, ManagerClass>(ignoreFields);
        if (item == nullptr) return;
        auto uses = CarManager::instance().filter([item](const CarManagerClass* i) {
            return (*i)["manager_id"].toString() == (*item)["id"].toString();
        });
        if (!uses.empty()) {
            showBanner(B_USED_IN);
            printTable(uses, {"car_id", "manager_id"});
            return;
        }
        Manager::instance().remove(item);
        showBanner(B_SUCCESS);
    });
    actions.emplace_back("Recreate", [ignoreFields](){
        auto item = searchEngine<Manager, ManagerClass>(ignoreFields);
        if (item == nullptr) return;
        string id = (*item)["id"].toString();
        auto newItem = addEngine<ManagerClass>(ignoreFields);
        if (newItem == nullptr) return;
        if (findElement(Manager::instance().elements(), newItem)) {
            delete newItem;
            showBanner(B_ALREADY_EXISTS);
            return;
        }
        *item = *newItem;
        delete newItem;
        (*item)["id"] = id;
        for (const auto& carManager: CarManager::instance().elements()) {
            if ((*carManager)["manager_id"] != id) continue;
            (*carManager)["lastname"] = (*item)["lastname"];
            (*carManager)["city"] = (*item)["city"];
            (*carManager)["phone"] = (*item)["phone"];
        }
        showBanner(B_SUCCESS);
    });
    actions.emplace_back("Show", [ignoreFields](){
        printTable(&Manager::instance(), ignoreFields);
    });
    actions.emplace_back("Search", [ignoreFields](){
        auto item = searchEngine<Manager, ManagerClass>(ignoreFields);
        if (item != nullptr) printTable<ManagerClass>({item}, ignoreFields);
    });
    printMenu(title, actions, "Back");
}

void UserInterface::carManagerMenu() {
    vector<string> ignoreFields = {"car_id", "manager_id"};
    string title = "Car and manager";
    vector<pair<string, std::function<void()>>> actions;
    actions.emplace_back("Add", [](){
        auto manager = searchEngine<Manager, ManagerClass>({"id"});
        if (manager == nullptr) return;
        auto car = searchEngine<Car, CarClass>({"id", "model_id"});
        if (car == nullptr) return;
        CarManagerClass carManager;
        carManager["car_id"] = (*car)["id"];
        carManager["mark"] = (*car)["mark"];
        carManager["manager_id"] = (*manager)["id"];
        carManager["lastname"] = (*manager)["lastname"];
        carManager["city"] = (*manager)["city"];
        carManager["phone"] = (*manager)["phone"];
        if (!findElement(CarManager::instance().elements(), &carManager)) {
            CarManager::instance().add(carManager.clone());
            showBanner(B_SUCCESS);
        } else {
            showBanner(B_ALREADY_EXISTS);
        }
    });
    actions.emplace_back("Delete", [ignoreFields](){
        auto item = searchEngine<CarManager, CarManagerClass>(ignoreFields);
        if (item != nullptr) {
            CarManager::instance().remove(item);
            showBanner(B_SUCCESS);
        }
    });
    actions.emplace_back("Recreate", [ignoreFields](){
        auto item = searchEngine<CarManager, CarManagerClass>(ignoreFields);
        if (item == nullptr) return;
        auto manager = searchEngine<Manager, ManagerClass>({"id"});
        if (manager == nullptr) return;
        auto car = searchEngine<Car, CarClass>({"id", "model_id"});
        if (car == nullptr) return;
        CarManagerClass newItem;
        newItem["car_id"] = (*car)["id"];
        newItem["mark"] = (*car)["mark"];
        newItem["manager_id"] = (*manager)["id"];
        newItem["lastname"] = (*manager)["lastname"];
        newItem["city"] = (*manager)["city"];
        newItem["phone"] = (*manager)["phone"];
        *item = newItem;
        showBanner(B_SUCCESS);
    });
    actions.emplace_back("Show", [ignoreFields](){
        printTable(&CarManager::instance(), ignoreFields);
    });
    actions.emplace_back("Search", [ignoreFields](){
        auto item = searchEngine<CarManager, CarManagerClass>(ignoreFields);
        if (item != nullptr) printTable<CarManagerClass>({item}, ignoreFields);
    });
    printMenu(title, actions, "Back");
}

void UserInterface::mainMenu() {

    CarModelView carModelView;
    CarManagerView carManagerView;

    string title = "Main menu";
    vector<pair<string, std::function<void()>>> actions;
    actions.emplace_back("Managers", [](){managerMenu();});
    actions.emplace_back("Models", [](){carModelMenu();});
    actions.emplace_back("Cars", [](){carMenu();});
    actions.emplace_back("Cars and managers", [](){carManagerMenu();});
    actions.emplace_back("Show cars and models", [carModelView](){carModelView.print();});
    actions.emplace_back("Show cars and managers", [carManagerView](){carManagerView.print();});
    actions.emplace_back("Save all", [](){
        CarModel::instance().save();
        Car::instance().save();
        Manager::instance().save();
        CarManager::instance().save();
        showBanner(B_SUCCESS);
    });
    actions.emplace_back("Load all", [](){
        CarModel::instance().load();
        Car::instance().load();
        Manager::instance().load();
        CarManager::instance().load();
        showBanner(B_SUCCESS);
    });
    printMenu(title, actions, "Exit");
}

template<typename TableType>
TableType *UserInterface::addEngine(const vector<std::string> &ignoreFields) {
    showBanner(B_CREATE);
    string s;
    auto m = new TableType();

    for (const auto& field: (*m).fields()) {
        if (contains(ignoreFields, field.first) ||
            field.second.type == et_empty ||
            field.second.type == et_array ||
            field.second.type == et_object) continue;
        (*m)[field.first] = askForValue(field.second.name, field.second.type);
    }

    return m;
}

template<typename Table, typename TableType>
TableType *UserInterface::searchEngine(const vector<std::string> &ignoreFields) {
    auto filter = [](list<Object*>& elements, const std::function<bool(const Object*)>& f) {
        for (auto i = elements.begin(); i != elements.end(); i++) {
            if (!f(*i)) elements.erase(i--);
        }
    };

    showBanner(B_SEARCH);
    auto elements = Table::instance().elements();
    string s;
    ElementValue find;
    list<Object*> result;

    for (const auto& i: elements) {
        result.emplace_back(i);
    }

    for (const auto& field: TableType().fields()) {
        if (contains(ignoreFields, field.first) ||
            field.second.type == et_empty ||
            field.second.type == et_array ||
            field.second.type == et_object) continue;
        if (!result.empty()) printTable(result, ignoreFields);
        find = askForValue(field.second.name, field.second.type);
        filter(result, [field, find](const Object* i){
            return (*i)[field.first].toString() == find.toString();
        });
        if (result.empty()) {
            showBanner(B_NOT_FOUND);
            return nullptr;
        } else if (result.size() == 1) {
            return dynamic_cast<TableType*>(result.front());
        }
    }
    if (result.empty()) {
        showBanner(B_NOT_FOUND);
        return nullptr;
    }
    return dynamic_cast<TableType*>(result.front());
}