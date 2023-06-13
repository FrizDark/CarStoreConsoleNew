#include "Configuration.h"

string Configuration::fileName = "configuration.xml";

const map<string, TypeName> Configuration::fields() const {
    map<string, TypeName> f;
    f.insert(make_pair("saveOnExit", TypeName {et_boolean, "Save on exit"}));
    f.insert(make_pair("loadOnStart", TypeName {et_boolean, "Load on start"}));
    return f;
}

const ElementValue &Configuration::operator[](const std::string &key) const {
    return Object::operator[](key);
}

ElementValue &Configuration::operator[](const std::string &key) {
    return Object::operator[](key);
}

void Configuration::saveConfig() const {
    save(BasicTable::dataFilePath, fileName);
}

void Configuration::loadConfig() {
    if (!load(BasicTable::dataFilePath + fileName)) {
        (*this)["saveOnExit"] = false;
        (*this)["loadOnStart"] = true;
        saveConfig();
    }
}