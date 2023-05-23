#ifndef CARSTORE_BASICSERIALIZABLE_H
#define CARSTORE_BASICSERIALIZABLE_H


#include "../object/Object.h"

class BasicSerializable {

protected:

    std::string m_name {};
    std::string m_path {};
    list<Object*> m_elements {};

    BasicSerializable(const std::string& name): m_name(name) {}

    pt::ptree saver(const Object& item) const;
    pt::ptree saver(ElementValue i) const;

public:

    virtual ~BasicSerializable() = default;

    static std::string dataFilePath;

    inline std::string name() const { return m_name; }
    inline list<Object*> elements() const { return m_elements; }

    inline void clear() { m_elements.clear(); }

    void save() const;

    inline virtual void add(Object* m) { m_elements.emplace_back(m); }
    inline virtual void remove(Object* m) { m_elements.remove(m); }

    virtual vector<const Object*> filter(std::function<bool(const Object*)> f) const;
    virtual vector<Object*> filter(std::function<bool(const Object*)> f);

};


#endif //CARSTORE_BASICSERIALIZABLE_H
