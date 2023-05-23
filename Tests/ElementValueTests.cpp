#include "../model/object/Object.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(ElementValueSuite)

    struct ElementValueFixture {
        ElementValue ev;
    };

    BOOST_FIXTURE_TEST_CASE(elementValueEmpty, ElementValueFixture) {
        ev = ElementValue();
        BOOST_CHECK_EQUAL(ev.type, et_empty);
    }

    BOOST_FIXTURE_TEST_CASE(elementValueBoolean, ElementValueFixture) {
        ev = ElementValue(true);
        BOOST_CHECK_EQUAL(ev.value.boolean, true);
        BOOST_CHECK_EQUAL(ev.type, et_boolean);
    }

    BOOST_FIXTURE_TEST_CASE(elementValueInteger, ElementValueFixture) {
        ev = ElementValue(10);
        BOOST_CHECK_EQUAL(ev.value.number, 10);
        BOOST_CHECK_EQUAL(ev.type, et_number);
    }

    BOOST_FIXTURE_TEST_CASE(elementValueDouble, ElementValueFixture) {
        ev = ElementValue(25.1234);
        BOOST_CHECK_EQUAL(ev.value.number, 25.1234);
        BOOST_CHECK_EQUAL(ev.type, et_number);
    }

    BOOST_FIXTURE_TEST_CASE(elementValueString, ElementValueFixture) {
        ev = ElementValue("TEST STRING");
        BOOST_CHECK_EQUAL(*ev.value.string, "TEST STRING");
        BOOST_CHECK_EQUAL(ev.type, et_string);
    }

    BOOST_FIXTURE_TEST_CASE(elementValueArray, ElementValueFixture) {
        std::vector<ElementValue> v = {10, true, "Test"};
        ev = ElementValue(v);
        BOOST_CHECK_EQUAL((*ev.value.array)[0].toString(), v[0].toString());
        BOOST_CHECK_EQUAL((*ev.value.array)[1].toString(), v[1].toString());
        BOOST_CHECK_EQUAL((*ev.value.array)[2].toString(), v[2].toString());
        BOOST_CHECK_EQUAL(ev.type, et_array);
    }

    BOOST_FIXTURE_TEST_CASE(elementValueObject, ElementValueFixture) {
        Object dc;
        dc["a"] = 10;
        dc["b"] = true;
        dc["c"] = "Test";
        ev = ElementValue(dc);
        BOOST_CHECK_EQUAL((*ev.value.object)["a"].value.number, 10);
        BOOST_CHECK_EQUAL((*ev.value.object)["b"].toString(), "true");
        BOOST_CHECK_EQUAL((*ev.value.object)["c"].toString(), "Test");
        BOOST_CHECK_EQUAL(ev.type, et_object);
    }

BOOST_AUTO_TEST_SUITE_END();