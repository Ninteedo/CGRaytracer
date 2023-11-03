#include "../src/JSONParser.h"
#include <iostream>
#include <cassert>

void testSimpleValues() {
    JsonParser parser;
    JsonValue value;

    // Test null
    value = parser.parse("null");
    assert(value.isNull());

    // Test boolean
    value = parser.parse("true");
    assert(value.asBool() == true);

    value = parser.parse("false");
    assert(value.asBool() == false);

    // Test numbers
    value = parser.parse("42");
    assert(value.asInt() == 42);

    value = parser.parse("-42");
    assert(value.asInt() == -42);

    value = parser.parse("3.14");
    assert(value.asDouble() == 3.14);

    // Test strings
    value = parser.parse("\"Hello World\"");
    assert(value.asString() == "Hello World");
}

void testObjectsAndArrays() {
    JsonParser parser;
    JsonValue value;

    // Test object
    value = parser.parse(R"({"name":"John", "age":30})");
    assert(value.asObject().at("name").asString() == "John");
    assert(value.asObject().at("age").asInt() == 30);

    // Test array
    value = parser.parse("[1, 2, 3, 4]");
    assert(value.asArray().size() == 4);
    assert(value.asArray()[0].asInt() == 1);
    assert(value.asArray()[1].asInt() == 2);
    assert(value.asArray()[2].asInt() == 3);
    assert(value.asArray()[3].asInt() == 4);
}

void testNestedStructures() {
    JsonParser parser;
    JsonValue value;

    // Test nested structures
    value = parser.parse(R"({"person":{"name":"John", "age":30}, "numbers":[1, 2, 3]})");
    assert(value.asObject().at("person").asObject().at("name").asString() == "John");
    assert(value.asObject().at("person").asObject().at("age").asInt() == 30);
    assert(value.asObject().at("numbers").asArray()[0].asInt() == 1);
    assert(value.asObject().at("numbers").asArray()[1].asInt() == 2);
    assert(value.asObject().at("numbers").asArray()[2].asInt() == 3);
}

void testErrorConditions() {
    JsonParser parser;
    bool caught;

    // Test malformed JSON
    caught = false;
    try {
        parser.parse("{invalid_json}");
    }
    catch (const std::exception&) {
        caught = true;
    }
    assert(caught);

    // Test incomplete JSON
    caught = false;
    try {
        parser.parse("{\"name\":");
    }
    catch (const std::exception&) {
        caught = true;
    }
    assert(caught);
}

int main() {
    testSimpleValues();
    testObjectsAndArrays();
    testNestedStructures();
    testErrorConditions();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}
