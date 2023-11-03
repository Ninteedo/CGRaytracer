#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <string>
#include <map>
#include <vector>

class JsonParseError : public std::exception {
 public:
  JsonParseError(const std::string &json, size_t position, const std::string &message);

  virtual const char *what() const noexcept override;

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
  JsonValue(bool value);
  JsonValue(int value);
  JsonValue(double value);
  JsonValue(const std::string &value);
  JsonValue(const char *value);
  JsonValue(const JsonObject &value);
  JsonValue(const JsonArray &value);

  Type getType() const;
  bool isNull() const;
  bool asBool() const;
  int asInt() const;
  double asDouble() const;
  const std::string &asString() const;
  const JsonObject &asObject() const;
  const JsonArray &asArray() const;

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
  JsonValue parse(const std::string &json);
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
