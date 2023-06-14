#ifndef CARSTORE_BASICTABLE_H
#define CARSTORE_BASICTABLE_H


#include "../object/Object.h"

class BasicTable {

protected:

    string m_name {};
    list<Object*> m_elements {};

    BasicTable(const string& name): m_name(name) {}

public:

    virtual ~BasicTable() = default;

    static string dataFilePath;

    inline const string& name() const { return m_name; }
    inline const list<Object*>& elements() const { return m_elements; }

    inline void clear() { m_elements.clear(); }

    void save() const;

    inline virtual void add(Object* m) { m_elements.emplace_back(m); }
    inline virtual void remove(Object* m) { m_elements.remove(m); }

    virtual vector<const Object*> filter(const function<bool(const Object*)>& f) const;
    virtual vector<Object*> filter(const function<bool(const Object*)>& f);

    virtual const Object* first(const function<bool(const Object*)>& f) const;
    virtual Object* first(const function<bool(const Object*)>& f);

};


#endif //CARSTORE_BASICTABLE_H
