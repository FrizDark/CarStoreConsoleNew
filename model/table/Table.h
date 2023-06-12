#ifndef CARSTORE_TABLE_H
#define CARSTORE_TABLE_H

#include "BasicTable.h"

template<class T>
class Table: public BasicTable {

public:

    Table(const string name): BasicTable(name) {}

    bool load() {
        pt::ptree root;
        string fileName = dataFilePath + name() + "Data.xml";
        try {
            pt::read_xml(fileName, root);
        } catch (const boost::property_tree::xml_parser_error& ex) {
            return false;
        }

        clear();

        T* item = new T();

        try {
            for(const auto& element: root.get_child(name())) {
                item->decode(element.second);
                m_elements.emplace_back(item->clone());
                item->clear();
            }
        } catch(const pt::ptree_bad_path& ex) {
            return false;
        }

        return true;
    }

    inline void add(T* m) { BasicTable::add(m); }
    inline void remove(T* m) { BasicTable::remove(m); }

    list<const T*> filter(function<bool(const T*)> f) const {
        list<const T*> out;
        for (auto i: m_elements) {
            if (f(static_cast<T*>(i))) out.emplace_back(static_cast<T*>(i));
        }
        return out;
    }

    list<T*> filter(function<bool(const T*)> f) {
        list<T*> out;
        for (auto i: m_elements) {
            if (f(static_cast<T*>(i))) out.emplace_back(static_cast<T*>(i));
        }
        return out;
    }

    const T* first(function<bool(const T*)> f) const {
        auto result = filter(f);
        return result.empty() ? nullptr : result.front();
    }
    T* first(function<bool(const T*)> f) {
        auto result = filter(f);
        return result.empty() ? nullptr : result.front();
    }

};

class CarModel: public Table<CarModelClass> {

public:

    static auto& instance() {
        static CarModel table;
        return table;
    }

    CarModel(): Table<CarModelClass>("Models") {}

};

class Car: public Table<CarClass> {

public:

    static auto& instance() {
        static Car table;
        return table;
    }

    Car(): Table<CarClass>("Cars") {}

};

class Manager: public Table<ManagerClass> {

public:

    static auto& instance() {
        static Manager table;
        return table;
    }

    Manager(): Table<ManagerClass>("Managers") {}

};

class CarManager: public Table<CarManagerClass> {

public:

    static auto& instance() {
        static CarManager table;
        return table;
    }

    CarManager(): Table<CarManagerClass>("CarManagers") {}

};

#endif //CARSTORE_TABLE_H
