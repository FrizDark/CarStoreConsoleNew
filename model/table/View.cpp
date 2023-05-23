#include "View.h"

pair<std::string, std::string> getToken(std::string key) {
    vector<std::string> chunks;
    auto c = [](const std::string& s) { return s; };
    separate(key, chunks, ".");
    if (key.size() >= 2) {
        return make_pair(chunks[0], chunks[1]);
    }
    return make_pair("", "");
}

View::View(const BasicSerializable& parent, vector<JoinField> children) {
    _parent = &parent;
    _children = vector(children);
    for (const auto& element: _parent->elements()) {
        for (const auto& field: element->fields()) {
            m_fields.emplace(_parent->name() + "." + field.first, field.second);
        }
    }
    for (const auto& i: _children) {
        for (const auto& element: i.child->elements()) {
            for (const auto& field: element->fields()) {
                if (field.first == i.childField) continue;
                m_fields.emplace(i.child->name() + "." + field.first, field.second);
            }
        }
    }
}

list<Object> View::elements() const {
    list<Object> elements;

    for (const auto& li: _parent->elements()) {
        Object element;
        for (const auto& i: li->values()) {
            element[_parent->name() + "." + i.first] = i.second;
        }
        for (const auto& item: _children) {
            if ((*li)[item.parentField].type == et_empty) continue;
            auto pred = [=, this](const Object* i) {
                return (*i)[item.childField].toString() == (*li)[item.parentField].toString();
            };
            auto child = item.child->filter(pred);
            if (child.empty()) continue;
            for (const auto& i: child.front()->values()) {
                if (i.first == item.childField) continue;
                element[item.child->name() + "." + i.first] = i.second;
            }
        }
        elements.emplace_back(element);
    }
    return elements;
}

list<Object> View::filter(const std::function<bool(const Object&)>& f) const {
    list<Object> v;
    for (const auto& i: elements()) {
        if (f(i)) v.emplace_back(i);
    }
    return v;
}

void ViewPrintable::print() const {
    auto values = elements();
    auto format = [](std::string value, size_t width) {
        if (width > value.size()) value.append(width - value.size(), ' ');
        return value;
    };
    map<std::string, size_t> columns;
    string line = "|";
    for (const auto& field: m_fields) {
        if (!contains(m_printFields, field.first)) continue;
        for (const auto& value: values) {
            if (!columns.contains(field.first)) {
                columns[field.first] = field.second.name.size();
            }
            if (columns[field.first] < value[field.first].toString().size()) {
                columns[field.first] = value[field.first].toString().size();
            }
        }
        line += string(columns[field.first] + 2, '-') + "|";
    }
    cout << line << endl;
    cout << "| ";
    for (const auto& field: m_fields) {
        if (!contains(m_printFields, field.first)) continue;
        cout << format(field.second.name, columns[field.first]) << " | ";
    }
    cout << endl;
    cout << line << endl;
    for (const auto& value: values) {
        cout << "| ";
        for (const auto& field: m_fields) {
            if (!contains(m_printFields, field.first)) continue;
            cout << format(value[field.first].toString(), columns[field.first]) << " | ";
        }
        cout << endl;
    }
    cout << line << endl;
}
