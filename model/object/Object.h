#ifndef CARSTORE_OBJECT_H
#define CARSTORE_OBJECT_H


#include "../common/Extensions.h"

enum ElementType {
    et_empty = 0,
    et_boolean = 1,
    et_number = 2,
    et_string = 3,
    et_array = 4,
    et_object = 5
};

ElementType getType(int id);

struct TypeName {
    ElementType type;
    string name;
};

class Object;

struct ElementValue {

    ElementType type;
    union {
        bool boolean;
        double number;
        string* string;
        vector<ElementValue>* array;
        Object* object;
    } value {};

    ElementValue();
    ElementValue(bool i);
    ElementValue(int i);
    ElementValue(double i);
    ElementValue(const string& i);
    ElementValue(const char i[]);
    ElementValue(const vector<ElementValue>& i);
    ElementValue(const Object& i);

    ElementValue(const ElementValue& obj);
    ElementValue& operator=(const ElementValue& obj);

    bool operator==(const ElementValue& obj) const;

    ~ElementValue();

    string toString() const;

};

class Object {

private:

    pt::ptree encoder(const ElementValue& item) const;

    ElementValue decoder(const pt::ptree& element, const string& data, const ElementType* field = nullptr) const;
    ElementValue decodeObject(pt::ptree root) const;
    ElementValue decodeArray(pt::ptree root) const;

protected:

    map<string, ElementValue> m_values {};

public:

    Object() = default;
    Object(const Object&);

    virtual ~Object() = default;

    inline virtual const map<string, TypeName> fields() const { return {}; };
    inline const map<string, ElementValue> values() const { return m_values; }
    inline void erase(const string& key) { m_values.erase(key); }
    inline void clear() { m_values.clear(); }

    ElementValue& operator[](const string& key);
    const ElementValue& operator[](const string& key) const;
    Object& operator=(const Object& obj);
    inline virtual Object* clone() const { return new Object(*this); };

    bool operator==(const Object&) const;

    string toString() const;

    pt::ptree encode() const;
    bool decode(const pt::ptree& root);
    void save(const string& path) const;
    bool load(const string& path);

};

class CarModelClass: public Object {
public:

    CarModelClass() = default;
    CarModelClass(const CarModelClass& obj): Object(obj) {}

    const map<string, TypeName> fields() const override;
    inline CarModelClass * clone() const override { return new CarModelClass(*this); }

};

class CarClass: public Object {
public:

    CarClass() = default;
    CarClass(const CarClass& obj): Object(obj) {}

    const map<string, TypeName> fields() const override;
    inline CarClass * clone() const override { return new CarClass(*this); }

};

class ManagerClass: public Object {
public:

    ManagerClass() = default;
    ManagerClass(const ManagerClass& obj): Object(obj) {}

    const map<string, TypeName> fields() const override;
    inline ManagerClass * clone() const override { return new ManagerClass(*this); }

};

class CarManagerClass: public Object {
public:

    CarManagerClass() = default;
    CarManagerClass(const CarManagerClass& obj): Object(obj) {}

    const map<string, TypeName> fields() const override;
    inline CarManagerClass * clone() const override { return new CarManagerClass(*this); }

};


#endif //CARSTORE_OBJECT_H
