#include "JSONParser.h"
#include <cctype>
#include <ostream>
#include <sstream>
#include <stdexcept>

JsonParseError::JsonParseError(const std::string &json, size_t position, const std::string &message) {
  std::stringstream ss;
  ss << "Error at position " << position << ": " << message << std::endl;
  ss << json << std::endl;
  ss << std::string(position, ' ') << '^';
  errorMessage = ss.str();
}

const char *JsonParseError::what() const noexcept {
  return errorMessage.c_str();
}

JsonValue::JsonValue() : type(Type::Null) {}
JsonValue::JsonValue(bool value) : type(Type::Boolean), boolValue(value) {}
JsonValue::JsonValue(int value) : type(Type::Number), numberValue(static_cast<double>(value)) {}
JsonValue::JsonValue(double value) : type(Type::Number), numberValue(value) {}
JsonValue::JsonValue(const std::string &value) : type(Type::String), stringValue(value) {}
JsonValue::JsonValue(const char *value) : type(Type::String), stringValue(value) {}
JsonValue::JsonValue(const JsonObject &value) : type(Type::Object), objectValue(value) {}
JsonValue::JsonValue(const JsonArray &value) : type(Type::Array), arrayValue(value) {}

JsonValue::Type JsonValue::getType() const { return type; }
bool JsonValue::isNull() const { return type == Type::Null; }
bool JsonValue::asBool() const { return boolValue; }
int JsonValue::asInt() const { return static_cast<int>(numberValue); }
double JsonValue::asDouble() const { return numberValue; }
const std::string &JsonValue::asString() const { return stringValue; }
const JsonObject &JsonValue::asObject() const { return objectValue; }
const JsonArray &JsonValue::asArray() const { return arrayValue; }

JsonParser::JsonParser() : pos(0) {}

void JsonParser::skipWhitespace() {
  while (pos < json.length() && std::isspace(json[pos])) {
    pos++;
  }
}

char JsonParser::nextChar() {
  return json[pos++];
}

char JsonParser::peekChar() {
  return json[pos];
}

// Returns true if the next characters in the input match the given pattern.
// If the pattern is not matched, the input position is reset to the start.
// Otherwise the input position is left at the end of the pattern.
bool JsonParser::match(const std::string &pattern) {
  size_t start = pos;
  for (char c : pattern) {
    if (pos >= json.length() || json[pos++] != c) {
      pos = start;
      return false;
    }
  }
  return true;
}

JsonValue JsonParser::parse(const std::string &json) {
  this->json = json;
  pos = 0;
  return parseValue();
}

JsonValue JsonParser::parseValue() {
  skipWhitespace();
  if (pos >= json.length()) {
    throw JsonParseError(json, pos, "Unexpected end of JSON input");
  }

  char c = peekChar();
  switch (c) {
    case '{': return parseObject();
    case '[': return parseArray();
    case '"': return parseString();
    case 't': return parseBoolean();
    case 'f': return parseBoolean();
    case 'n': parseNull();
      return JsonValue();
    default: return parseNumber();
  }
}

JsonObject JsonParser::parseObject() {
  JsonObject obj;
  skipWhitespace();
  if (nextChar() != '{') {
    throw JsonParseError(json, pos, "Expected '{'");
  }

  do {
    skipWhitespace();
    if (peekChar() != '"') {
      throw JsonParseError(json, pos, "Expected '\"'");
    }
    std::string key = parseString();
    skipWhitespace();
    if (nextChar() != ':') {
      throw JsonParseError(json, pos, "Expected ':'");
    }
    JsonValue value = parseValue();
    obj.insert({key, value});
    skipWhitespace();
  } while (match(","));

  if (nextChar() != '}') {
    throw JsonParseError(json, pos, "Expected '}'");
  }
  return obj;
}

JsonArray JsonParser::parseArray() {
  JsonArray arr;
  skipWhitespace();
  if (nextChar() != '[') {
    throw JsonParseError(json, pos, "Expected '['");
  }

  do {
    arr.push_back(parseValue());
    skipWhitespace();
  } while (match(","));

  if (nextChar() != ']') {
    throw JsonParseError(json, pos, "Expected ']'");
  }
  return arr;
}

std::string JsonParser::parseString() {
  std::string str;
  if (nextChar() != '"') {
    throw JsonParseError(json, pos, "Expected '\"'");
  }
  while (pos < json.length()) {
    char c = nextChar();
    if (c == '"') {
      return str;
    } else if (c == '\\') {
      if (pos >= json.length()) {
        throw JsonParseError(json, pos, "Unexpected end of input");
      }
      c = nextChar();
      switch (c) {
        case '"': str += '"';
          break;
        case '\\': str += '\\';
          break;
        case '/': str += '/';
          break;
        case 'b': str += '\b';
          break;
        case 'f': str += '\f';
          break;
        case 'n': str += '\n';
          break;
        case 'r': str += '\r';
          break;
        case 't': str += '\t';
          break;
        default: throw JsonParseError(json, pos, "Invalid escape character");
      }
    } else {
      str += c;
    }
  }
  throw JsonParseError(json, pos, "Unexpected end of input");
}

double JsonParser::parseNumber() {
  size_t start = pos;
  if (json[pos] == '-') {
    pos++;
  }
  if (pos >= json.length() || !std::isdigit(json[pos])) {
    throw JsonParseError(json, pos, "Expected digit");
  }
  while (pos < json.length() && std::isdigit(json[pos])) {
    pos++;
  }
  if (pos < json.length() && json[pos] == '.') {
    pos++;
    if (pos >= json.length() || !std::isdigit(json[pos])) {
      throw JsonParseError(json, pos, "Expected digit after '.'");
    }
    while (pos < json.length() && std::isdigit(json[pos])) {
      pos++;
    }
  }
  if (pos < json.length() && (json[pos] == 'e' || json[pos] == 'E')) {
    pos++;
    if (pos < json.length() && (json[pos] == '+' || json[pos] == '-')) {
      pos++;
    }
    if (pos >= json.length() || !std::isdigit(json[pos])) {
      throw JsonParseError(json, pos, "Expected digit after 'e' or 'E'");
    }
    while (pos < json.length() && std::isdigit(json[pos])) {
      pos++;
    }
  }
  return std::stod(json.substr(start, pos - start));
}

bool JsonParser::parseBoolean() {
  if (match("true")) {
    return true;
  }
  if (match("false")) {
    return false;
  }
  throw JsonParseError(json, pos, "Expected 'true' or 'false'");
}

void JsonParser::parseNull() {
  if (!match("null")) {
    throw JsonParseError(json, pos, "Expected 'null'");
  }
}
