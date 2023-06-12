#ifndef CARSTORE_DEMODATA_H
#define CARSTORE_DEMODATA_H

#include "../table/Table.h"

namespace demo {

    string getId();
    vector<string> generateIds(int count);

    void createModels();
    void createCars();
    void createManagers();
    void createCarManagers();

    void create();

}


#endif //CARSTORE_DEMODATA_H
