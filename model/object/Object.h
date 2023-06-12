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

    map<string, ElementValue> _values {};

    pt::ptree encoder(ElementValue item) const;

    ElementValue decoder(pt::ptree element, const string& data, const ElementType* field = nullptr) const;
    ElementValue decodeObject(pt::ptree root) const;
    ElementValue decodeArray(pt::ptree root) const;

public:

    Object() = default;
    Object(const Object&);

    virtual ~Object() = default;

    inline virtual const map<string, TypeName> fields() const { return {}; };
    inline const map<string, ElementValue> values() const { return _values; }
    inline void insert(pair<string, ElementValue> item) { _values.insert(item); }
    inline void erase(string key) { _values.erase(key); }
    inline void clear() { _values.clear(); }

    ElementValue& operator[](const string& key);
    const ElementValue& operator[](const string& key) const;
    Object& operator=(const Object& obj);
    inline Object* clone() const { return new Object(*this); };

    bool operator==(const Object&) const;

    string toString() const;

    pt::ptree encode() const;
    bool decode(const pt::ptree& root);
    void save(const string& path) const;
    bool load(const string& path);

};

class CarModelClass: public Object {
public:

    CarModelClass() {}
    CarModelClass(const CarModelClass& obj): Object(obj) {}

    const map<string, TypeName> fields() const override;
    inline CarModelClass * clone() const { return new CarModelClass(*this); }

};

class CarClass: public Object {
public:

    CarClass() {}
    CarClass(const CarClass& obj): Object(obj) {}

    const map<string, TypeName> fields() const override;
    inline CarClass * clone() const { return new CarClass(*this); }

};

class ManagerClass: public Object {
public:

    ManagerClass() {}
    ManagerClass(const ManagerClass& obj): Object(obj) {}

    const map<string, TypeName> fields() const override;
    inline ManagerClass * clone() const { return new ManagerClass(*this); }

};

class CarManagerClass: public Object {
public:

    CarManagerClass() {}
    CarManagerClass(const CarManagerClass& obj): Object(obj) {}

    const map<string, TypeName> fields() const override;
    inline CarManagerClass * clone() const { return new CarManagerClass(*this); }

};


#endif //CARSTORE_OBJECT_H
