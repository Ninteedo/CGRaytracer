#pragma once
#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <string>
#include <map>
#include <vector>

class JsonParseError : public std::exception {
 public:
  JsonParseError(const std::string &json, size_t position, const std::string &message);

  [[nodiscard]] const char *what() const noexcept override;

 private:
  std::string errorMessage;
};

class JsonValue;

typedef std::map<std::string, JsonValue> JsonObject;
typedef std::vector<JsonValue> JsonArray;

class JsonValue {
 public:
  enum Type {
    Null,
    Boolean,
    Number,
    String,
    Object,
    Array
  };

  JsonValue();
  explicit JsonValue(bool value);
  explicit JsonValue(int value);
  explicit JsonValue(double value);
  explicit JsonValue(std::string value);
  explicit JsonValue(const char *value);
  explicit JsonValue(JsonObject value);
  explicit JsonValue(JsonArray value);

  [[nodiscard]] Type getType() const;
  [[nodiscard]] bool isNull() const;
  [[nodiscard]] bool asBool() const;
  [[nodiscard]] int asInt() const;
  [[nodiscard]] double asDouble() const;
  [[nodiscard]] const std::string &asString() const;
  [[nodiscard]] const JsonObject &asObject() const;
  [[nodiscard]] const JsonArray &asArray() const;

 private:
  Type type;
  union {
    bool boolValue;
    double numberValue;
  };
  std::string stringValue;
  JsonObject objectValue;
  JsonArray arrayValue;
};

class JsonParser {
 public:
  JsonParser();
  JsonValue parse(const std::string &jsonInput);
  JsonValue parseFile(const std::string &filename);

 private:
  size_t pos;
  std::string json;

  void skipWhitespace();
  char nextChar();
  char peekChar();
  bool match(const std::string &pattern);

  JsonValue parseValue();
  JsonObject parseObject();
  JsonArray parseArray();
  std::string parseString();
  double parseNumber();
  bool parseBoolean();
  void parseNull();
};

#endif // JSON_PARSER_H
