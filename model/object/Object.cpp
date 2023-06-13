#include "Object.h"
#include <filesystem>
namespace fs = std::filesystem;

ElementType getType(int id) {
    switch (id) {
        default: return et_empty;
        case 1: return et_boolean;
        case 2: return et_number;
        case 3: return et_string;
        case 4: return et_array;
        case 5: return et_object;
    }
}

ElementValue::ElementValue() {
    type = et_empty;
}

ElementValue::ElementValue(bool i) {
    type = et_boolean;
    value.boolean = i;
}

ElementValue::ElementValue(int i) {
    type = et_number;
    value.number = i;
}

ElementValue::ElementValue(double i) {
    type = et_number;
    value.number = i;
}

ElementValue::ElementValue(const string& i) {
    type = et_string;
    value.string = new string(i);
}

ElementValue::ElementValue(const char i[]) {
    type = et_string;
    value.string = new string(i);
}

ElementValue::ElementValue(const vector<ElementValue>& i) {
    type = et_array;
    value.array = new vector(i);
}

ElementValue::ElementValue(const Object& i) {
    type = et_object;
    value.object = i.clone();
}

ElementValue::ElementValue(const ElementValue& obj) {
    type = obj.type;
    switch (type) {
        case et_boolean:
            value.boolean = obj.value.boolean;
            break;
        case et_number:
            value.number = obj.value.number;
            break;
        case et_string:
            value.string = new string(*obj.value.string);
            break;
        case et_array:
            value.array = new vector(*obj.value.array);
            break;
        case et_object:
            value.object = obj.value.object->clone();
            break;
        default:
            break;
    }
}

ElementValue &ElementValue::operator=(const ElementValue &obj) {
    type = obj.type;
    switch (type) {
        case et_boolean:
            value.boolean = obj.value.boolean;
            break;
        case et_number:
            value.number = obj.value.number;
            break;
        case et_string:
            value.string = new string(*obj.value.string);
            break;
        case et_array:
            value.array = new vector(*obj.value.array);
            break;
        case et_object:
            value.object = obj.value.object->clone();
            break;
        default:
            break;
    }
    return *this;
}

bool ElementValue::operator==(const ElementValue &obj) const {
    if (type != obj.type) return false;
    switch (type) {
        case et_boolean: return value.boolean == obj.value.boolean;
        case et_number: return value.number == obj.value.number;
        case et_string: return *value.string == *obj.value.string;
        case et_array: return *value.array == *obj.value.array;
        case et_object: return *value.object == *obj.value.object;
        default: return true;
    }
}

ElementValue::~ElementValue() {
    switch (type) {
        case et_string:
            delete value.string;
            break;
        case et_array:
            delete value.array;
            break;
        case et_object:
            delete value.object;
            break;
        default:
            break;
    }
}

string ElementValue::toString() const {
    auto convertArray = [](const ElementValue& item) {
        return item.toString();
    };
    string num;
    switch (type) {
        case et_empty:
            return "null";
        case et_boolean:
            return value.boolean ? "true" : "false";
        case et_number:
            num = to_string(value.number);
            while (num.back() == '0') num.pop_back();
            if (num.back() == '.') num.pop_back();
            return num;
        case et_string:
            return *value.string;
        case et_array:
            return join(*value.array, convertArray, ", ");
        case et_object:
            return value.object->toString();
    }
}

Object::Object(const Object & obj) {
    m_values = map<string, ElementValue>(obj.m_values);
}

ElementValue &Object::operator[](const string& key) {
    auto value = m_values.find(key);
    if (value == m_values.end()) {
        m_values[key] = ElementValue();
    }
    return m_values[key];
}

const ElementValue &Object::operator[](const string& key) const {
    auto value = m_values.find(key);
    if (value == m_values.end()) {
        throw out_of_range("Key not found.");
    }
    return m_values.at(key);
}

Object &Object::operator=(const Object &obj) {
    m_values = map<string, ElementValue>(obj.m_values);
    return *this;
}

bool Object::operator==(const Object &obj) const {
    for (const auto& field: fields()) {
        if (values().contains(field.first) && obj.values().contains(field.first)) {
            if (values().at(field.first) != obj[field.first]) return false;
        }
    }
    return true;
}

string Object::toString() const {
    auto convert = [](pair<string, ElementValue> i) {
        return "(" + i.first + ":" + i.second.toString() + ")";
    };
    return join(values(), convert, ", ");
}

pt::ptree Object::encode() const {
    pt::ptree root, array, element;
    for (const auto& value: values()) {
        if (!fields().empty() && !fields().contains(value.first)) continue;
        switch(value.second.type) {
            case et_empty:
                root.put(value.first, nullptr);
                break;
            case et_boolean:
                root.put(value.first, value.second.value.boolean);
                break;
            case et_number:
                root.put(value.first, value.second.value.number);
                break;
            case et_string:
                root.put(value.first, *value.second.value.string);
                break;
            case et_array:
                for (const auto& i: *value.second.value.array) {
                    array.add_child("item" + to_string(i.type), encoder(i));
                    element.clear();
                }
                root.put_child(value.first, array);
                array.clear();
                break;
            case et_object:
                root.put_child(value.first, value.second.value.object->encode());
                break;
        }
        root.put(value.first + ".<xmlattr>.type", value.second.type);
    }
    return root;
}

pt::ptree Object::encoder(const ElementValue &item) const {
    pt::ptree root, array;
    switch(item.type) {
        case et_empty:
            root.put_value(NULL);
            break;
        case et_boolean:
            root.put_value(item.value.boolean);
            break;
        case et_number:
            root.put_value(item.value.number);
            break;
        case et_string:
            root.put_value(*item.value.string);
            break;
        case et_array:
            for (const auto& i: *item.value.array) {
                array.put_child("item", encoder(i));
                array.put("item.<xmlattr>.type", i.type);
            }
            root.add_child("array", array);
            break;
        case et_object:
            root.add_child("object", item.value.object->encode());
            break;
    }
    return root;
}

ElementValue Object::decoder(const pt::ptree &element, const std::string &data, const ElementType *field) const {
    ElementType fieldType;
    try {
        ElementType type = getType(element.get<int>("<xmlattr>.type"));
        fieldType = field == nullptr ? type : *field;
        if (fieldType != type) return {};
    } catch (const pt::ptree_bad_path& ex) {
        if (field == nullptr) return {};
        fieldType = *field;
    }
    switch (fieldType) {
        case et_empty: return {};
        case et_boolean: return { data == "true" };
        case et_number:
            try {
                return { stod(data) };
            } catch (const invalid_argument& ex) {
                return { 0.0 };
            }
        case et_string: return { data };
        case et_array: return decodeArray(element);
        case et_object: return decodeObject(element);
    }
}

ElementValue Object::decodeObject(pt::ptree root) const {
    Object *child = new Object();
    root.erase("<xmlattr>");
    for(auto &i : root) {
        (*child)[i.first] = decoder(root.get_child(i.first), i.second.data());
    }
    return { *child };
}

ElementValue Object::decodeArray(pt::ptree root) const {
    vector<ElementValue> values;
    root.erase("<xmlattr>");
    for (const auto& i: root) {
        try {
            ElementType type = getType(stoi(i.first.substr(4, i.first.size())));
            values.emplace_back(decoder(root.get_child(i.first), i.second.data(), &type));
        } catch (const invalid_argument& ex) {
            // TODO: process this error
        }
    }
    return values.empty() ? ElementValue() : ElementValue(values);
}

bool Object::decode(const pt::ptree &root) {
    auto f = fields();
    clear();
    try {
        for (const auto& value: root) {
            auto result = f.find(value.first);
            (*this)[value.first] = decoder(
                    root.get_child(value.first),
                    value.second.data(),
                    result == fields().end() ? &result->second.type : nullptr
            );
        }
        return true;
    } catch(const pt::ptree_bad_path& ex) {
        return false;
    }
}

void Object::save(const std::string &path, const std::string &fileName) const {
    pt::ptree root;
    root.put_child("item", encode());
    if (!fs::is_directory(path)) {
        if (fs::create_directory(path)) {
            pt::write_xml(path + fileName, root);
        }
    } else {
        pt::write_xml(path, root);
    }
}

bool Object::load(const std::string &path) {
    pt::ptree root;
    try {
        pt::read_xml(path, root);
        return decode(root.get_child("item"));
    } catch (const boost::property_tree::xml_parser_error& ex) {
        return false;
    } catch (const boost::property_tree::ptree_bad_path& ex) {
        return false;
    }
}

const map<string, TypeName> CarModelClass::fields() const {
    map<string, TypeName> f;
    f.insert(make_pair("id", TypeName {et_string, "ID"}));
    f.insert(make_pair("mark", TypeName {et_string, "Mark"}));
    f.insert(make_pair("model", TypeName {et_string, "Model"}));
    f.insert(make_pair("type", TypeName {et_string, "Type"}));
    return f;
}

const map<string, TypeName> CarClass::fields() const {
    map<string, TypeName> f;
    f.insert(make_pair("id", TypeName {et_string, "ID"}));
    f.insert(make_pair("model_id", TypeName {et_string, "Model"}));
    f.insert(make_pair("color", TypeName {et_string, "Color"}));
    f.insert(make_pair("price", TypeName {et_number, "Price"}));
    return f;
}

const map<string, TypeName> ManagerClass::fields() const {
    map<string, TypeName> f;
    f.insert(make_pair("id", TypeName {et_string, "ID"}));
    f.insert(make_pair("firstname", TypeName {et_string, "First name"}));
    f.insert(make_pair("lastname", TypeName {et_string, "Last name"}));
    f.insert(make_pair("age", TypeName {et_number, "Age"}));
    f.insert(make_pair("city", TypeName {et_string, "City"}));
    f.insert(make_pair("phone", TypeName {et_string, "Phone"}));
    return f;
}

const map<string, TypeName> CarManagerClass::fields() const {
    map<string, TypeName> f;
    f.insert(make_pair("car_id", TypeName {et_string, "Car"}));
    f.insert(make_pair("manager_id", TypeName {et_string, "Manager"}));
    return f;
}