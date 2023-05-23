#ifndef CARSTORE_VIEW_H
#define CARSTORE_VIEW_H

#include "Serializable.h"

struct JoinField {
    const BasicSerializable* child;
    std::string parentField;
    std::string childField;
};

class View {

private:

    const BasicSerializable* _parent;
    vector<JoinField> _children;

protected:

    map<std::string, TypeName> m_fields;

public:

    View(const BasicSerializable& parent, vector<JoinField> children);

    inline map<std::string, TypeName> fields() const { return m_fields; }
    list<Object> elements() const;

    list<Object> filter(const std::function<bool(const Object&)>& f) const;

};

class ViewPrintable: public View {

protected:

    vector<std::string> m_printFields;

public:

    ViewPrintable(const BasicSerializable& parent, vector<JoinField> children): View(parent, children) {};

    virtual void print() const;

};

class CarModelView: public ViewPrintable {
public:
    CarModelView(): ViewPrintable(Car::instance(), {
        {&CarModel::instance(), "model_id", "id"}
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
    CarManagerView(): ViewPrintable(CarManager::instance(), {
        {&Car::instance(), "car_id", "id"},
        {&Manager::instance(), "manager_id", "id"}
    }) {
        m_printFields.emplace_back(Car::instance().name() + ".mark");
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
