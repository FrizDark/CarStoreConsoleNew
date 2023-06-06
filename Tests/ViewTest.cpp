#include "../model/table/View.h"

#include <boost/test/unit_test.hpp>

class TestModel1: public Object {
public:
    TestModel1() = default;
    TestModel1(const TestModel1& obj): Object(obj) {}
    const map<std::string, TypeName> fields() const override {
        map<std::string, TypeName> f;
        f.insert(std::make_pair("id", TypeName {et_number, "ID"}));
        f.insert(std::make_pair("itemId", TypeName {et_number, "Item ID"}));
        f.insert(std::make_pair("name", TypeName {et_string, "Name"}));
        return f;
    }
    inline Object * clone() const { return new TestModel1(*this); }
};
class TestModel2: public Object {
public:
    TestModel2() = default;
    TestModel2(const TestModel2& obj): Object(obj) {}
    const map<std::string, TypeName> fields() const override {
        map<std::string, TypeName> f;
        f.insert(std::make_pair("id", TypeName {et_number, "ID"}));
        f.insert(std::make_pair("name", TypeName {et_string, "Name"}));
        f.insert(std::make_pair("code", TypeName {et_number, "Code"}));
        return f;
    }
    inline Object * clone() const { return new TestModel2(*this); }
};
class TestTable1: public Serializable<TestModel1> {
public:
    static auto& instance() {
        static TestTable1 table;
        return table;
    }
    TestTable1(): Serializable<TestModel1>("Test1") {}
};
class TestTable2: public Serializable<TestModel2> {
public:
    static auto& instance() {
        static TestTable2 table;
        return table;
    }
    TestTable2(): Serializable<TestModel2>("Test2") {}
};

class TestView: public View {
public:
    TestView(): View(
            JoinField {
                &TestTable1::instance(), "id",
                list { make_pair<string, JoinField>("itemId", { &TestTable2::instance(), "id" }) }
            }) {}
};

BOOST_AUTO_TEST_SUITE(ViewTestSuite)

    struct ViewFixture {
        TestModel1 t11, t12;
        TestModel2 t21, t22;
        ViewFixture() {
            t11["id"] = 1;
            t11["itemId"] = 11;
            t11["name"] = "Test 1";
            t12["id"] = 2;
            t12["itemId"] = 12;
            t12["name"] = "Test 2";
            t21["id"] = 11;
            t21["name"] = "Child 1";
            t21["code"] = 123;
            t22["id"] = 12;
            t22["name"] = "Child 2";
            t22["code"] = 456;
        }
    };

    BOOST_FIXTURE_TEST_CASE(ViewTestElements, ViewFixture) {
        TestTable1::instance().add(&t11);
        TestTable1::instance().add(&t12);
        TestTable2::instance().add(&t21);
        TestTable2::instance().add(&t22);
        TestView view;
        BOOST_CHECK_EQUAL(view.fields().size(), 5);
        BOOST_CHECK_EQUAL(view.elements().size(), 2);
        for (const auto& i: view.elements()) {
            if (i["Test1.id"].value.number == 1) {
                BOOST_CHECK_EQUAL(i["Test1.itemId"].value.number, 11);
                BOOST_CHECK_EQUAL(*i["Test1.name"].value.string, "Test 1");
                BOOST_CHECK_EQUAL(*i["Test2.name"].value.string, "Child 1");
                BOOST_CHECK_EQUAL(i["Test2.code"].value.number, 123);
            } else {
                BOOST_CHECK_EQUAL(i["Test1.itemId"].value.number, 12);
                BOOST_CHECK_EQUAL(*i["Test1.name"].value.string, "Test 2");
                BOOST_CHECK_EQUAL(*i["Test2.name"].value.string, "Child 2");
                BOOST_CHECK_EQUAL(i["Test2.code"].value.number, 456);
            }
        }
    }

BOOST_AUTO_TEST_SUITE_END();