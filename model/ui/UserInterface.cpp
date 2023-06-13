#include "UserInterface.h"
#include "../configuration/DemoData.h"

bool UserInterface::findElement(list<Object*> items, Object* item) {
    return std::any_of(items.begin(), items.end(), [item](const auto& i) {
        return *item == *i;
    });
}

inline bool isInvalid(const vector<string>& ignoreFields, const pair<string, TypeName>& field) {
    return contains(ignoreFields, field.first) ||
        field.second.type == et_empty ||
        field.second.type == et_array ||
        field.second.type == et_object;
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
    cout << "### #   ";
    switch (type) {
        case B_SUCCESS:
            cout << "SUCCESS";
            break;
        case B_NOT_FOUND:
            cout << "NOT FOUND";
            break;
        case B_TRY_AGAIN:
            cout << "TRY AGAIN";
            break;
        case B_SEARCH:
            cout << "SEARCH";
            break;
        case B_CREATE:
            cout << "CREATE";
            break;
        case B_USED_IN:
            cout << "USED IN";
            break;
        case B_ALREADY_EXISTS:
            cout << "ALREADY EXISTS";
            break;
        case B_PRESS_ANY_KEY_TO_CONTINUE:
            cout << "PRESS ANY KEY TO CONTINUE";
            break;
    }
    cout << "   # ###" << endl;
}

void UserInterface::printMenu(
        const std::string &title,
        const vector<pair<string, std::function<void()>>> &actions,
        const string& exitName,
        const function<void()>& onAppear
        ) {
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

    while (true) {
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
            showBanner(B_PRESS_ANY_KEY_TO_CONTINUE);
            cin.get();
            continue;
        }
        if (in == 0) return;
        else if (in > 0 && in <= actions.size()) {
            actions[in - 1].second();
            showBanner(B_PRESS_ANY_KEY_TO_CONTINUE);
            cin.get();
        } else {
            showBanner(B_TRY_AGAIN);
            showBanner(B_PRESS_ANY_KEY_TO_CONTINUE);
            cin.get();
        }
    }

}

template <typename T>
void UserInterface::printTable(const list<T *> &values, const vector<std::string> &ignoreFields) {
    if (values.empty()) {
        showBanner(B_NOT_FOUND);
        return;
    }
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
            auto size = (*value)[field.first].toString().size();
            if (columns[field.first] < size) {
                columns[field.first] = size;
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

void UserInterface::printTable(const BasicTable* table, const vector<std::string> &ignoreFields) {
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
            CarModelView().print([uses](const Object& item){
                for (const auto& i: uses) {
                    return item[Car::instance().name() + ".id"] == (*i)["id"];
                }
                return false;
            });
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
            CarManagerView().print([uses](const Object& item){
                for (const auto& i: uses) {
                    return item[CarManager::instance().name() + ".car_id"] == (*i)["car_id"];
                }
                return false;
            });
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
            CarManagerView().print([uses](const Object& item){
                for (const auto& i: uses) {
                    return item[CarManager::instance().name() + ".manager_id"] == (*i)["manager_id"];
                }
                return false;
            });
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
    actions.emplace_back("Delete", [=](){
        auto viewItem = searchEngine(carManagerView);
        if (viewItem == nullopt) return;
        auto item = CarManager::instance().first([viewItem](const CarManagerClass* item){
            return (*item)["car_id"] == (*viewItem)[CarManager::instance().name() + ".car_id"] &&
                (*item)["manager_id"] == (*viewItem)[CarManager::instance().name() + ".manager_id"];
        });
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
        if (!CarModel::instance().load()) return;
        if (!Car::instance().load()) return;
        if (!Manager::instance().load()) return;
        if (!CarManager::instance().load()) return;
        showBanner(B_SUCCESS);
    });
    actions.emplace_back("Configuration", [](){ configurationMenu(); });
    printMenu(title, actions, "Exit");
}

void UserInterface::configurationMenu() {
    string title = "Configuration";
    vector<pair<string, std::function<void()>>> actions;
    actions.emplace_back("Switch save on exit", [](){
        toggle(Configuration::instance()["saveOnExit"].value.boolean);
        Configuration::instance().saveConfig();
        showBanner(B_SUCCESS);
    });
    actions.emplace_back("Switch load on start", [](){
        toggle(Configuration::instance()["loadOnStart"].value.boolean);
        Configuration::instance().saveConfig();
        showBanner(B_SUCCESS);
    });
    actions.emplace_back("Fill files with demo data", [](){
        demo::create();
        showBanner(B_SUCCESS);
    });
    map<string, function<string(const ElementValue&)>> specifics;
    printMenu(title, actions, "Exit", [](){
        printTable(list{&Configuration::instance()}, {});
    });
}

template<typename TableType>
TableType *UserInterface::addEngine(const vector<std::string> &ignoreFields) {
    showBanner(B_CREATE);
    string s;
    auto m = new TableType();

    for (const auto& field: (*m).fields()) {
        if (isInvalid(ignoreFields, field)) continue;
        (*m)[field.first] = askForValue(field.second.name, field.second.type);
    }

    return m;
}

Object *UserInterface::searchEngine(
        const list<Object *> &elements,
        const map<std::string, TypeName> &fields,
        const vector<std::string> &ignoreFields,
        const function<void(const list<Object*>&)>& print
) {
    auto filter = [](list<Object*>& elements, const std::function<bool(const Object*)>& f) {
        for (auto i = elements.begin(); i != elements.end(); i++) {
            if (!f(*i)) elements.erase(i--);
        }
    };

    showBanner(B_SEARCH);
    string s;
    ElementValue find;
    list<Object*> result = list(elements);

    for (const auto& field: fields) {
        if (isInvalid(ignoreFields, field)) continue;
        if (result.empty()) {
            showBanner(B_NOT_FOUND);
            return nullptr;
        } else print(result);
        find = askForValue(field.second.name, field.second.type);
        filter(result, [field, find](const Object* i){
            return (*i)[field.first].toString() == find.toString();
        });
        if (result.size() == 1) {
            return result.front();
        }
    }
    if (result.empty()) {
        showBanner(B_NOT_FOUND);
        return nullptr;
    }
    return result.front();
}

template<typename Table, typename TableType>
TableType *UserInterface::searchEngine(const vector<std::string> &ignoreFields) {
    auto print = [ignoreFields](const list<Object*>& items) {
        printTable(items, ignoreFields);
    };
    return dynamic_cast<TableType*>(searchEngine(Table::instance().elements(), TableType().fields(), ignoreFields, print));
}

template<typename View>
optional<Object> UserInterface::searchEngine(View view) {
    auto elements = view.elements();
    list<Object*> el;
    for (auto& i: elements) el.push_back(&i);
    auto print = [view](const list<Object*>& items) {
        view.print(convert<Object*, Object>(items, [](Object* item){ return *item; }));
    };
    auto item = searchEngine(el, view.fields(), view.ignored(), print);
    if (item == nullptr) return nullopt;
    else return *item;
}

template<typename TableType>
void UserInterface::editEngine(
        TableType *item,
        const vector<std::string> &ignoreFields,
        const function<void()>& showItem,
        const map<std::string, function<void()>> &specifics
        ) {
    vector<pair<string, function<void()>>> actions;
    for (const auto& field: item->fields()) {
        if (specifics.contains(field.first)) {
            actions.emplace_back(field.second.name, specifics.at(field.first));
        } else {
            if (isInvalid(ignoreFields, field)) continue;
            actions.emplace_back(field.second.name, [=, &item](){
                (*item)[field.first] = askForValue(field.second.name, field.second.type);
            });
        }
    }
    printMenu("Edit", actions, "Done", showItem);
}