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
        const string& exitName,
        const function<void()>& onAppear
        ) {
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
        if (onAppear != NULL) onAppear();
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
    actions.emplace_back("Edit", [ignoreFields](){
        auto item = searchEngine<CarModel, CarModelClass>(ignoreFields);
        if (item == nullptr) return;
        auto copy = item->clone();
        string id = (*copy)["id"].toString();
        copy->erase("id");
        editEngine(copy, ignoreFields, [&](){
            printTable<CarModelClass>({copy}, ignoreFields);
        });
        if (findElement(CarModel::instance().elements(), copy)) {
            delete copy;
            showBanner(B_ALREADY_EXISTS);
            return;
        }
        (*copy)["id"] = id;
        *item = *copy;
        delete copy;
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
    CarModelView carModelView;
    vector<string> ignoreFields = {"id", "model_id"};
    string title = "Car";
    vector<pair<string, std::function<void()>>> actions;
    actions.emplace_back("Add", [](){
        auto model = searchEngine<CarModel, CarModelClass>({"id"});
        if (model == nullptr) return;
        auto car = addEngine<CarClass>({"id", "model_id", "mark", "model"});
        if (car == nullptr) return;
        (*car)["model_id"] = (*model)["id"];
        if (!findElement(Car::instance().elements(), car)) {
            (*car)["id"] = generateId();
            Car::instance().add(car);
            showBanner(B_SUCCESS);
        } else {
            delete car;
            showBanner(B_ALREADY_EXISTS);
        }
    });
    actions.emplace_back("Delete", [=](){
        auto viewItem = searchEngine(carModelView);
        if (viewItem == nullopt) return;
        auto item = Car::instance().first([viewItem](const CarClass* item){
            return (*item)["id"] == viewItem.value()[Car::instance().name() + ".id"];
        });
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
    actions.emplace_back("Edit", [=](){
        auto viewItem = searchEngine(carModelView);
        if (viewItem == nullopt) return;
        auto item = Car::instance().first([viewItem](const CarClass* item){
            return (*item)["id"] == viewItem.value()[Car::instance().name() + ".id"];
        });
        if (item == nullptr) return;
        auto copy = item->clone();
        string id = (*copy)["id"].toString();
        copy->erase("id");
        map<string, function<void()>> specifics;
        specifics["model_id"] = [=, &copy]() {
            auto model = searchEngine<CarModel, CarModelClass>({"id"});
            if (model == nullptr) return;
            (*copy)["model_id"] = (*model)["id"];
        };
        editEngine(copy, ignoreFields, [&](){
            auto model = CarModel::instance().first([copy](const CarModelClass* item) {
                return (*item)["id"] == (*copy)["model_id"];
            });
            if (model != nullptr) printTable<CarModelClass>({model}, {"id"});
            printTable<CarClass>({copy}, ignoreFields);
        }, specifics);
        if (findElement(Car::instance().elements(), copy)) {
            delete copy;
            showBanner(B_ALREADY_EXISTS);
            return;
        }
        (*copy)["id"] = id;
        *item = *copy;
        delete copy;
    });
    actions.emplace_back("Show", [carModelView](){
        carModelView.print();
    });
    actions.emplace_back("Search", [=](){
        auto item = searchEngine(carModelView);
        if (item != nullopt) carModelView.print({*item});
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
    actions.emplace_back("Edit", [ignoreFields](){
        auto item = searchEngine<Manager, ManagerClass>(ignoreFields);
        if (item == nullptr) return;
        auto copy = item->clone();
        string id = (*copy)["id"].toString();
        copy->erase("id");
        editEngine(copy, ignoreFields, [&](){
            printTable<ManagerClass>({copy}, ignoreFields);
        });
        if (findElement(Manager::instance().elements(), copy)) {
            delete copy;
            showBanner(B_ALREADY_EXISTS);
            return;
        }
        (*copy)["id"] = id;
        *item = *copy;
        delete copy;
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
    CarManagerView carManagerView;
    vector<string> ignoreFields = {"car_id", "manager_id"};
    string title = "Car and manager";
    vector<pair<string, std::function<void()>>> actions;
    actions.emplace_back("Add", [](){
        auto manager = searchEngine<Manager, ManagerClass>({"id"});
        if (manager == nullptr) return;
        auto viewCar = searchEngine(CarModelView());
        if (viewCar == nullopt) return;
        auto car = Car::instance().first([viewCar](const CarClass* item) {
            return (*item)["id"] == viewCar.value()[Car::instance().name() + ".id"];
        });
        if (car == nullptr) return;
        CarManagerClass carManager;
        carManager["car_id"] = (*car)["id"];
        carManager["manager_id"] = (*manager)["id"];
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
    actions.emplace_back("Edit", [=](){
        auto viewItem = searchEngine(carManagerView);
        if (viewItem == nullopt) return;
        auto item = CarManager::instance().first([viewItem](const CarManagerClass* item){
            return (*item)["car_id"] == viewItem.value()[CarManager::instance().name() + ".car_id"] &&
                (*item)["manager_id"] == viewItem.value()[CarManager::instance().name() + ".manager_id"];
        });
        if (item == nullptr) return;
        auto copy = item->clone();
        map<string, function<void()>> specifics;
        specifics["car_id"] = [=, &copy]() {
            auto viewCar = searchEngine(CarModelView());
            if (viewCar == nullopt) return;
            auto car = Car::instance().first([viewCar](const CarClass* item) {
                return (*item)["id"] == viewCar.value()[Car::instance().name() + ".id"];
            });
            if (car == nullptr) return;
            (*copy)["car_id"] = (*car)["id"];
        };
        specifics["manager_id"] = [=, &copy]() {
            auto manager = searchEngine<Manager, ManagerClass>({"id"});
            if (manager == nullptr) return;
            (*copy)["manager_id"] = (*manager)["id"];
        };
        editEngine(copy, ignoreFields, [&](){
            auto car = Car::instance().first([copy](const CarClass* item) {
                return (*item)["id"] == (*copy)["car_id"];
            });
            if (car != nullptr) CarModelView().print([car](const Object item){
                return item[Car::instance().name() + ".id"] == (*car)["id"];
            });
            auto manager = Manager::instance().first([copy](const ManagerClass* item) {
                return (*item)["id"] == (*copy)["manager_id"];
            });
            if (manager != nullptr) printTable<ManagerClass>({manager}, {"id"});
        }, specifics);
        if (findElement(CarManager::instance().elements(), copy)) {
            delete copy;
            showBanner(B_ALREADY_EXISTS);
            return;
        }
        *item = *copy;
        delete copy;
    });
    actions.emplace_back("Show", [carManagerView](){
        carManagerView.print();
    });
    actions.emplace_back("Search", [=](){
        auto item = searchEngine(carManagerView);
        if (item != nullopt) carManagerView.print({*item});
    });
    printMenu(title, actions, "Back");
}

void UserInterface::mainMenu() {

    string title = "Main menu";
    vector<pair<string, std::function<void()>>> actions;
    actions.emplace_back("Managers", [](){managerMenu();});
    actions.emplace_back("Models", [](){carModelMenu();});
    actions.emplace_back("Cars", [](){carMenu();});
    actions.emplace_back("Cars and managers", [](){carManagerMenu();});
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

template<typename View>
optional<Object> UserInterface::searchEngine(View view) {
    auto filter = [](list<Object>& elements, const std::function<bool(const Object&)>& f) {
        for (auto i = elements.begin(); i != elements.end(); i++) {
            if (!f(*i)) elements.erase(i--);
        }
    };

    showBanner(B_SEARCH);
    auto elements = view.elements();
    auto ignoreFields = view.ignored();
    string s;
    ElementValue find;
    list<Object> result;

    for (const auto& i: elements) {
        result.emplace_back(i);
    }

    for (const auto& field: view.fields()) {
        if (contains(ignoreFields, field.first) ||
            field.second.type == et_empty ||
            field.second.type == et_array ||
            field.second.type == et_object) continue;
        if (!result.empty()) view.print(result);
        find = askForValue(field.second.name, field.second.type);
        filter(result, [field, find](const Object& i){
            return i[field.first].toString() == find.toString();
        });
        if (result.empty()) {
            showBanner(B_NOT_FOUND);
            return nullopt;
        } else if (result.size() == 1) {
            return result.front();
        }
    }
    if (result.empty()) {
        showBanner(B_NOT_FOUND);
        return nullopt;
    }
    return result.front();
}

template<typename TableType>
void UserInterface::editEngine(
        TableType *item,
        const vector<std::string> &ignoreFields,
        const function<void()> showItem,
        const map<std::string, function<void()>> &specifics
        ) {
    vector<pair<string, function<void()>>> actions;
    for (const auto& field: item->fields()) {
        if (specifics.contains(field.first)) {
            actions.emplace_back(field.second.name, specifics.at(field.first));
        } else {
            if (contains(ignoreFields, field.first) ||
                field.second.type == et_empty ||
                field.second.type == et_array ||
                field.second.type == et_object) continue;
            actions.emplace_back(field.second.name, [=, &item](){
                (*item)[field.first] = askForValue(field.second.name, field.second.type);
            });
        }
    }
    printMenu("Edit", actions, "Done", showItem);
}