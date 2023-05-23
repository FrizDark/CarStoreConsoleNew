#ifndef CARSTORE_USERINTERFACE_H
#define CARSTORE_USERINTERFACE_H


#include "../table/View.h"

class UserInterface {

private:

    enum Banner {
        B_SUCCESS,
        B_NOT_FOUND,
        B_TRY_AGAIN,
        B_SEARCH,
        B_CREATE,
        B_USED_IN,
        B_ALREADY_EXISTS
    };

    static bool findElement(list<Object*> items, Object* item);
    static ElementValue askForValue(const string& title, ElementType type);

    static void showBanner(Banner type);
    static void printMenu(
            const string& title,
            const vector<pair<string, std::function<void()>>>& actions,
            const string& exitName);
    template <typename T>
    static void printTable(const vector<T*>& values, const vector<string>& ignore);
    static void printTable(const BasicSerializable* table, const vector<string>& ignore);

    static void carModelMenu();
    static void carMenu();
    static void managerMenu();
    static void carManagerMenu();

    template <typename TableType>
    static TableType* addEngine(const vector<string>& ignore);
    template <typename Table, typename TableType>
    static TableType* searchEngine(const vector<string>& ignore);

public:

    UserInterface() = delete;

    static void mainMenu();

};


#endif //CARSTORE_USERINTERFACE_H
