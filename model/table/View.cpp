#include "View.h"

pair<string, string> getToken(string key) {
    vector<string> chunks;
    auto c = [](const string& s) { return s; };
    separate(key, chunks, ".");
    if (key.size() >= 2) {
        return make_pair(chunks[0], chunks[1]);
    }
    return make_pair("", "");
}

View::JoinField::JoinField(
        const BasicSerializable *parent,
        const string &idField,
        list<pair<string, View::JoinField>> children) {
    this->parent = parent;
    this->idField = idField;
    this->children = list(children);
}

View::JoinField::JoinField(const View::JoinField &obj) {
    this->parent = obj.parent;
    this->idField = obj.idField;
    this->children = list(obj.children);
}

View::JoinField &View::JoinField::operator=(const View::JoinField &obj) {
    this->parent = obj.parent;
    this->idField = obj.idField;
    this->children = list(obj.children);
    return *this;
}

void View::createJoins(const View::JoinField &join, bool noId) {
    for (const auto& element: join.parent->elements()) {
        for (const auto& field: element->fields()) {
            if (noId && field.first == join.idField) continue;
            m_fields.emplace(join.parent->name() + "." + field.first, field.second);
        }
    }
    for (const auto& child: join.children) {
        createJoins(child.second, true);
    }
}

View::View(const JoinField& join) {
    _join = join;
    createJoins(join);
}

Object View::createObjects(const View::JoinField &join, const function<bool(const Object*)>& pred) const {
    auto element = join.parent->filter(pred);
    if (element.empty()) return {};
    Object obj;
    for (const auto& value: element.front()->values()) {
        if (value.first == join.idField) continue;
        obj[join.parent->name() + "." + value.first] = value.second;
    }
    for (const auto& child: join.children) {
        if ((*element.front())[child.first].type == et_empty) break;
        auto childObj = createObjects(child.second, [=](const Object* i) {
            return (*i)[child.second.idField] == (*element.front())[child.first];
        });
        for (const auto& value: childObj.values()) {
            obj[value.first] = value.second;
        }
    }
    return obj;
}

list<Object> View::elements() const {
    list<Object> elements;
    for (const auto& element: _join.parent->elements()) {
        Object obj;
        for (const auto& value: element->values()) {
            obj[_join.parent->name() + "." + value.first] = value.second;
        }
        for (const auto& child: _join.children) {
            if ((*element)[child.first].type == et_empty) break;
            auto childObj = createObjects(child.second, [=](const Object* i) {
                return (*i)[child.second.idField] == (*element)[child.first];
            });
            for (const auto& value: childObj.values()) {
                obj[value.first] = value.second;
            }
        }
        elements.emplace_back(obj);
    }
    return elements;
}

list<Object> View::filter(const function<bool(const Object&)>& f) const {
    list<Object> v;
    for (const auto& i: elements()) {
        if (f(i)) v.emplace_back(i);
    }
    return v;
}

vector<string> ViewPrintable::ignored() const {
    vector<string> ignored;
    for (const auto& i: m_fields) {
        if (!contains(m_printFields, i.first)) {
            ignored.emplace_back(i.first);
        }
    }
    return ignored;
}

void ViewPrintable::print(const list<Object> &values) const {
    auto format = [](string value, size_t width) {
        if (width > value.size()) value.append(width - value.size(), ' ');
        return value;
    };
    map<string, size_t> columns;
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

void ViewPrintable::print(function<bool(const Object &)> f) const {
    print(filter(f));
}

void ViewPrintable::print() const {
    print(elements());
}
