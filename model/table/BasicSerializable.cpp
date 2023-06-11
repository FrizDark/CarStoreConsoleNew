#include "BasicSerializable.h"

string BasicSerializable::dataFilePath = "../data/";

void BasicSerializable::save() const {
    pt::ptree root, array;
    for(const auto& element: m_elements) {
        array.push_back(make_pair("item", saver(*element)));
    }
    root.put_child(name(), array);
    string fileName = dataFilePath + name() + "Data.xml";
    pt::write_xml(fileName, root);
}

pt::ptree BasicSerializable::saver(const Object &item) const {
    pt::ptree root, array, element;
    for (const auto& value: item.values()) {
        if (!item.fields().empty() && !item.fields().contains(value.first)) continue;
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
                    array.add_child("item" + to_string(i.type), saver(i));
                    element.clear();
                }
                root.put_child(value.first, array);
                array.clear();
                break;
            case et_object:
                root.put_child(value.first, saver(*value.second.value.object));
                break;
        }
        root.put(value.first + ".<xmlattr>.type", value.second.type);
    }
    return root;
}

pt::ptree BasicSerializable::saver(ElementValue item) const {
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
                array.put_child("item", saver(i));
                array.put("item.<xmlattr>.type", i.type);
            }
            root.add_child("array", array);
            break;
        case et_object:
            root.add_child("object", saver(*item.value.object));
            break;
    }
    return root;
}

vector<const Object*> BasicSerializable::filter(function<bool(const Object*)> f) const {
    vector<const Object*> out;
    for (auto i: m_elements) {
        if (f(i)) out.emplace_back(i);
    }
    return out;
}

vector<Object*> BasicSerializable::filter(function<bool(const Object*)> f) {
    vector<Object*> out;
    for (auto i: m_elements) {
        if (f(i)) out.emplace_back(i);
    }
    return out;
}

const Object *BasicSerializable::first(function<bool(const Object *)> f) const {
    auto result = filter(f);
    return result.empty() ? nullptr : result.front();
}

Object *BasicSerializable::first(function<bool(const Object *)> f) {
    auto result = filter(f);
    return result.empty() ? nullptr : result.front();
}