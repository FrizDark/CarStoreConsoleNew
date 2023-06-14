#ifndef CARSTORE_USERINTERFACE_H
#define CARSTORE_USERINTERFACE_H


#include "../table/View.h"
#include "../configuration/Configuration.h"

class UserInterface {

private:

    enum Banner {
        B_SUCCESS,
        B_NOT_FOUND,
        B_TRY_AGAIN,
        B_SEARCH,
        B_CREATE,
        B_USED_IN,
        B_ALREADY_EXISTS,
        B_PRESS_ANY_KEY_TO_CONTINUE
    };

    static bool findElement(list<Object*> items, Object* item);
    static ElementValue askForValue(
            const string& title,
            ElementType type,
            const function<bool(ElementValue)>& check = NULL
    );

    static void showBanner(Banner type);
    static void printMenu(
            const string& title,
            const vector<pair<string, function<void()>>>& actions,
            const string& exitName,
            const function<void()>& onAppear = NULL
    );
    template <typename T>
    static void printTable(const list<T*>& values, const vector<string>& ignoreFields);
    static void printTable(const BasicTable* table, const vector<string>& ignoreFields);

    static void carModelMenu();
    static void carMenu();
    static void managerMenu();
    static void carManagerMenu();

    static void configurationMenu();

    template <typename TableType>
    static TableType* addEngine(const vector<string>& ignoreFields);
    static Object* searchEngine(
            const list<Object*>& elements,
            const map<string, TypeName>& fields,
            const vector<string>& ignoreFields,
            const function<void(const list<Object*>&)>& print
    );
    template <typename Table, typename TableType>
    static TableType* searchEngine(const vector<string>& ignoreFields);
    template <typename View>
    static optional<Object> searchEngine(View view);
    template <typename TableType>
    static void editEngine(
            TableType* item,
            const vector<string>& ignoreFields,
            const function<void()>& showItem,
            const map<string, function<void()>>& specifics = {}
    );

public:

    UserInterface() = delete;

    static void mainMenu();

};


#endif //CARSTORE_USERINTERFACE_H
