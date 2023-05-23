#ifndef CARSTORE_INCLUDES_H
#define CARSTORE_INCLUDES_H


// XML parsing and serialization
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
namespace pt = boost::property_tree;

// UUID generator
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
using namespace boost::uuids;

// Tokenizer
#include <boost/tokenizer.hpp>
typedef boost::tokenizer<boost::char_separator<char>> tokenizer;

// Standard libraries
#include <iostream>
#include <map>
#include <list>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;

inline string generateId() {
    static random_generator gen;
    return to_string(gen());
}

#endif //CARSTORE_INCLUDES_H
