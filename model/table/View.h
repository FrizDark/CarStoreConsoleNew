#ifndef CARSTORE_VIEW_H
#define CARSTORE_VIEW_H

#include "Table.h"

class View {

public:

    struct JoinField {

        const BasicTable* parent;
        string idField;
        list<pair<string, JoinField>> children;

        JoinField() = default;
        JoinField(const BasicTable* parent, const string& idField, const list<pair<string, JoinField>>& children = {});

        JoinField(const JoinField& obj);
        JoinField& operator=(const JoinField& obj);

    };

private:

    JoinField _join;

    void createJoins(const JoinField& join, bool noId = false);
    Object createObjects(const View::JoinField& join, const function<bool(const Object*)>& pred) const;

protected:

    map<string, TypeName> m_fields;

public:

    View(const JoinField& join);

    inline map<string, TypeName> fields() const { return m_fields; }
    list<Object> elements() const;

    list<Object> filter(const function<bool(const Object&)>& f) const;

};

class ViewPrintable: public View {

protected:

    vector<string> m_printFields;

public:

    ViewPrintable(const JoinField& joins): View(joins) {};

    vector<string> ignored() const;

    void print(const list<Object>& values) const;
    void print() const;
    void print(const function<bool(const Object&)>& f) const;

};

class CarModelView: public ViewPrintable {
public:
    CarModelView(): ViewPrintable(
            JoinField {
                &Car::instance(), "id",
                list { make_pair<string, JoinField>("model_id", { &CarModel::instance(), "id" }) }
            }) {
        m_printFields.emplace_back(Car::instance().name() + ".color");
        m_printFields.emplace_back(Car::instance().name() + ".price");

        m_printFields.emplace_back(CarModel::instance().name() + ".mark");
        m_printFields.emplace_back(CarModel::instance().name() + ".model");
        m_printFields.emplace_back(CarModel::instance().name() + ".type");
    }
};

class CarManagerView: public ViewPrintable {
public:
    CarManagerView(): ViewPrintable(
            JoinField {
                &CarManager::instance(), "id",
                list {
                    make_pair<string, JoinField>("car_id", {
                        &Car::instance(), "id",
                        list { make_pair<string, JoinField>("model_id", { &CarModel::instance(), "id" }) }
                    }),
                    make_pair<string, JoinField>("manager_id", { &Manager::instance(), "id" })
                }
            }) {
        m_printFields.emplace_back(CarModel::instance().name() + ".mark");
        m_printFields.emplace_back(CarModel::instance().name() + ".model");
        m_printFields.emplace_back(CarModel::instance().name() + ".type");

        m_printFields.emplace_back(Car::instance().name() + ".color");
        m_printFields.emplace_back(Car::instance().name() + ".price");

        m_printFields.emplace_back(Manager::instance().name() + ".firstname");
        m_printFields.emplace_back(Manager::instance().name() + ".lastname");
        m_printFields.emplace_back(Manager::instance().name() + ".age");
        m_printFields.emplace_back(Manager::instance().name() + ".city");
        m_printFields.emplace_back(Manager::instance().name() + ".phone");
    }
};


#endif //CARSTORE_VIEW_H
