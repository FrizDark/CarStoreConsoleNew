#ifndef CARSTORE_CONFIGURATION_H
#define CARSTORE_CONFIGURATION_H


#include "../table/BasicTable.h"

class Configuration: private Object {

private:

    static string fileName;

    Configuration() { loadConfig(); };

public:

    static auto& instance() {
        static Configuration INSTANCE;
        return INSTANCE;
    }

    Configuration(const Configuration& obj) = delete;
    Configuration& operator=(const Configuration& obj) = delete;

    const map<string, TypeName> fields() const override;
    inline const map<string, ElementValue> values() const { return Object::values(); }

    const ElementValue& operator[](const string& key) const;
    ElementValue& operator[](const string& key);

    void saveConfig() const;
    void loadConfig();

};


#endif //CARSTORE_CONFIGURATION_H
