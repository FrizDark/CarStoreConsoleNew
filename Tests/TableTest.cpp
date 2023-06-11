#include "../model/table/Table.h"

#include <boost/test/unit_test.hpp>

class TestModel: public Object {
public:
    TestModel() = default;
    TestModel(const TestModel& obj): Object(obj) {}

    const map<string, TypeName> fields() const override {
        map<string, TypeName> f;
        TypeName tn = {et_empty, "Empty"};
        f.insert(make_pair("empty", tn));
        tn = {et_boolean, "Boolean"};
        f.insert(make_pair("boolean", tn));
        tn = {et_number, "Number"};
        f.insert(make_pair("number", tn));
        tn = {et_string, "String"};
        f.insert(make_pair("string", tn));
        tn = {et_array, "Array"};
        f.insert(make_pair("array", tn));
        tn = {et_object, "Object"};
        f.insert(make_pair("object", tn));
        return f;
    }
    inline TestModel * clone() const { return new TestModel(*this); }
};

class TestSerializable: public Table<TestModel> {
public:
    static auto& instance() {
        static TestSerializable test;
        return test;
    }

    TestSerializable(): Table<TestModel>("Test") { dataFilePath = "./"; }
};

BOOST_AUTO_TEST_SUITE(SerializableTestSuite)

struct SerializableFixture {
    TestSerializable test;
};

    BOOST_FIXTURE_TEST_CASE(SerializableInit, SerializableFixture) {
        TestModel m1, m2;
        m1["empty"] = {};
        m1["boolean"] = true;
        m1["number"] = 12.34;
        m1["string"] = "Test text";
        m1["array"] = vector<ElementValue>{10, false, "123"};

        m2["empty"] = {};
        m2["boolean"] = false;
        m2["number"] = 56.789;
        m2["string"] = "Some random text";
        m2["object"] = m1;

        test.add(m1.clone());
        test.add(m2.clone());

        BOOST_CHECK_EQUAL(test.elements().size(), 2);
        test.save();
    }

    BOOST_FIXTURE_TEST_CASE(SerializableLoad, SerializableFixture) {
        test.load();
        BOOST_CHECK_EQUAL(test.elements().size(), 2);
        for (const auto& element: test.elements()) {
            BOOST_CHECK_EQUAL(element->values().size(), 5);
            if ((*element)["number"].value.number == 12.34) {
                BOOST_CHECK_EQUAL((*element)["boolean"].value.boolean, true);
                BOOST_CHECK_EQUAL(*(*element)["string"].value.string, "Test text");
                BOOST_CHECK_EQUAL((*element)["array"].value.array->front().value.number, 10);
            } else {
                BOOST_CHECK_EQUAL((*element)["boolean"].value.boolean, false);
                BOOST_CHECK_EQUAL((*element)["number"].value.number, 56.789);
                BOOST_CHECK_EQUAL(*(*element)["string"].value.string, "Some random text");
                BOOST_CHECK_EQUAL(*(*(*element)["object"].value.object)["string"].value.string, "Test text");
            }
        }
    }

BOOST_AUTO_TEST_SUITE_END();