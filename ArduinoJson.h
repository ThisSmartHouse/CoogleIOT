// Copyright Benoit Blanchon 2014-2017
// MIT License
//
// Arduino JSON library
// https://bblanchon.github.io/ArduinoJson/
// If you like this project, please add a star!

#pragma once

#include <stddef.h>  // for size_t
#include <stdint.h>  // for uint8_t
#include <string.h>
#ifndef ARDUINOJSON_EMBEDDED_MODE
#if defined(ARDUINO) || defined(__IAR_SYSTEMS_ICC__)
#define ARDUINOJSON_EMBEDDED_MODE 1
#else
#define ARDUINOJSON_EMBEDDED_MODE 0
#endif
#endif
#if ARDUINOJSON_EMBEDDED_MODE
#ifndef ARDUINOJSON_USE_DOUBLE
#define ARDUINOJSON_USE_DOUBLE 0
#endif
#ifndef ARDUINOJSON_USE_LONG_LONG
#define ARDUINOJSON_USE_LONG_LONG 0
#endif
#ifndef ARDUINOJSON_USE_INT64
#define ARDUINOJSON_USE_INT64 0
#endif
#ifndef ARDUINOJSON_ENABLE_STD_STRING
#define ARDUINOJSON_ENABLE_STD_STRING 0
#endif
#ifndef ARDUINOJSON_ENABLE_STD_STREAM
#define ARDUINOJSON_ENABLE_STD_STREAM 0
#endif
#ifndef ARDUINOJSON_DEFAULT_NESTING_LIMIT
#define ARDUINOJSON_DEFAULT_NESTING_LIMIT 10
#endif
#else  // ARDUINOJSON_EMBEDDED_MODE
#ifndef ARDUINOJSON_USE_DOUBLE
#define ARDUINOJSON_USE_DOUBLE 1
#endif
#ifndef ARDUINOJSON_USE_LONG_LONG
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
#define ARDUINOJSON_USE_LONG_LONG 1
#else
#define ARDUINOJSON_USE_LONG_LONG 0
#endif
#endif
#ifndef ARDUINOJSON_USE_INT64
#if defined(_MSC_VER) && _MSC_VER <= 1700
#define ARDUINOJSON_USE_INT64 1
#else
#define ARDUINOJSON_USE_INT64 0
#endif
#endif
#ifndef ARDUINOJSON_ENABLE_STD_STRING
#define ARDUINOJSON_ENABLE_STD_STRING 1
#endif
#ifndef ARDUINOJSON_ENABLE_STD_STREAM
#define ARDUINOJSON_ENABLE_STD_STREAM 1
#endif
#ifndef ARDUINOJSON_DEFAULT_NESTING_LIMIT
#define ARDUINOJSON_DEFAULT_NESTING_LIMIT 50
#endif
#endif  // ARDUINOJSON_EMBEDDED_MODE
#ifdef ARDUINO
#ifndef ARDUINOJSON_ENABLE_ARDUINO_STRING
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 1
#endif
#ifndef ARDUINOJSON_ENABLE_ARDUINO_STREAM
#define ARDUINOJSON_ENABLE_ARDUINO_STREAM 1
#endif
#else  // ARDUINO
#ifndef ARDUINOJSON_ENABLE_ARDUINO_STRING
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 0
#endif
#ifndef ARDUINOJSON_ENABLE_ARDUINO_STREAM
#define ARDUINOJSON_ENABLE_ARDUINO_STREAM 0
#endif
#endif  // ARDUINO
#ifndef ARDUINOJSON_ENABLE_PROGMEM
#ifdef PROGMEM
#define ARDUINOJSON_ENABLE_PROGMEM 1
#else
#define ARDUINOJSON_ENABLE_PROGMEM 0
#endif
#endif
#ifndef ARDUINOJSON_ENABLE_ALIGNMENT
#ifdef ARDUINO_ARCH_AVR
#define ARDUINOJSON_ENABLE_ALIGNMENT 0
#else
#define ARDUINOJSON_ENABLE_ALIGNMENT 1
#endif
#endif
#ifndef ARDUINOJSON_ENABLE_DEPRECATED
#define ARDUINOJSON_ENABLE_DEPRECATED 1
#endif
#ifndef ARDUINOJSON_POSITIVE_EXPONENTIATION_THRESHOLD
#define ARDUINOJSON_POSITIVE_EXPONENTIATION_THRESHOLD 1e7
#endif
#ifndef ARDUINOJSON_NEGATIVE_EXPONENTIATION_THRESHOLD
#define ARDUINOJSON_NEGATIVE_EXPONENTIATION_THRESHOLD 1e-5
#endif
#ifndef ARDUINOJSON_DOUBLE_IS_64BITS
#if /*GCC*/ (defined(__SIZEOF_DOUBLE__) && __SIZEOF_DOUBLE__ < 8) || \
    /*IAR*/ (defined(__DOUBLE__) && __DOUBLE__ < 64)
#define ARDUINOJSON_DOUBLE_IS_64BITS 0
#else
#define ARDUINOJSON_DOUBLE_IS_64BITS 1  // by default support 64-bit
#endif
#endif
#if ARDUINOJSON_USE_LONG_LONG && ARDUINOJSON_USE_INT64
#error ARDUINOJSON_USE_LONG_LONG and ARDUINOJSON_USE_INT64 cannot be set together
#endif
namespace ArduinoJson {
namespace Internals {
#if ARDUINOJSON_USE_DOUBLE
typedef double JsonFloat;
#else
typedef float JsonFloat;
#endif
}
}
namespace ArduinoJson {
namespace Internals {
#if ARDUINOJSON_USE_LONG_LONG
typedef long long JsonInteger;
typedef unsigned long long JsonUInt;
#elif ARDUINOJSON_USE_INT64
typedef __int64 JsonInteger;
typedef unsigned _int64 JsonUInt;
#else
typedef long JsonInteger;
typedef unsigned long JsonUInt;
#endif
}
}
namespace ArduinoJson {
class JsonArray;
class JsonObject;
namespace Internals {
union JsonVariantContent {
  JsonFloat asFloat;     // used for double and float
  JsonUInt asInteger;    // used for bool, char, short, int and longs
  const char* asString;  // asString can be null
  JsonArray* asArray;    // asArray cannot be null
  JsonObject* asObject;  // asObject cannot be null
};
}
}
namespace ArduinoJson {
namespace Internals {
template <typename T>
struct JsonVariantDefault {
  static T get() {
    return T();
  }
};
template <typename T>
struct JsonVariantDefault<const T> : JsonVariantDefault<T> {};
template <typename T>
struct JsonVariantDefault<T&> : JsonVariantDefault<T> {};
}
}
namespace ArduinoJson {
class JsonArray;
class JsonObject;
namespace Internals {
enum JsonVariantType {
  JSON_UNDEFINED,         // JsonVariant has not been initialized
  JSON_UNPARSED,          // JsonVariant contains an unparsed string
  JSON_STRING,            // JsonVariant stores a const char*
  JSON_BOOLEAN,           // JsonVariant stores a bool
  JSON_POSITIVE_INTEGER,  // JsonVariant stores an JsonUInt
  JSON_NEGATIVE_INTEGER,  // JsonVariant stores an JsonUInt that must be negated
  JSON_ARRAY,             // JsonVariant stores a pointer to a JsonArray
  JSON_OBJECT,            // JsonVariant stores a pointer to a JsonObject
  JSON_FLOAT              // JsonVariant stores a JsonFloat
};
}
}
namespace ArduinoJson {
namespace Internals {
template <typename T>
struct JsonVariantAs {
  typedef T type;
};
template <>
struct JsonVariantAs<char*> {
  typedef const char* type;
};
template <>
struct JsonVariantAs<JsonArray> {
  typedef JsonArray& type;
};
template <>
struct JsonVariantAs<const JsonArray> {
  typedef const JsonArray& type;
};
template <>
struct JsonVariantAs<JsonObject> {
  typedef JsonObject& type;
};
template <>
struct JsonVariantAs<const JsonObject> {
  typedef const JsonObject& type;
};
}
}
#ifdef _MSC_VER  // Visual Studio
#define FORCE_INLINE __forceinline
#define NO_INLINE __declspec(noinline)
#define DEPRECATED(msg) __declspec(deprecated(msg))
#elif defined(__GNUC__)  // GCC or Clang
#define FORCE_INLINE __attribute__((always_inline))
#define NO_INLINE __attribute__((noinline))
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
#define DEPRECATED(msg) __attribute__((deprecated(msg)))
#else
#define DEPRECATED(msg) __attribute__((deprecated))
#endif
#else  // Other compilers
#define FORCE_INLINE
#define NO_INLINE
#define DEPRECATED(msg)
#endif
namespace ArduinoJson {
namespace TypeTraits {
template <bool Condition, typename T = void>
struct EnableIf {};
template <typename T>
struct EnableIf<true, T> {
  typedef T type;
};
}
}
namespace ArduinoJson {
namespace Internals {
class DummyPrint {
 public:
  size_t print(char) {
    return 1;
  }
  size_t print(const char* s) {
    return strlen(s);
  }
};
}
}
namespace ArduinoJson {
namespace TypeTraits {
template <typename TBase, typename TDerived>
class IsBaseOf {
 protected:  // <- to avoid GCC's "all member functions in class are private"
  typedef char Yes[1];
  typedef char No[2];
  static Yes &probe(const TBase *);
  static No &probe(...);
 public:
  enum {
    value = sizeof(probe(reinterpret_cast<TDerived *>(0))) == sizeof(Yes)
  };
};
}
}
namespace ArduinoJson {
namespace TypeTraits {
template <typename T, typename U>
struct IsSame {
  static const bool value = false;
};
template <typename T>
struct IsSame<T, T> {
  static const bool value = true;
};
}
}
namespace ArduinoJson {
namespace TypeTraits {
template <typename T>
struct IsChar {
  static const bool value = IsSame<T, char>::value ||
                            IsSame<T, signed char>::value ||
                            IsSame<T, unsigned char>::value;
};
template <typename T>
struct IsChar<const T> : IsChar<T> {};
}
}
namespace ArduinoJson {
namespace TypeTraits {
template <typename T>
struct RemoveReference {
  typedef T type;
};
template <typename T>
struct RemoveReference<T&> {
  typedef T type;
};
}
}
namespace ArduinoJson {
namespace Internals {
template <typename TString, typename Enable = void>
struct StringTraits {};
template <typename TString>
struct StringTraits<const TString, void> : StringTraits<TString> {};
template <typename TString>
struct StringTraits<TString&, void> : StringTraits<TString> {};
}
}
#if ARDUINOJSON_ENABLE_ARDUINO_STREAM
#include <Stream.h>
namespace ArduinoJson {
namespace Internals {
struct ArduinoStreamTraits {
  class Reader {
    Stream& _stream;
    char _current, _next;
   public:
    Reader(Stream& stream) : _stream(stream), _current(0), _next(0) {}
    void move() {
      _current = _next;
      _next = 0;
    }
    char current() {
      if (!_current) _current = read();
      return _current;
    }
    char next() {
      if (!_next) _next = read();
      return _next;
    }
   private:
    char read() {
      char c = 0;
      _stream.readBytes(&c, 1);
      return c;
    }
  };
};
template <typename TStream>
struct StringTraits<TStream,
                    typename TypeTraits::EnableIf<TypeTraits::IsBaseOf<
                        Stream, typename TypeTraits::RemoveReference<
                                    TStream>::type>::value>::type>
    : ArduinoStreamTraits {};
}
}
#endif
namespace ArduinoJson {
namespace Internals {
template <typename TChar>
struct CharPointerTraits {
  class Reader {
    const TChar* _ptr;
   public:
    Reader(const TChar* ptr)
        : _ptr(ptr ? ptr : reinterpret_cast<const TChar*>("")) {}
    void move() {
      ++_ptr;
    }
    char current() const {
      return char(_ptr[0]);
    }
    char next() const {
      return char(_ptr[1]);
    }
  };
  static bool equals(const TChar* str, const char* expected) {
    return strcmp(reinterpret_cast<const char*>(str), expected) == 0;
  }
  template <typename Buffer>
  static char* duplicate(const TChar* str, Buffer* buffer) {
    if (!str) return NULL;
    size_t size = strlen(reinterpret_cast<const char*>(str)) + 1;
    void* dup = buffer->alloc(size);
    if (dup != NULL) memcpy(dup, str, size);
    return static_cast<char*>(dup);
  }
  static const bool has_append = false;
  static const bool has_equals = true;
  static const bool should_duplicate = false;
};
template <typename TChar>
struct StringTraits<TChar*, typename TypeTraits::EnableIf<
                                TypeTraits::IsChar<TChar>::value>::type>
    : CharPointerTraits<TChar> {};
}
}
#if ARDUINOJSON_ENABLE_PROGMEM
namespace ArduinoJson {
namespace Internals {
template <>
struct StringTraits<const __FlashStringHelper*, void> {
  class Reader {
    const char* _ptr;
   public:
    Reader(const __FlashStringHelper* ptr)
        : _ptr(reinterpret_cast<const char*>(ptr)) {}
    void move() {
      _ptr++;
    }
    char current() const {
      return pgm_read_byte_near(_ptr);
    }
    char next() const {
      return pgm_read_byte_near(_ptr + 1);
    }
  };
  static bool equals(const __FlashStringHelper* str, const char* expected) {
    return strcmp_P(expected, (PGM_P)str) == 0;
  }
  template <typename Buffer>
  static char* duplicate(const __FlashStringHelper* str, Buffer* buffer) {
    if (!str) return NULL;
    size_t size = strlen_P((PGM_P)str) + 1;
    void* dup = buffer->alloc(size);
    if (dup != NULL) memcpy_P(dup, (PGM_P)str, size);
    return static_cast<char*>(dup);
  }
  static const bool has_append = false;
  static const bool has_equals = true;
  static const bool should_duplicate = true;
};
}
}
#endif
#if ARDUINOJSON_ENABLE_STD_STREAM
#include <istream>
namespace ArduinoJson {
namespace Internals {
struct StdStreamTraits {
  class Reader {
    std::istream& _stream;
    char _current, _next;
   public:
    Reader(std::istream& stream) : _stream(stream), _current(0), _next(0) {}
    void move() {
      _current = _next;
      _next = 0;
    }
    char current() {
      if (!_current) _current = read();
      return _current;
    }
    char next() {
      if (!_next) _next = read();
      return _next;
    }
   private:
    Reader& operator=(const Reader&);  // Visual Studio C4512
    char read() {
      return _stream.eof() ? '\0' : static_cast<char>(_stream.get());
    }
  };
};
template <typename TStream>
struct StringTraits<TStream,
                    typename TypeTraits::EnableIf<TypeTraits::IsBaseOf<
                        std::istream, typename TypeTraits::RemoveReference<
                                          TStream>::type>::value>::type>
    : StdStreamTraits {};
}
}
#endif
#if ARDUINOJSON_ENABLE_STD_STRING || ARDUINOJSON_ENABLE_ARDUINO_STRING
#if ARDUINOJSON_ENABLE_ARDUINO_STRING
#include <WString.h>
#endif
#if ARDUINOJSON_ENABLE_STD_STRING
#include <string>
#endif
namespace ArduinoJson {
namespace Internals {
template <typename TString>
struct StdStringTraits {
  template <typename Buffer>
  static char* duplicate(const TString& str, Buffer* buffer) {
    if (!str.c_str()) return NULL;  // <- Arduino string can return NULL
    size_t size = str.length() + 1;
    void* dup = buffer->alloc(size);
    if (dup != NULL) memcpy(dup, str.c_str(), size);
    return static_cast<char*>(dup);
  }
  struct Reader : CharPointerTraits<char>::Reader {
    Reader(const TString& str) : CharPointerTraits<char>::Reader(str.c_str()) {}
  };
  static bool equals(const TString& str, const char* expected) {
    return 0 == strcmp(str.c_str(), expected);
  }
  static void append(TString& str, char c) {
    str += c;
  }
  static void append(TString& str, const char* s) {
    str += s;
  }
  static const bool has_append = true;
  static const bool has_equals = true;
  static const bool should_duplicate = true;
};
#if ARDUINOJSON_ENABLE_ARDUINO_STRING
template <>
struct StringTraits<String, void> : StdStringTraits<String> {};
template <>
struct StringTraits<StringSumHelper, void> : StdStringTraits<StringSumHelper> {
};
#endif
#if ARDUINOJSON_ENABLE_STD_STRING
template <>
struct StringTraits<std::string, void> : StdStringTraits<std::string> {};
#endif
}
}
#endif
namespace ArduinoJson {
namespace TypeTraits {
template <typename T, typename Enable = void>
struct IsString {
  static const bool value = false;
};
template <typename T>
struct IsString<T, typename TypeTraits::EnableIf<
                       Internals::StringTraits<T>::has_equals>::type> {
  static const bool value = Internals::StringTraits<T>::has_equals;
};
}
}
namespace ArduinoJson {
namespace Internals {
template <typename TString>
class DynamicStringBuilder {
 public:
  DynamicStringBuilder(TString &str) : _str(str) {}
  size_t print(char c) {
    StringTraits<TString>::append(_str, c);
    return 1;
  }
  size_t print(const char *s) {
    size_t initialLen = _str.length();
    StringTraits<TString>::append(_str, s);
    return _str.length() - initialLen;
  }
 private:
  DynamicStringBuilder &operator=(const DynamicStringBuilder &);
  TString &_str;
};
}
}
namespace ArduinoJson {
namespace Internals {
template <typename Print>
class IndentedPrint {
 public:
  explicit IndentedPrint(Print &p) : sink(&p) {
    level = 0;
    tabSize = 2;
    isNewLine = true;
  }
  size_t print(char c) {
    size_t n = 0;
    if (isNewLine) n += writeTabs();
    n += sink->print(c);
    isNewLine = c == '\n';
    return n;
  }
  size_t print(const char *s) {
    size_t n = 0;
    while (*s) n += print(*s++);
    return n;
  }
  void indent() {
    if (level < MAX_LEVEL) level++;
  }
  void unindent() {
    if (level > 0) level--;
  }
  void setTabSize(uint8_t n) {
    if (n < MAX_TAB_SIZE) tabSize = n & MAX_TAB_SIZE;
  }
 private:
  Print *sink;
  uint8_t level : 4;
  uint8_t tabSize : 3;
  bool isNewLine : 1;
  size_t writeTabs() {
    size_t n = 0;
    for (int i = 0; i < level * tabSize; i++) n += sink->print(' ');
    return n;
  }
  static const int MAX_LEVEL = 15;    // because it's only 4 bits
  static const int MAX_TAB_SIZE = 7;  // because it's only 3 bits
};
}
}
namespace ArduinoJson {
namespace Internals {
class Encoding {
 public:
  static char escapeChar(char c) {
    const char *p = escapeTable(false);
    while (p[0] && p[1] != c) {
      p += 2;
    }
    return p[0];
  }
  static char unescapeChar(char c) {
    const char *p = escapeTable(true);
    for (;;) {
      if (p[0] == '\0') return c;
      if (p[0] == c) return p[1];
      p += 2;
    }
  }
 private:
  static const char *escapeTable(bool excludeIdenticals) {
    return &"\"\"\\\\b\bf\fn\nr\rt\t"[excludeIdenticals ? 4 : 0];
  }
};
}
}
namespace ArduinoJson {
namespace Polyfills {
template <typename T>
bool isNaN(T x) {
  return x != x;
}
template <typename T>
bool isInfinity(T x) {
  return x != 0.0 && x * 2 == x;
}
}
}
namespace ArduinoJson {
namespace Polyfills {
template <typename T>
int16_t normalize(T& value) {
  int16_t powersOf10 = 0;
  if (value >= ARDUINOJSON_POSITIVE_EXPONENTIATION_THRESHOLD) {
#if ARDUINOJSON_DOUBLE_IS_64BITS
    if (value >= 1e256) {
      value /= 1e256;
      powersOf10 = int16_t(powersOf10 + 256);
    }
    if (value >= 1e128) {
      value /= 1e128;
      powersOf10 = int16_t(powersOf10 + 128);
    }
    if (value >= 1e64) {
      value /= 1e64;
      powersOf10 = int16_t(powersOf10 + 64);
    }
#endif
    if (value >= 1e32) {
      value /= 1e32;
      powersOf10 = int16_t(powersOf10 + 32);
    }
    if (value >= 1e16) {
      value /= 1e16;
      powersOf10 = int16_t(powersOf10 + 16);
    }
    if (value >= 1e8) {
      value /= 1e8;
      powersOf10 = int16_t(powersOf10 + 8);
    }
    if (value >= 1e4) {
      value /= 1e4;
      powersOf10 = int16_t(powersOf10 + 4);
    }
    if (value >= 1e2) {
      value /= 1e2;
      powersOf10 = int16_t(powersOf10 + 2);
    }
    if (value >= 1e1) {
      value /= 1e1;
      powersOf10 = int16_t(powersOf10 + 1);
    }
  }
  if (value > 0 && value <= ARDUINOJSON_NEGATIVE_EXPONENTIATION_THRESHOLD) {
#if ARDUINOJSON_DOUBLE_IS_64BITS
    if (value < 1e-255) {
      value *= 1e256;
      powersOf10 = int16_t(powersOf10 - 256);
    }
    if (value < 1e-127) {
      value *= 1e128;
      powersOf10 = int16_t(powersOf10 - 128);
    }
    if (value < 1e-63) {
      value *= 1e64;
      powersOf10 = int16_t(powersOf10 - 64);
    }
#endif
    if (value < 1e-31) {
      value *= 1e32;
      powersOf10 = int16_t(powersOf10 - 32);
    }
    if (value < 1e-15) {
      value *= 1e16;
      powersOf10 = int16_t(powersOf10 - 16);
    }
    if (value < 1e-7) {
      value *= 1e8;
      powersOf10 = int16_t(powersOf10 - 8);
    }
    if (value < 1e-3) {
      value *= 1e4;
      powersOf10 = int16_t(powersOf10 - 4);
    }
    if (value < 1e-1) {
      value *= 1e2;
      powersOf10 = int16_t(powersOf10 - 2);
    }
    if (value < 1e0) {
      value *= 1e1;
      powersOf10 = int16_t(powersOf10 - 1);
    }
  }
  return powersOf10;
}
}
}
#include <stdlib.h>  // for size_t
namespace ArduinoJson {
namespace TypeTraits {
template <typename T, size_t = sizeof(T)>
struct FloatTraits {};
#if ARDUINOJSON_DOUBLE_IS_64BITS
template <typename T>
struct FloatTraits<T, 8 /*64bits*/> {
  typedef int64_t mantissa_type;
  static const short mantissa_bits = 52;
  static const mantissa_type mantissa_max =
      (static_cast<mantissa_type>(1) << mantissa_bits) - 1;
  typedef int16_t exponent_type;
  static const exponent_type exponent_max = 308;
  template <typename TExponent>
  static T make_float(T m, TExponent e) {
    if (e >= 0)
      return m * (e & 1 ? 1e1 : 1) * (e & 2 ? 1e2 : 1) * (e & 4 ? 1e4 : 1) *
             (e & 8 ? 1e8 : 1) * (e & 16 ? 1e16 : 1) * (e & 32 ? 1e32 : 1) *
             (e & 64 ? 1e64 : 1) * (e & 128 ? 1e128 : 1) *
             (e & 256 ? 1e256 : 1);
    e = TExponent(-e);
    return m * (e & 1 ? 1e-1 : 1) * (e & 2 ? 1e-2 : 1) * (e & 4 ? 1e-4 : 1) *
           (e & 8 ? 1e-8 : 1) * (e & 16 ? 1e-16 : 1) * (e & 32 ? 1e-32 : 1) *
           (e & 64 ? 1e-64 : 1) * (e & 128 ? 1e-128 : 1) *
           (e & 256 ? 1e-256 : 1);
  }
  static T nan() {
    uint64_t x = uint64_t(0x7ff8) << 48;
    return *reinterpret_cast<T*>(&x);
  }
  static T inf() {
    uint64_t x = uint64_t(0x7ff0) << 48;
    return *reinterpret_cast<T*>(&x);
  }
};
#endif
template <typename T>
struct FloatTraits<T, 4 /*32bits*/> {
  typedef int32_t mantissa_type;
  static const short mantissa_bits = 23;
  static const mantissa_type mantissa_max =
      (static_cast<mantissa_type>(1) << mantissa_bits) - 1;
  typedef int8_t exponent_type;
  static const exponent_type exponent_max = 38;
  template <typename TExponent>
  static T make_float(T m, TExponent e) {
    if (e > 0)
      return m * (e & 1 ? 1e1f : 1) * (e & 2 ? 1e2f : 1) * (e & 4 ? 1e4f : 1) *
             (e & 8 ? 1e8f : 1) * (e & 16 ? 1e16f : 1) * (e & 32 ? 1e32f : 1);
    e = -e;
    return m * (e & 1 ? 1e-1f : 1) * (e & 2 ? 1e-2f : 1) * (e & 4 ? 1e-4f : 1) *
           (e & 8 ? 1e-8f : 1) * (e & 16 ? 1e-16f : 1) * (e & 32 ? 1e-32f : 1);
  }
  static T nan() {
    uint32_t x = 0x7fc00000;
    return *reinterpret_cast<T*>(&x);
  }
  static T inf() {
    uint32_t x = 0x7f800000;
    return *reinterpret_cast<T*>(&x);
  }
};
}
}
namespace ArduinoJson {
namespace Internals {
template <typename Print>
class JsonWriter {
  static const uint8_t maxDecimalPlaces = sizeof(JsonFloat) >= 8 ? 9 : 6;
  static const uint32_t maxDecimalPart =
      sizeof(JsonFloat) >= 8 ? 1000000000 : 1000000;
 public:
  explicit JsonWriter(Print &sink) : _sink(sink), _length(0) {}
  size_t bytesWritten() const {
    return _length;
  }
  void beginArray() {
    writeRaw('[');
  }
  void endArray() {
    writeRaw(']');
  }
  void beginObject() {
    writeRaw('{');
  }
  void endObject() {
    writeRaw('}');
  }
  void writeColon() {
    writeRaw(':');
  }
  void writeComma() {
    writeRaw(',');
  }
  void writeBoolean(bool value) {
    writeRaw(value ? "true" : "false");
  }
  void writeString(const char *value) {
    if (!value) {
      writeRaw("null");
    } else {
      writeRaw('\"');
      while (*value) writeChar(*value++);
      writeRaw('\"');
    }
  }
  void writeChar(char c) {
    char specialChar = Encoding::escapeChar(c);
    if (specialChar) {
      writeRaw('\\');
      writeRaw(specialChar);
    } else {
      writeRaw(c);
    }
  }
  void writeFloat(JsonFloat value) {
    if (Polyfills::isNaN(value)) return writeRaw("NaN");
    if (value < 0.0) {
      writeRaw('-');
      value = -value;
    }
    if (Polyfills::isInfinity(value)) return writeRaw("Infinity");
    uint32_t integralPart, decimalPart;
    int16_t powersOf10;
    splitFloat(value, integralPart, decimalPart, powersOf10);
    writeInteger(integralPart);
    if (decimalPart) writeDecimals(decimalPart, maxDecimalPlaces);
    if (powersOf10 < 0) {
      writeRaw("e-");
      writeInteger(-powersOf10);
    }
    if (powersOf10 > 0) {
      writeRaw('e');
      writeInteger(powersOf10);
    }
  }
  template <typename UInt>
  void writeInteger(UInt value) {
    char buffer[22];
    char *ptr = buffer + sizeof(buffer) - 1;
    *ptr = 0;
    do {
      *--ptr = char(value % 10 + '0');
      value = UInt(value / 10);
    } while (value);
    writeRaw(ptr);
  }
  void writeDecimals(uint32_t value, int8_t width) {
    while (value % 10 == 0 && width > 0) {
      value /= 10;
      width--;
    }
    char buffer[maxDecimalPlaces + 2];
    char *ptr = buffer + sizeof(buffer) - 1;
    *ptr = 0;
    while (width--) {
      *--ptr = char(value % 10 + '0');
      value /= 10;
    }
    *--ptr = '.';
    writeRaw(ptr);
  }
  void writeRaw(const char *s) {
    _length += _sink.print(s);
  }
  void writeRaw(char c) {
    _length += _sink.print(c);
  }
 protected:
  Print &_sink;
  size_t _length;
 private:
  JsonWriter &operator=(const JsonWriter &);  // cannot be assigned
  void splitFloat(JsonFloat value, uint32_t &integralPart,
                  uint32_t &decimalPart, int16_t &powersOf10) {
    powersOf10 = Polyfills::normalize(value);
    integralPart = uint32_t(value);
    JsonFloat remainder = value - JsonFloat(integralPart);
    remainder *= maxDecimalPart;
    decimalPart = uint32_t(remainder);
    remainder = remainder - JsonFloat(decimalPart);
    decimalPart += uint32_t(remainder * 2);
    if (decimalPart >= maxDecimalPart) {
      decimalPart = 0;
      integralPart++;
      if (powersOf10 && integralPart >= 10) {
        powersOf10++;
        integralPart = 1;
      }
    }
  }
};
}
}
namespace ArduinoJson {
class JsonArray;
class JsonArraySubscript;
class JsonObject;
template <typename TKey>
class JsonObjectSubscript;
class JsonVariant;
namespace Internals {
template <typename Writer>
class JsonSerializer {
 public:
  static void serialize(const JsonArray &, Writer &);
  static void serialize(const JsonArraySubscript &, Writer &);
  static void serialize(const JsonObject &, Writer &);
  template <typename TKey>
  static void serialize(const JsonObjectSubscript<TKey> &, Writer &);
  static void serialize(const JsonVariant &, Writer &);
};
}
}
namespace ArduinoJson {
namespace Internals {
template <typename Print>
class Prettyfier {
 public:
  explicit Prettyfier(IndentedPrint<Print>& p) : _sink(p) {
    _previousChar = 0;
    _inString = false;
  }
  size_t print(char c) {
    size_t n = _inString ? handleStringChar(c) : handleMarkupChar(c);
    _previousChar = c;
    return n;
  }
  size_t print(const char* s) {
    size_t n = 0;
    while (*s) n += print(*s++);
    return n;
  }
 private:
  Prettyfier& operator=(const Prettyfier&);  // cannot be assigned
  bool inEmptyBlock() {
    return _previousChar == '{' || _previousChar == '[';
  }
  size_t handleStringChar(char c) {
    bool isQuote = c == '"' && _previousChar != '\\';
    if (isQuote) _inString = false;
    return _sink.print(c);
  }
  size_t handleMarkupChar(char c) {
    switch (c) {
      case '{':
      case '[':
        return writeBlockOpen(c);
      case '}':
      case ']':
        return writeBlockClose(c);
      case ':':
        return writeColon();
      case ',':
        return writeComma();
      case '"':
        return writeQuoteOpen();
      default:
        return writeNormalChar(c);
    }
  }
  size_t writeBlockClose(char c) {
    size_t n = 0;
    n += unindentIfNeeded();
    n += _sink.print(c);
    return n;
  }
  size_t writeBlockOpen(char c) {
    size_t n = 0;
    n += indentIfNeeded();
    n += _sink.print(c);
    return n;
  }
  size_t writeColon() {
    size_t n = 0;
    n += _sink.print(": ");
    return n;
  }
  size_t writeComma() {
    size_t n = 0;
    n += _sink.print(",\r\n");
    return n;
  }
  size_t writeQuoteOpen() {
    _inString = true;
    size_t n = 0;
    n += indentIfNeeded();
    n += _sink.print('"');
    return n;
  }
  size_t writeNormalChar(char c) {
    size_t n = 0;
    n += indentIfNeeded();
    n += _sink.print(c);
    return n;
  }
  size_t indentIfNeeded() {
    if (!inEmptyBlock()) return 0;
    _sink.indent();
    return _sink.print("\r\n");
  }
  size_t unindentIfNeeded() {
    if (inEmptyBlock()) return 0;
    _sink.unindent();
    return _sink.print("\r\n");
  }
  char _previousChar;
  IndentedPrint<Print>& _sink;
  bool _inString;
};
}
}
namespace ArduinoJson {
namespace Internals {
class StaticStringBuilder {
 public:
  StaticStringBuilder(char *buf, size_t size) : end(buf + size - 1), p(buf) {
    *p = '\0';
  }
  size_t print(char c) {
    if (p >= end) return 0;
    *p++ = c;
    *p = '\0';
    return 1;
  }
  size_t print(const char *s) {
    char *begin = p;
    while (p < end && *s) *p++ = *s++;
    *p = '\0';
    return size_t(p - begin);
  }
 private:
  char *end;
  char *p;
};
}
}
#if ARDUINOJSON_ENABLE_STD_STREAM
#if ARDUINOJSON_ENABLE_STD_STREAM
#include <ostream>
namespace ArduinoJson {
namespace Internals {
class StreamPrintAdapter {
 public:
  explicit StreamPrintAdapter(std::ostream& os) : _os(os) {}
  size_t print(char c) {
    _os << c;
    return 1;
  }
  size_t print(const char* s) {
    _os << s;
    return strlen(s);
  }
 private:
  StreamPrintAdapter& operator=(const StreamPrintAdapter&);
  std::ostream& _os;
};
}
}
#endif  // ARDUINOJSON_ENABLE_STD_STREAM
#endif
namespace ArduinoJson {
namespace Internals {
template <typename T>
class JsonPrintable {
 public:
  template <typename Print>
  typename TypeTraits::EnableIf<!TypeTraits::IsString<Print>::value,
                                size_t>::type
  printTo(Print &print) const {
    JsonWriter<Print> writer(print);
    JsonSerializer<JsonWriter<Print> >::serialize(downcast(), writer);
    return writer.bytesWritten();
  }
#if ARDUINOJSON_ENABLE_STD_STREAM
  std::ostream &printTo(std::ostream &os) const {
    StreamPrintAdapter adapter(os);
    printTo(adapter);
    return os;
  }
#endif
  size_t printTo(char *buffer, size_t bufferSize) const {
    StaticStringBuilder sb(buffer, bufferSize);
    return printTo(sb);
  }
  template <size_t N>
  size_t printTo(char (&buffer)[N]) const {
    return printTo(buffer, N);
  }
  template <typename TString>
  typename TypeTraits::EnableIf<StringTraits<TString>::has_append, size_t>::type
  printTo(TString &str) const {
    DynamicStringBuilder<TString> sb(str);
    return printTo(sb);
  }
  template <typename Print>
  size_t prettyPrintTo(IndentedPrint<Print> &print) const {
    Prettyfier<Print> p(print);
    return printTo(p);
  }
  size_t prettyPrintTo(char *buffer, size_t bufferSize) const {
    StaticStringBuilder sb(buffer, bufferSize);
    return prettyPrintTo(sb);
  }
  template <size_t N>
  size_t prettyPrintTo(char (&buffer)[N]) const {
    return prettyPrintTo(buffer, N);
  }
  template <typename Print>
  typename TypeTraits::EnableIf<!TypeTraits::IsString<Print>::value,
                                size_t>::type
  prettyPrintTo(Print &print) const {
    IndentedPrint<Print> indentedPrint(print);
    return prettyPrintTo(indentedPrint);
  }
  template <typename TString>
  typename TypeTraits::EnableIf<StringTraits<TString>::has_append, size_t>::type
  prettyPrintTo(TString &str) const {
    DynamicStringBuilder<TString> sb(str);
    return prettyPrintTo(sb);
  }
  size_t measureLength() const {
    DummyPrint dp;
    return printTo(dp);
  }
  size_t measurePrettyLength() const {
    DummyPrint dp;
    return prettyPrintTo(dp);
  }
 private:
  const T &downcast() const {
    return *static_cast<const T *>(this);
  }
};
#if ARDUINOJSON_ENABLE_STD_STREAM
template <typename T>
inline std::ostream &operator<<(std::ostream &os, const JsonPrintable<T> &v) {
  return v.printTo(os);
}
#endif
}
}
namespace ArduinoJson {
class JsonArraySubscript;
template <typename TKey>
class JsonObjectSubscript;
template <typename TImpl>
class JsonVariantBase : public Internals::JsonPrintable<TImpl> {
 public:
#if ARDUINOJSON_ENABLE_DEPRECATED
  DEPRECATED("use as<JsonArray>() instead")
  FORCE_INLINE JsonArray &asArray() const {
    return as<JsonArray>();
  }
  DEPRECATED("use as<JsonObject>() instead")
  FORCE_INLINE JsonObject &asObject() const {
    return as<JsonObject>();
  }
  DEPRECATED("use as<char*>() instead")
  FORCE_INLINE const char *asString() const {
    return as<const char *>();
  }
#endif
  FORCE_INLINE operator JsonArray &() const {
    return as<JsonArray &>();
  }
  FORCE_INLINE operator JsonObject &() const {
    return as<JsonObject &>();
  }
  template <typename T>
  FORCE_INLINE operator T() const {
    return as<T>();
  }
  template <typename T>
  FORCE_INLINE const typename Internals::JsonVariantAs<T>::type as() const {
    return impl()->template as<T>();
  }
  template <typename T>
  FORCE_INLINE bool is() const {
    return impl()->template is<T>();
  }
  size_t size() const {
    return as<JsonArray>().size() + as<JsonObject>().size();
  }
  FORCE_INLINE const JsonArraySubscript operator[](size_t index) const;
  FORCE_INLINE JsonArraySubscript operator[](size_t index);
  template <typename TString>
  FORCE_INLINE typename TypeTraits::EnableIf<
      Internals::StringTraits<TString>::has_equals,
      const JsonObjectSubscript<const TString &> >::type
  operator[](const TString &key) const {
    return as<JsonObject>()[key];
  }
  template <typename TString>
  FORCE_INLINE typename TypeTraits::EnableIf<
      Internals::StringTraits<TString>::has_equals,
      JsonObjectSubscript<const TString &> >::type
  operator[](const TString &key) {
    return as<JsonObject>()[key];
  }
  template <typename TString>
  FORCE_INLINE typename TypeTraits::EnableIf<
      Internals::StringTraits<const TString *>::has_equals,
      JsonObjectSubscript<const TString *> >::type
  operator[](const TString *key) {
    return as<JsonObject>()[key];
  }
  template <typename TString>
  FORCE_INLINE typename TypeTraits::EnableIf<
      Internals::StringTraits<TString *>::has_equals,
      const JsonObjectSubscript<const TString *> >::type
  operator[](const TString *key) const {
    return as<JsonObject>()[key];
  }
 private:
  const TImpl *impl() const {
    return static_cast<const TImpl *>(this);
  }
};
namespace TypeTraits {
template <typename T>
struct IsVariant : IsBaseOf<JsonVariantBase<T>, T> {};
}
}
namespace ArduinoJson {
class RawJson {
 public:
  explicit RawJson(const char* str) : _str(str) {}
  operator const char*() const {
    return _str;
  }
 private:
  const char* _str;
};
}
namespace ArduinoJson {
namespace TypeTraits {
template <typename T>
struct IsFloatingPoint {
  static const bool value = IsSame<T, float>::value || IsSame<T, double>::value;
};
}
}
namespace ArduinoJson {
namespace TypeTraits {
template <typename T>
struct IsSignedIntegral {
  static const bool value = TypeTraits::IsSame<T, signed char>::value ||
                            TypeTraits::IsSame<T, signed short>::value ||
                            TypeTraits::IsSame<T, signed int>::value ||
                            TypeTraits::IsSame<T, signed long>::value ||
#if ARDUINOJSON_USE_LONG_LONG
                            TypeTraits::IsSame<T, signed long long>::value ||
#endif
#if ARDUINOJSON_USE_INT64
                            TypeTraits::IsSame<T, signed __int64>::value ||
#endif
                            false;
};
}
}
namespace ArduinoJson {
namespace TypeTraits {
template <typename T>
struct IsUnsignedIntegral {
  static const bool value = TypeTraits::IsSame<T, unsigned char>::value ||
                            TypeTraits::IsSame<T, unsigned short>::value ||
                            TypeTraits::IsSame<T, unsigned int>::value ||
                            TypeTraits::IsSame<T, unsigned long>::value ||
#if ARDUINOJSON_USE_LONG_LONG
                            TypeTraits::IsSame<T, unsigned long long>::value ||
#endif
#if ARDUINOJSON_USE_INT64
                            TypeTraits::IsSame<T, unsigned __int64>::value ||
#endif
                            false;
};
}
}
namespace ArduinoJson {
namespace TypeTraits {
template <typename T>
struct IsIntegral {
  static const bool value = TypeTraits::IsSignedIntegral<T>::value ||
                            TypeTraits::IsUnsignedIntegral<T>::value ||
                            TypeTraits::IsSame<T, char>::value;
};
template <typename T>
struct IsIntegral<const T> : IsIntegral<T> {};
}
}
namespace ArduinoJson {
namespace TypeTraits {
template <typename T>
struct RemoveConst {
  typedef T type;
};
template <typename T>
struct RemoveConst<const T> {
  typedef T type;
};
}
}
namespace ArduinoJson {
class JsonArray;
class JsonObject;
class JsonVariant : public JsonVariantBase<JsonVariant> {
  template <typename Print>
  friend class Internals::JsonSerializer;
 public:
  JsonVariant() : _type(Internals::JSON_UNDEFINED) {}
  JsonVariant(bool value) {
    using namespace Internals;
    _type = JSON_BOOLEAN;
    _content.asInteger = static_cast<JsonUInt>(value);
  }
  template <typename T>
  JsonVariant(T value, typename TypeTraits::EnableIf<
                           TypeTraits::IsFloatingPoint<T>::value>::type * = 0) {
    using namespace Internals;
    _type = JSON_FLOAT;
    _content.asFloat = static_cast<JsonFloat>(value);
  }
  template <typename T>
  DEPRECATED("Second argument is not supported anymore")
  JsonVariant(T value, uint8_t,
              typename TypeTraits::EnableIf<
                  TypeTraits::IsFloatingPoint<T>::value>::type * = 0) {
    using namespace Internals;
    _type = JSON_FLOAT;
    _content.asFloat = static_cast<JsonFloat>(value);
  }
  template <typename T>
  JsonVariant(T value, typename TypeTraits::EnableIf<
                           TypeTraits::IsSignedIntegral<T>::value ||
                           TypeTraits::IsSame<T, char>::value>::type * = 0) {
    using namespace Internals;
    if (value >= 0) {
      _type = JSON_POSITIVE_INTEGER;
      _content.asInteger = static_cast<JsonUInt>(value);
    } else {
      _type = JSON_NEGATIVE_INTEGER;
      _content.asInteger = static_cast<JsonUInt>(-value);
    }
  }
  template <typename T>
  JsonVariant(T value,
              typename TypeTraits::EnableIf<
                  TypeTraits::IsUnsignedIntegral<T>::value>::type * = 0) {
    using namespace Internals;
    _type = JSON_POSITIVE_INTEGER;
    _content.asInteger = static_cast<JsonUInt>(value);
  }
  template <typename TChar>
  JsonVariant(
      const TChar *value,
      typename TypeTraits::EnableIf<TypeTraits::IsChar<TChar>::value>::type * =
          0) {
    _type = Internals::JSON_STRING;
    _content.asString = reinterpret_cast<const char *>(value);
  }
  JsonVariant(RawJson value) {
    _type = Internals::JSON_UNPARSED;
    _content.asString = value;
  }
  JsonVariant(const JsonArray &array);
  JsonVariant(const JsonObject &object);
  template <typename T>
  const typename TypeTraits::EnableIf<TypeTraits::IsIntegral<T>::value, T>::type
  as() const {
    return variantAsInteger<T>();
  }
  template <typename T>
  const typename TypeTraits::EnableIf<TypeTraits::IsSame<T, bool>::value,
                                      T>::type
  as() const {
    return variantAsInteger<int>() != 0;
  }
  template <typename T>
  const typename TypeTraits::EnableIf<TypeTraits::IsFloatingPoint<T>::value,
                                      T>::type
  as() const {
    return variantAsFloat<T>();
  }
  template <typename T>
  typename TypeTraits::EnableIf<TypeTraits::IsSame<T, const char *>::value ||
                                    TypeTraits::IsSame<T, char *>::value,
                                const char *>::type
  as() const {
    return variantAsString();
  }
  template <typename T>
  typename TypeTraits::EnableIf<Internals::StringTraits<T>::has_append, T>::type
  as() const {
    const char *cstr = variantAsString();
    if (cstr) return T(cstr);
    T s;
    printTo(s);
    return s;
  }
  template <typename T>
  typename TypeTraits::EnableIf<
      TypeTraits::IsSame<typename TypeTraits::RemoveReference<T>::type,
                         JsonArray>::value,
      JsonArray &>::type
  as() const {
    return variantAsArray();
  }
  template <typename T>
  typename TypeTraits::EnableIf<
      TypeTraits::IsSame<typename TypeTraits::RemoveReference<T>::type,
                         const JsonArray>::value,
      const JsonArray &>::type
  as() const {
    return variantAsArray();
  }
  template <typename T>
  typename TypeTraits::EnableIf<
      TypeTraits::IsSame<typename TypeTraits::RemoveReference<T>::type,
                         JsonObject>::value,
      JsonObject &>::type
  as() const {
    return variantAsObject();
  }
  template <typename T>
  typename TypeTraits::EnableIf<
      TypeTraits::IsSame<typename TypeTraits::RemoveReference<T>::type,
                         const JsonObject>::value,
      const JsonObject &>::type
  as() const {
    return variantAsObject();
  }
  template <typename T>
  typename TypeTraits::EnableIf<TypeTraits::IsSame<T, JsonVariant>::value,
                                T>::type
  as() const {
    return *this;
  }
  template <typename T>
  typename TypeTraits::EnableIf<TypeTraits::IsIntegral<T>::value, bool>::type
  is() const {
    return variantIsInteger();
  }
  template <typename T>
  typename TypeTraits::EnableIf<TypeTraits::IsFloatingPoint<T>::value,
                                bool>::type
  is() const {
    return variantIsFloat();
  }
  template <typename T>
  typename TypeTraits::EnableIf<TypeTraits::IsSame<T, bool>::value, bool>::type
  is() const {
    return variantIsBoolean();
  }
  template <typename T>
  typename TypeTraits::EnableIf<TypeTraits::IsSame<T, const char *>::value ||
                                    TypeTraits::IsSame<T, char *>::value,
                                bool>::type
  is() const {
    return variantIsString();
  }
  template <typename T>
  typename TypeTraits::EnableIf<
      TypeTraits::IsSame<
          typename TypeTraits::RemoveConst<
              typename TypeTraits::RemoveReference<T>::type>::type,
          JsonArray>::value,
      bool>::type
  is() const {
    return variantIsArray();
  }
  template <typename T>
  typename TypeTraits::EnableIf<
      TypeTraits::IsSame<
          typename TypeTraits::RemoveConst<
              typename TypeTraits::RemoveReference<T>::type>::type,
          JsonObject>::value,
      bool>::type
  is() const {
    return variantIsObject();
  }
  bool success() const {
    return _type != Internals::JSON_UNDEFINED;
  }
 private:
  JsonArray &variantAsArray() const;
  JsonObject &variantAsObject() const;
  const char *variantAsString() const;
  template <typename T>
  T variantAsFloat() const;
  template <typename T>
  T variantAsInteger() const;
  bool variantIsBoolean() const;
  bool variantIsFloat() const;
  bool variantIsInteger() const;
  bool variantIsArray() const {
    return _type == Internals::JSON_ARRAY;
  }
  bool variantIsObject() const {
    return _type == Internals::JSON_OBJECT;
  }
  bool variantIsString() const {
    return _type == Internals::JSON_STRING ||
           (_type == Internals::JSON_UNPARSED && _content.asString &&
            !strcmp("null", _content.asString));
  }
  Internals::JsonVariantType _type;
  Internals::JsonVariantContent _content;
};
DEPRECATED("Decimal places are ignored, use the float value instead")
inline JsonVariant float_with_n_digits(float value, uint8_t) {
  return JsonVariant(value);
}
DEPRECATED("Decimal places are ignored, use the double value instead")
inline JsonVariant double_with_n_digits(double value, uint8_t) {
  return JsonVariant(value);
}
}
namespace ArduinoJson {
namespace TypeTraits {
template <typename T>
struct IsArray {
  static const bool value = false;
};
template <typename T>
struct IsArray<T[]> {
  static const bool value = true;
};
template <typename T, size_t N>
struct IsArray<T[N]> {
  static const bool value = true;
};
}
}
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#elif defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif
namespace ArduinoJson {
class JsonArray;
class JsonObject;
class JsonBuffer {
 public:
  JsonArray &createArray();
  JsonObject &createObject();
  template <typename TString>
  typename TypeTraits::EnableIf<!TypeTraits::IsArray<TString>::value,
                                char *>::type
  strdup(const TString &src) {
    return Internals::StringTraits<TString>::duplicate(src, this);
  }
  template <typename TString>
  char *strdup(const TString *src) {
    return Internals::StringTraits<const TString *>::duplicate(src, this);
  }
  virtual void *alloc(size_t size) = 0;
 protected:
  static FORCE_INLINE size_t round_size_up(size_t bytes) {
#if ARDUINOJSON_ENABLE_ALIGNMENT
    const size_t x = sizeof(void *) - 1;
    return (bytes + x) & ~x;
#else
    return bytes;
#endif
  }
};
}
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
#endif
namespace ArduinoJson {
namespace TypeTraits {
template <typename T>
struct IsConst {
  static const bool value = false;
};
template <typename T>
struct IsConst<const T> {
  static const bool value = true;
};
}
}
namespace ArduinoJson {
namespace Internals {
template <typename TChar>
class StringWriter {
 public:
  class String {
   public:
    String(TChar** ptr) : _writePtr(ptr), _startPtr(*ptr) {}
    void append(char c) {
      *(*_writePtr)++ = TChar(c);
    }
    const char* c_str() const {
      *(*_writePtr)++ = 0;
      return reinterpret_cast<const char*>(_startPtr);
    }
   private:
    TChar** _writePtr;
    TChar* _startPtr;
  };
  StringWriter(TChar* buffer) : _ptr(buffer) {}
  String startString() {
    return String(&_ptr);
  }
 private:
  TChar* _ptr;
};
}
}
namespace ArduinoJson {
namespace Internals {
template <typename TReader, typename TWriter>
class JsonParser {
 public:
  JsonParser(JsonBuffer *buffer, TReader reader, TWriter writer,
             uint8_t nestingLimit)
      : _buffer(buffer),
        _reader(reader),
        _writer(writer),
        _nestingLimit(nestingLimit) {}
  JsonArray &parseArray();
  JsonObject &parseObject();
  JsonVariant parseVariant() {
    JsonVariant result;
    parseAnythingTo(&result);
    return result;
  }
 private:
  JsonParser &operator=(const JsonParser &);  // non-copiable
  static bool eat(TReader &, char charToSkip);
  FORCE_INLINE bool eat(char charToSkip) {
    return eat(_reader, charToSkip);
  }
  const char *parseString();
  bool parseAnythingTo(JsonVariant *destination);
  FORCE_INLINE bool parseAnythingToUnsafe(JsonVariant *destination);
  inline bool parseArrayTo(JsonVariant *destination);
  inline bool parseObjectTo(JsonVariant *destination);
  inline bool parseStringTo(JsonVariant *destination);
  static inline bool isInRange(char c, char min, char max) {
    return min <= c && c <= max;
  }
  static inline bool isLetterOrNumber(char c) {
    return isInRange(c, '0', '9') || isInRange(c, 'a', 'z') ||
           isInRange(c, 'A', 'Z') || c == '+' || c == '-' || c == '.';
  }
  static inline bool isQuote(char c) {
    return c == '\'' || c == '\"';
  }
  JsonBuffer *_buffer;
  TReader _reader;
  TWriter _writer;
  uint8_t _nestingLimit;
};
template <typename TJsonBuffer, typename TString, typename Enable = void>
struct JsonParserBuilder {
  typedef typename Internals::StringTraits<TString>::Reader InputReader;
  typedef JsonParser<InputReader, TJsonBuffer &> TParser;
  static TParser makeParser(TJsonBuffer *buffer, TString &json,
                            uint8_t nestingLimit) {
    return TParser(buffer, InputReader(json), *buffer, nestingLimit);
  }
};
template <typename TJsonBuffer, typename TChar>
struct JsonParserBuilder<
    TJsonBuffer, TChar *,
    typename TypeTraits::EnableIf<!TypeTraits::IsConst<TChar>::value>::type> {
  typedef typename Internals::StringTraits<TChar *>::Reader TReader;
  typedef StringWriter<TChar> TWriter;
  typedef JsonParser<TReader, TWriter> TParser;
  static TParser makeParser(TJsonBuffer *buffer, TChar *json,
                            uint8_t nestingLimit) {
    return TParser(buffer, TReader(json), TWriter(json), nestingLimit);
  }
};
template <typename TJsonBuffer, typename TString>
inline typename JsonParserBuilder<TJsonBuffer, TString>::TParser makeParser(
    TJsonBuffer *buffer, TString &json, uint8_t nestingLimit) {
  return JsonParserBuilder<TJsonBuffer, TString>::makeParser(buffer, json,
                                                             nestingLimit);
}
}
}
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#elif defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif
namespace ArduinoJson {
template <typename TDerived>
class JsonBufferBase : public JsonBuffer {
 public:
  template <typename TString>
  typename TypeTraits::EnableIf<!TypeTraits::IsArray<TString>::value,
                                JsonArray &>::type
  parseArray(const TString &json,
             uint8_t nestingLimit = ARDUINOJSON_DEFAULT_NESTING_LIMIT) {
    return Internals::makeParser(that(), json, nestingLimit).parseArray();
  }
  template <typename TString>
  JsonArray &parseArray(
      TString *json, uint8_t nestingLimit = ARDUINOJSON_DEFAULT_NESTING_LIMIT) {
    return Internals::makeParser(that(), json, nestingLimit).parseArray();
  }
  template <typename TString>
  JsonArray &parseArray(
      TString &json, uint8_t nestingLimit = ARDUINOJSON_DEFAULT_NESTING_LIMIT) {
    return Internals::makeParser(that(), json, nestingLimit).parseArray();
  }
  template <typename TString>
  typename TypeTraits::EnableIf<!TypeTraits::IsArray<TString>::value,
                                JsonObject &>::type
  parseObject(const TString &json,
              uint8_t nestingLimit = ARDUINOJSON_DEFAULT_NESTING_LIMIT) {
    return Internals::makeParser(that(), json, nestingLimit).parseObject();
  }
  template <typename TString>
  JsonObject &parseObject(
      TString *json, uint8_t nestingLimit = ARDUINOJSON_DEFAULT_NESTING_LIMIT) {
    return Internals::makeParser(that(), json, nestingLimit).parseObject();
  }
  template <typename TString>
  JsonObject &parseObject(
      TString &json, uint8_t nestingLimit = ARDUINOJSON_DEFAULT_NESTING_LIMIT) {
    return Internals::makeParser(that(), json, nestingLimit).parseObject();
  }
  template <typename TString>
  typename TypeTraits::EnableIf<!TypeTraits::IsArray<TString>::value,
                                JsonVariant>::type
  parse(const TString &json,
        uint8_t nestingLimit = ARDUINOJSON_DEFAULT_NESTING_LIMIT) {
    return Internals::makeParser(that(), json, nestingLimit).parseVariant();
  }
  template <typename TString>
  JsonVariant parse(TString *json,
                    uint8_t nestingLimit = ARDUINOJSON_DEFAULT_NESTING_LIMIT) {
    return Internals::makeParser(that(), json, nestingLimit).parseVariant();
  }
  template <typename TString>
  JsonVariant parse(TString &json,
                    uint8_t nestingLimit = ARDUINOJSON_DEFAULT_NESTING_LIMIT) {
    return Internals::makeParser(that(), json, nestingLimit).parseVariant();
  }
 private:
  TDerived *that() {
    return static_cast<TDerived *>(this);
  }
};
}
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
#endif
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#elif defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif
namespace ArduinoJson {
class DefaultAllocator {
 public:
  void* allocate(size_t size) {
    return malloc(size);
  }
  void deallocate(void* pointer) {
    free(pointer);
  }
};
template <typename TAllocator>
class DynamicJsonBufferBase
    : public JsonBufferBase<DynamicJsonBufferBase<TAllocator> > {
  struct Block;
  struct EmptyBlock {
    Block* next;
    size_t capacity;
    size_t size;
  };
  struct Block : EmptyBlock {
    uint8_t data[1];
  };
 public:
  DynamicJsonBufferBase(size_t initialSize = 256)
      : _head(NULL), _nextBlockCapacity(initialSize) {}
  ~DynamicJsonBufferBase() {
    Block* currentBlock = _head;
    while (currentBlock != NULL) {
      Block* nextBlock = currentBlock->next;
      _allocator.deallocate(currentBlock);
      currentBlock = nextBlock;
    }
  }
  size_t size() const {
    size_t total = 0;
    for (const Block* b = _head; b; b = b->next) total += b->size;
    return total;
  }
  virtual void* alloc(size_t bytes) {
    alignNextAlloc();
    return canAllocInHead(bytes) ? allocInHead(bytes) : allocInNewBlock(bytes);
  }
  class String {
   public:
    String(DynamicJsonBufferBase* parent)
        : _parent(parent), _start(NULL), _length(0) {}
    void append(char c) {
      if (_parent->canAllocInHead(1)) {
        char* end = static_cast<char*>(_parent->allocInHead(1));
        *end = c;
        if (_length == 0) _start = end;
      } else {
        char* newStart =
            static_cast<char*>(_parent->allocInNewBlock(_length + 1));
        if (_start && newStart) memcpy(newStart, _start, _length);
        if (newStart) newStart[_length] = c;
        _start = newStart;
      }
      _length++;
    }
    const char* c_str() {
      append(0);
      return _start;
    }
   private:
    DynamicJsonBufferBase* _parent;
    char* _start;
    size_t _length;
  };
  String startString() {
    return String(this);
  }
 private:
  void alignNextAlloc() {
    if (_head) _head->size = this->round_size_up(_head->size);
  }
  bool canAllocInHead(size_t bytes) const {
    return _head != NULL && _head->size + bytes <= _head->capacity;
  }
  void* allocInHead(size_t bytes) {
    void* p = _head->data + _head->size;
    _head->size += bytes;
    return p;
  }
  void* allocInNewBlock(size_t bytes) {
    size_t capacity = _nextBlockCapacity;
    if (bytes > capacity) capacity = bytes;
    if (!addNewBlock(capacity)) return NULL;
    _nextBlockCapacity *= 2;
    return allocInHead(bytes);
  }
  bool addNewBlock(size_t capacity) {
    size_t bytes = sizeof(EmptyBlock) + capacity;
    Block* block = static_cast<Block*>(_allocator.allocate(bytes));
    if (block == NULL) return false;
    block->capacity = capacity;
    block->size = 0;
    block->next = _head;
    _head = block;
    return true;
  }
  TAllocator _allocator;
  Block* _head;
  size_t _nextBlockCapacity;
};
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
#endif
typedef DynamicJsonBufferBase<DefaultAllocator> DynamicJsonBuffer;
}
namespace ArduinoJson {
namespace Internals {
class JsonBufferAllocated {
 public:
  void *operator new(size_t n, JsonBuffer *jsonBuffer) throw() {
    if (!jsonBuffer) return NULL;
    return jsonBuffer->alloc(n);
  }
  void operator delete(void *, JsonBuffer *)throw() {}
};
}
}
namespace ArduinoJson {
namespace Internals {
template <typename T>
struct ListNode : public Internals::JsonBufferAllocated {
  ListNode() : next(NULL) {}
  ListNode<T> *next;
  T content;
};
}
}
namespace ArduinoJson {
namespace Internals {
template <typename T>
class ListConstIterator {
 public:
  explicit ListConstIterator(const ListNode<T> *node = NULL) : _node(node) {}
  const T &operator*() const {
    return _node->content;
  }
  const T *operator->() {
    return &_node->content;
  }
  bool operator==(const ListConstIterator<T> &other) const {
    return _node == other._node;
  }
  bool operator!=(const ListConstIterator<T> &other) const {
    return _node != other._node;
  }
  ListConstIterator<T> &operator++() {
    if (_node) _node = _node->next;
    return *this;
  }
  ListConstIterator<T> &operator+=(size_t distance) {
    while (_node && distance) {
      _node = _node->next;
      --distance;
    }
    return *this;
  }
 private:
  const ListNode<T> *_node;
};
}
}
namespace ArduinoJson {
namespace Internals {
template <typename T>
class List;
template <typename T>
class ListIterator {
  friend class List<T>;
 public:
  explicit ListIterator(ListNode<T> *node = NULL) : _node(node) {}
  T &operator*() const {
    return _node->content;
  }
  T *operator->() {
    return &_node->content;
  }
  bool operator==(const ListIterator<T> &other) const {
    return _node == other._node;
  }
  bool operator!=(const ListIterator<T> &other) const {
    return _node != other._node;
  }
  ListIterator<T> &operator++() {
    if (_node) _node = _node->next;
    return *this;
  }
  ListIterator<T> &operator+=(size_t distance) {
    while (_node && distance) {
      _node = _node->next;
      --distance;
    }
    return *this;
  }
  operator ListConstIterator<T>() const {
    return ListConstIterator<T>(_node);
  }
 private:
  ListNode<T> *_node;
};
}
}
namespace ArduinoJson {
namespace Internals {
template <typename T>
class List {
 public:
  typedef T value_type;
  typedef ListNode<T> node_type;
  typedef ListIterator<T> iterator;
  typedef ListConstIterator<T> const_iterator;
  explicit List(JsonBuffer *buffer) : _buffer(buffer), _firstNode(NULL) {}
  bool success() const {
    return _buffer != NULL;
  }
  size_t size() const {
    size_t nodeCount = 0;
    for (node_type *node = _firstNode; node; node = node->next) nodeCount++;
    return nodeCount;
  }
  iterator add() {
    node_type *newNode = new (_buffer) node_type();
    if (_firstNode) {
      node_type *lastNode = _firstNode;
      while (lastNode->next) lastNode = lastNode->next;
      lastNode->next = newNode;
    } else {
      _firstNode = newNode;
    }
    return iterator(newNode);
  }
  iterator begin() {
    return iterator(_firstNode);
  }
  iterator end() {
    return iterator(NULL);
  }
  const_iterator begin() const {
    return const_iterator(_firstNode);
  }
  const_iterator end() const {
    return const_iterator(NULL);
  }
  void remove(iterator it) {
    node_type *nodeToRemove = it._node;
    if (!nodeToRemove) return;
    if (nodeToRemove == _firstNode) {
      _firstNode = nodeToRemove->next;
    } else {
      for (node_type *node = _firstNode; node; node = node->next)
        if (node->next == nodeToRemove) node->next = nodeToRemove->next;
    }
  }
 protected:
  JsonBuffer *_buffer;
 private:
  node_type *_firstNode;
};
}
}
namespace ArduinoJson {
namespace Internals {
class ReferenceType {
 public:
  bool operator==(const ReferenceType& other) const {
    return this == &other;
  }
  bool operator!=(const ReferenceType& other) const {
    return this != &other;
  }
 protected:
  ReferenceType() {}
 private:
  ReferenceType(const ReferenceType&);
  ReferenceType& operator=(const ReferenceType&);
};
}
}
namespace ArduinoJson {
namespace Internals {
template <typename TSourceRef, typename Enable = void>
struct ValueSetter {
  template <typename TDestination>
  static bool set(JsonBuffer*, TDestination& destination, TSourceRef source) {
    destination = source;
    return true;
  }
};
template <typename TSourceRef>
struct ValueSetter<TSourceRef, typename TypeTraits::EnableIf<StringTraits<
                                   TSourceRef>::should_duplicate>::type> {
  template <typename TDestination>
  static bool set(JsonBuffer* buffer, TDestination& destination,
                  TSourceRef source) {
    const char* copy = buffer->strdup(source);
    if (!copy) return false;
    destination = copy;
    return true;
  }
};
template <typename TSourceRef>
struct ValueSetter<TSourceRef, typename TypeTraits::EnableIf<!StringTraits<
                                   TSourceRef>::should_duplicate>::type> {
  template <typename TDestination>
  static bool set(JsonBuffer*, TDestination& destination, TSourceRef source) {
    destination = reinterpret_cast<const char*>(source);
    return true;
  }
};
}
}
#define JSON_ARRAY_SIZE(NUMBER_OF_ELEMENTS) \
  (sizeof(JsonArray) + (NUMBER_OF_ELEMENTS) * sizeof(JsonArray::node_type))
namespace ArduinoJson {
class JsonObject;
class JsonBuffer;
class JsonArraySubscript;
class JsonArray : public Internals::JsonPrintable<JsonArray>,
                  public Internals::ReferenceType,
                  public Internals::List<JsonVariant>,
                  public Internals::JsonBufferAllocated {
 public:
  explicit JsonArray(JsonBuffer *buffer)
      : Internals::List<JsonVariant>(buffer) {}
  const JsonArraySubscript operator[](size_t index) const;
  JsonArraySubscript operator[](size_t index);
  template <typename T>
  typename TypeTraits::EnableIf<!TypeTraits::IsArray<T>::value, bool>::type add(
      const T &value) {
    return add_impl<const T &>(value);
  }
  template <typename T>
  bool add(const T *value) {
    return add_impl<const T *>(value);
  }
  template <typename T>
  DEPRECATED("Second argument is not supported anymore")
  bool add(T value, uint8_t) {
    return add_impl<const JsonVariant &>(JsonVariant(value));
  }
  template <typename T>
  typename TypeTraits::EnableIf<!TypeTraits::IsArray<T>::value, bool>::type set(
      size_t index, const T &value) {
    return set_impl<const T &>(index, value);
  }
  template <typename T>
  bool set(size_t index, const T *value) {
    return set_impl<const T *>(index, value);
  }
  template <typename T>
  typename TypeTraits::EnableIf<TypeTraits::IsFloatingPoint<T>::value,
                                bool>::type
  set(size_t index, T value, uint8_t decimals) {
    return set_impl<const JsonVariant &>(index, JsonVariant(value, decimals));
  }
  template <typename T>
  typename Internals::JsonVariantAs<T>::type get(size_t index) const {
    const_iterator it = begin() += index;
    return it != end() ? it->as<T>() : Internals::JsonVariantDefault<T>::get();
  }
  template <typename T>
  bool is(size_t index) const {
    const_iterator it = begin() += index;
    return it != end() ? it->is<T>() : false;
  }
  JsonArray &createNestedArray();
  JsonObject &createNestedObject();
  void remove(size_t index) {
    remove(begin() += index);
  }
  using Internals::List<JsonVariant>::remove;
  static JsonArray &invalid() {
    static JsonArray instance(NULL);
    return instance;
  }
  template <typename T, size_t N>
  bool copyFrom(T (&array)[N]) {
    return copyFrom(array, N);
  }
  template <typename T>
  bool copyFrom(T *array, size_t len) {
    bool ok = true;
    for (size_t i = 0; i < len; i++) {
      ok &= add(array[i]);
    }
    return ok;
  }
  template <typename T, size_t N1, size_t N2>
  bool copyFrom(T (&array)[N1][N2]) {
    bool ok = true;
    for (size_t i = 0; i < N1; i++) {
      JsonArray &nestedArray = createNestedArray();
      for (size_t j = 0; j < N2; j++) {
        ok &= nestedArray.add(array[i][j]);
      }
    }
    return ok;
  }
  template <typename T, size_t N>
  size_t copyTo(T (&array)[N]) const {
    return copyTo(array, N);
  }
  template <typename T>
  size_t copyTo(T *array, size_t len) const {
    size_t i = 0;
    for (const_iterator it = begin(); it != end() && i < len; ++it)
      array[i++] = *it;
    return i;
  }
  template <typename T, size_t N1, size_t N2>
  void copyTo(T (&array)[N1][N2]) const {
    size_t i = 0;
    for (const_iterator it = begin(); it != end() && i < N1; ++it) {
      it->as<JsonArray>().copyTo(array[i++]);
    }
  }
#if ARDUINOJSON_ENABLE_DEPRECATED
  DEPRECATED("use remove() instead")
  FORCE_INLINE void removeAt(size_t index) {
    return remove(index);
  }
#endif
 private:
  template <typename TValueRef>
  bool set_impl(size_t index, TValueRef value) {
    iterator it = begin() += index;
    if (it == end()) return false;
    return Internals::ValueSetter<TValueRef>::set(_buffer, *it, value);
  }
  template <typename TValueRef>
  bool add_impl(TValueRef value) {
    iterator it = Internals::List<JsonVariant>::add();
    if (it == end()) return false;
    return Internals::ValueSetter<TValueRef>::set(_buffer, *it, value);
  }
};
namespace Internals {
template <>
struct JsonVariantDefault<JsonArray> {
  static JsonArray &get() {
    return JsonArray::invalid();
  }
};
}
}
namespace ArduinoJson {
struct JsonPair {
  const char* key;
  JsonVariant value;
};
}
#define JSON_OBJECT_SIZE(NUMBER_OF_ELEMENTS) \
  (sizeof(JsonObject) + (NUMBER_OF_ELEMENTS) * sizeof(JsonObject::node_type))
namespace ArduinoJson {
class JsonArray;
class JsonBuffer;
class JsonObject : public Internals::JsonPrintable<JsonObject>,
                   public Internals::ReferenceType,
                   public Internals::List<JsonPair>,
                   public Internals::JsonBufferAllocated {
 public:
  explicit JsonObject(JsonBuffer* buffer) : Internals::List<JsonPair>(buffer) {}
  template <typename TString>
  typename TypeTraits::EnableIf<!TypeTraits::IsArray<TString>::value,
                                JsonObjectSubscript<const TString&> >::type
  operator[](const TString& key) {
    return JsonObjectSubscript<const TString&>(*this, key);
  }
  template <typename TString>
  JsonObjectSubscript<const TString*> operator[](const TString* key) {
    return JsonObjectSubscript<const TString*>(*this, key);
  }
  template <typename TString>
  typename TypeTraits::EnableIf<
      !TypeTraits::IsArray<TString>::value,
      const JsonObjectSubscript<const TString&> >::type
  operator[](const TString& key) const {
    return JsonObjectSubscript<const TString&>(*const_cast<JsonObject*>(this),
                                               key);
  }
  template <typename TString>
  const JsonObjectSubscript<const TString*> operator[](
      const TString* key) const {
    return JsonObjectSubscript<const TString*>(*const_cast<JsonObject*>(this),
                                               key);
  }
  template <typename TValue, typename TString>
  typename TypeTraits::EnableIf<!TypeTraits::IsArray<TString>::value &&
                                    !TypeTraits::IsArray<TValue>::value,
                                bool>::type
  set(const TString& key, const TValue& value) {
    return set_impl<const TString&, const TValue&>(key, value);
  }
  template <typename TValue, typename TString>
  typename TypeTraits::EnableIf<!TypeTraits::IsArray<TString>::value,
                                bool>::type
  set(const TString& key, const TValue* value) {
    return set_impl<const TString&, const TValue*>(key, value);
  }
  template <typename TValue, typename TString>
  typename TypeTraits::EnableIf<!TypeTraits::IsArray<TValue>::value, bool>::type
  set(const TString* key, const TValue& value) {
    return set_impl<const TString*, const TValue&>(key, value);
  }
  template <typename TValue, typename TString>
  bool set(const TString* key, const TValue* value) {
    return set_impl<const TString*, const TValue*>(key, value);
  }
  template <typename TValue, typename TString>
  DEPRECATED("Second argument is not supported anymore")
  typename TypeTraits::EnableIf<TypeTraits::IsFloatingPoint<TValue>::value &&
                                    !TypeTraits::IsArray<TString>::value,
                                bool>::type
      set(const TString& key, TValue value, uint8_t) {
    return set_impl<const TString&, const JsonVariant&>(key,
                                                        JsonVariant(value));
  }
  template <typename TValue, typename TString>
  DEPRECATED("Second argument is not supported anymore")
  typename TypeTraits::EnableIf<TypeTraits::IsFloatingPoint<TValue>::value,
                                bool>::type
      set(const TString* key, TValue value, uint8_t) {
    return set_impl<const TString*, const JsonVariant&>(key,
                                                        JsonVariant(value));
  }
  template <typename TValue, typename TString>
  typename TypeTraits::EnableIf<
      !TypeTraits::IsArray<TString>::value,
      typename Internals::JsonVariantAs<TValue>::type>::type
  get(const TString& key) const {
    return get_impl<const TString&, TValue>(key);
  }
  template <typename TValue, typename TString>
  typename Internals::JsonVariantAs<TValue>::type get(
      const TString* key) const {
    return get_impl<const TString*, TValue>(key);
  }
  template <typename TValue, typename TString>
  typename TypeTraits::EnableIf<!TypeTraits::IsArray<TString>::value,
                                bool>::type
  is(const TString& key) const {
    return is_impl<const TString&, TValue>(key);
  }
  template <typename TValue, typename TString>
  bool is(const TString* key) const {
    return is_impl<const TString*, TValue>(key);
  }
  template <typename TString>
  typename TypeTraits::EnableIf<!TypeTraits::IsArray<TString>::value,
                                JsonArray&>::type
  createNestedArray(const TString& key) {
    return createNestedArray_impl<const TString&>(key);
  }
  template <typename TString>
  JsonArray& createNestedArray(const TString* key) {
    return createNestedArray_impl<const TString*>(key);
  }
  template <typename TString>
  typename TypeTraits::EnableIf<!TypeTraits::IsArray<TString>::value,
                                JsonObject&>::type
  createNestedObject(const TString& key) {
    return createNestedObject_impl<const TString&>(key);
  }
  template <typename TString>
  JsonObject& createNestedObject(const TString* key) {
    return createNestedObject_impl<const TString*>(key);
  }
  template <typename TString>
  typename TypeTraits::EnableIf<!TypeTraits::IsArray<TString>::value,
                                bool>::type
  containsKey(const TString& key) const {
    return findKey<const TString&>(key) != end();
  }
  template <typename TString>
  bool containsKey(const TString* key) const {
    return findKey<const TString*>(key) != end();
  }
  template <typename TString>
  typename TypeTraits::EnableIf<!TypeTraits::IsArray<TString>::value,
                                void>::type
  remove(const TString& key) {
    remove(findKey<const TString&>(key));
  }
  template <typename TString>
  void remove(const TString* key) {
    remove(findKey<const TString*>(key));
  }
  using Internals::List<JsonPair>::remove;
  static JsonObject& invalid() {
    static JsonObject instance(NULL);
    return instance;
  }
 private:
  template <typename TStringRef>
  iterator findKey(TStringRef key) {
    iterator it;
    for (it = begin(); it != end(); ++it) {
      if (Internals::StringTraits<TStringRef>::equals(key, it->key)) break;
    }
    return it;
  }
  template <typename TStringRef>
  const_iterator findKey(TStringRef key) const {
    return const_cast<JsonObject*>(this)->findKey<TStringRef>(key);
  }
  template <typename TStringRef, typename TValue>
  typename Internals::JsonVariantAs<TValue>::type get_impl(
      TStringRef key) const {
    const_iterator it = findKey<TStringRef>(key);
    return it != end() ? it->value.as<TValue>()
                       : Internals::JsonVariantDefault<TValue>::get();
  }
  template <typename TStringRef, typename TValueRef>
  bool set_impl(TStringRef key, TValueRef value) {
    iterator it = findKey<TStringRef>(key);
    if (it == end()) {
      it = Internals::List<JsonPair>::add();
      if (it == end()) return false;
      bool key_ok =
          Internals::ValueSetter<TStringRef>::set(_buffer, it->key, key);
      if (!key_ok) return false;
    }
    return Internals::ValueSetter<TValueRef>::set(_buffer, it->value, value);
  }
  template <typename TStringRef, typename TValue>
  bool is_impl(TStringRef key) const {
    const_iterator it = findKey<TStringRef>(key);
    return it != end() ? it->value.is<TValue>() : false;
  }
  template <typename TStringRef>
  JsonArray& createNestedArray_impl(TStringRef key);
  template <typename TStringRef>
  JsonObject& createNestedObject_impl(TStringRef key);
};
namespace Internals {
template <>
struct JsonVariantDefault<JsonObject> {
  static JsonObject& get() {
    return JsonObject::invalid();
  }
};
}
}
namespace ArduinoJson {
namespace Internals {
template <typename TComparand, typename Enable = void>
struct JsonVariantComparer {};
template <typename TString>
struct JsonVariantComparer<
    TString,
    typename TypeTraits::EnableIf<TypeTraits::IsString<TString>::value>::type> {
  template <typename TVariant>
  static bool equals(const JsonVariantBase<TVariant> &variant,
                     const TString &comparand) {
    const char *value = variant.template as<const char *>();
    return Internals::StringTraits<TString>::equals(comparand, value);
  }
};
template <typename TComparand>
struct JsonVariantComparer<
    TComparand, typename TypeTraits::EnableIf<
                    !TypeTraits::IsVariant<TComparand>::value &&
                    !TypeTraits::IsString<TComparand>::value>::type> {
  template <typename TVariant>
  static bool equals(const JsonVariantBase<TVariant> &variant,
                     const TComparand &comparand) {
    return variant.template as<TComparand>() == comparand;
  }
};
template <typename TVariant2>
struct JsonVariantComparer<TVariant2,
                           typename TypeTraits::EnableIf<
                               TypeTraits::IsVariant<TVariant2>::value>::type> {
  template <typename TVariant1>
  static bool equals(const JsonVariantBase<TVariant1> &left,
                     const TVariant2 &right) {
    if (left.template is<bool>() && right.template is<bool>())
      return left.template as<bool>() == right.template as<bool>();
    if (left.template is<JsonInteger>() && right.template is<JsonInteger>())
      return left.template as<JsonInteger>() ==
             right.template as<JsonInteger>();
    if (left.template is<JsonFloat>() && right.template is<JsonFloat>())
      return left.template as<JsonFloat>() == right.template as<JsonFloat>();
    if (left.template is<JsonArray>() && right.template is<JsonArray>())
      return left.template as<JsonArray>() == right.template as<JsonArray>();
    if (left.template is<JsonObject>() && right.template is<JsonObject>())
      return left.template as<JsonObject>() == right.template as<JsonObject>();
    if (left.template is<char *>() && right.template is<char *>())
      return strcmp(left.template as<char *>(), right.template as<char *>()) ==
             0;
    return false;
  }
};
}
}
namespace ArduinoJson {
template <typename TVariant, typename TComparand>
inline bool operator==(const JsonVariantBase<TVariant> &variant,
                       TComparand comparand) {
  return Internals::JsonVariantComparer<TComparand>::equals(variant, comparand);
}
template <typename TVariant, typename TComparand>
inline typename TypeTraits::EnableIf<!TypeTraits::IsVariant<TComparand>::value,
                                     bool>::type
operator==(TComparand comparand, const JsonVariantBase<TVariant> &variant) {
  return Internals::JsonVariantComparer<TComparand>::equals(variant, comparand);
}
template <typename TVariant, typename TComparand>
inline bool operator!=(const JsonVariantBase<TVariant> &variant,
                       TComparand comparand) {
  return !Internals::JsonVariantComparer<TComparand>::equals(variant,
                                                             comparand);
}
template <typename TVariant, typename TComparand>
inline typename TypeTraits::EnableIf<!TypeTraits::IsVariant<TComparand>::value,
                                     bool>::type
operator!=(TComparand comparand, const JsonVariantBase<TVariant> &variant) {
  return !Internals::JsonVariantComparer<TComparand>::equals(variant,
                                                             comparand);
}
template <typename TVariant, typename TComparand>
inline bool operator<=(const JsonVariantBase<TVariant> &left,
                       TComparand right) {
  return left.template as<TComparand>() <= right;
}
template <typename TVariant, typename TComparand>
inline bool operator<=(TComparand comparand,
                       const JsonVariantBase<TVariant> &variant) {
  return comparand <= variant.template as<TComparand>();
}
template <typename TVariant, typename TComparand>
inline bool operator>=(const JsonVariantBase<TVariant> &variant,
                       TComparand comparand) {
  return variant.template as<TComparand>() >= comparand;
}
template <typename TVariant, typename TComparand>
inline bool operator>=(TComparand comparand,
                       const JsonVariantBase<TVariant> &variant) {
  return comparand >= variant.template as<TComparand>();
}
template <typename TVariant, typename TComparand>
inline bool operator<(const JsonVariantBase<TVariant> &varian,
                      TComparand comparand) {
  return varian.template as<TComparand>() < comparand;
}
template <typename TVariant, typename TComparand>
inline bool operator<(TComparand comparand,
                      const JsonVariantBase<TVariant> &variant) {
  return comparand < variant.template as<TComparand>();
}
template <typename TVariant, typename TComparand>
inline bool operator>(const JsonVariantBase<TVariant> &variant,
                      TComparand comparand) {
  return variant.template as<TComparand>() > comparand;
}
template <typename TVariant, typename TComparand>
inline bool operator>(TComparand comparand,
                      const JsonVariantBase<TVariant> &variant) {
  return comparand > variant.template as<TComparand>();
}
}
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#elif defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif
namespace ArduinoJson {
class StaticJsonBufferBase : public JsonBufferBase<StaticJsonBufferBase> {
 public:
  class String {
   public:
    String(StaticJsonBufferBase* parent) : _parent(parent) {
      _start = parent->_buffer + parent->_size;
    }
    void append(char c) {
      if (_parent->canAlloc(1)) {
        char* last = static_cast<char*>(_parent->doAlloc(1));
        *last = c;
      }
    }
    const char* c_str() const {
      if (_parent->canAlloc(1)) {
        char* last = static_cast<char*>(_parent->doAlloc(1));
        *last = '\0';
        return _start;
      } else {
        return NULL;
      }
    }
   private:
    StaticJsonBufferBase* _parent;
    char* _start;
  };
  StaticJsonBufferBase(char* buffer, size_t capa)
      : _buffer(buffer), _capacity(capa), _size(0) {}
  size_t capacity() const {
    return _capacity;
  }
  size_t size() const {
    return _size;
  }
  virtual void* alloc(size_t bytes) {
    alignNextAlloc();
    if (!canAlloc(bytes)) return NULL;
    return doAlloc(bytes);
  }
  String startString() {
    return String(this);
  }
 private:
  void alignNextAlloc() {
    _size = round_size_up(_size);
  }
  bool canAlloc(size_t bytes) const {
    return _size + bytes <= _capacity;
  }
  void* doAlloc(size_t bytes) {
    void* p = &_buffer[_size];
    _size += bytes;
    return p;
  }
  char* _buffer;
  size_t _capacity;
  size_t _size;
};
template <size_t CAPACITY>
class StaticJsonBuffer : public StaticJsonBufferBase {
 public:
  explicit StaticJsonBuffer() : StaticJsonBufferBase(_buffer, CAPACITY) {}
 private:
  char _buffer[CAPACITY];
};
}
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
#endif
namespace ArduinoJson {
namespace Internals {
template <typename TInput>
void skipSpacesAndComments(TInput& input) {
  for (;;) {
    switch (input.current()) {
      case ' ':
      case '\t':
      case '\r':
      case '\n':
        input.move();
        continue;
      case '/':
        switch (input.next()) {
          case '*':
            input.move();  // skip '/'
            for (;;) {
              input.move();
              if (input.current() == '\0') return;
              if (input.current() == '*' && input.next() == '/') {
                input.move();  // skip '*'
                input.move();  // skip '/'
                break;
              }
            }
            break;
          case '/':
            for (;;) {
              input.move();
              if (input.current() == '\0') return;
              if (input.current() == '\n') break;
            }
            break;
          default:
            return;
        }
        break;
      default:
        return;
    }
  }
}
}
}
template <typename TReader, typename TWriter>
inline bool ArduinoJson::Internals::JsonParser<TReader, TWriter>::eat(
    TReader &reader, char charToSkip) {
  skipSpacesAndComments(reader);
  if (reader.current() != charToSkip) return false;
  reader.move();
  return true;
}
template <typename TReader, typename TWriter>
inline bool ArduinoJson::Internals::JsonParser<
    TReader, TWriter>::parseAnythingTo(JsonVariant *destination) {
  if (_nestingLimit == 0) return false;
  _nestingLimit--;
  bool success = parseAnythingToUnsafe(destination);
  _nestingLimit++;
  return success;
}
template <typename TReader, typename TWriter>
inline bool ArduinoJson::Internals::JsonParser<
    TReader, TWriter>::parseAnythingToUnsafe(JsonVariant *destination) {
  skipSpacesAndComments(_reader);
  switch (_reader.current()) {
    case '[':
      return parseArrayTo(destination);
    case '{':
      return parseObjectTo(destination);
    default:
      return parseStringTo(destination);
  }
}
template <typename TReader, typename TWriter>
inline ArduinoJson::JsonArray &
ArduinoJson::Internals::JsonParser<TReader, TWriter>::parseArray() {
  JsonArray &array = _buffer->createArray();
  if (!eat('[')) goto ERROR_MISSING_BRACKET;
  if (eat(']')) goto SUCCESS_EMPTY_ARRAY;
  for (;;) {
    JsonVariant value;
    if (!parseAnythingTo(&value)) goto ERROR_INVALID_VALUE;
    if (!array.add(value)) goto ERROR_NO_MEMORY;
    if (eat(']')) goto SUCCES_NON_EMPTY_ARRAY;
    if (!eat(',')) goto ERROR_MISSING_COMMA;
  }
SUCCESS_EMPTY_ARRAY:
SUCCES_NON_EMPTY_ARRAY:
  return array;
ERROR_INVALID_VALUE:
ERROR_MISSING_BRACKET:
ERROR_MISSING_COMMA:
ERROR_NO_MEMORY:
  return JsonArray::invalid();
}
template <typename TReader, typename TWriter>
inline bool ArduinoJson::Internals::JsonParser<TReader, TWriter>::parseArrayTo(
    JsonVariant *destination) {
  JsonArray &array = parseArray();
  if (!array.success()) return false;
  *destination = array;
  return true;
}
template <typename TReader, typename TWriter>
inline ArduinoJson::JsonObject &
ArduinoJson::Internals::JsonParser<TReader, TWriter>::parseObject() {
  JsonObject &object = _buffer->createObject();
  if (!eat('{')) goto ERROR_MISSING_BRACE;
  if (eat('}')) goto SUCCESS_EMPTY_OBJECT;
  for (;;) {
    const char *key = parseString();
    if (!key) goto ERROR_INVALID_KEY;
    if (!eat(':')) goto ERROR_MISSING_COLON;
    JsonVariant value;
    if (!parseAnythingTo(&value)) goto ERROR_INVALID_VALUE;
    if (!object.set(key, value)) goto ERROR_NO_MEMORY;
    if (eat('}')) goto SUCCESS_NON_EMPTY_OBJECT;
    if (!eat(',')) goto ERROR_MISSING_COMMA;
  }
SUCCESS_EMPTY_OBJECT:
SUCCESS_NON_EMPTY_OBJECT:
  return object;
ERROR_INVALID_KEY:
ERROR_INVALID_VALUE:
ERROR_MISSING_BRACE:
ERROR_MISSING_COLON:
ERROR_MISSING_COMMA:
ERROR_NO_MEMORY:
  return JsonObject::invalid();
}
template <typename TReader, typename TWriter>
inline bool ArduinoJson::Internals::JsonParser<TReader, TWriter>::parseObjectTo(
    JsonVariant *destination) {
  JsonObject &object = parseObject();
  if (!object.success()) return false;
  *destination = object;
  return true;
}
template <typename TReader, typename TWriter>
inline const char *
ArduinoJson::Internals::JsonParser<TReader, TWriter>::parseString() {
  typename TypeTraits::RemoveReference<TWriter>::type::String str =
      _writer.startString();
  skipSpacesAndComments(_reader);
  char c = _reader.current();
  if (isQuote(c)) {  // quotes
    _reader.move();
    char stopChar = c;
    for (;;) {
      c = _reader.current();
      if (c == '\0') break;
      _reader.move();
      if (c == stopChar) break;
      if (c == '\\') {
        c = Encoding::unescapeChar(_reader.current());
        if (c == '\0') break;
        _reader.move();
      }
      str.append(c);
    }
  } else {  // no quotes
    for (;;) {
      if (!isLetterOrNumber(c)) break;
      _reader.move();
      str.append(c);
      c = _reader.current();
    }
  }
  return str.c_str();
}
template <typename TReader, typename TWriter>
inline bool ArduinoJson::Internals::JsonParser<TReader, TWriter>::parseStringTo(
    JsonVariant *destination) {
  bool hasQuotes = isQuote(_reader.current());
  const char *value = parseString();
  if (value == NULL) return false;
  if (hasQuotes) {
    *destination = value;
  } else {
    *destination = RawJson(value);
  }
  return true;
}
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4522)
#endif
namespace ArduinoJson {
class JsonArraySubscript : public JsonVariantBase<JsonArraySubscript> {
 public:
  FORCE_INLINE JsonArraySubscript(JsonArray& array, size_t index)
      : _array(array), _index(index) {}
  FORCE_INLINE JsonArraySubscript& operator=(const JsonArraySubscript& src) {
    _array.set(_index, src);
    return *this;
  }
  template <typename T>
  FORCE_INLINE JsonArraySubscript& operator=(const T& src) {
    _array.set(_index, src);
    return *this;
  }
  template <typename T>
  FORCE_INLINE JsonArraySubscript& operator=(const T* src) {
    _array.set(_index, src);
    return *this;
  }
  FORCE_INLINE bool success() const {
    return _index < _array.size();
  }
  template <typename T>
  FORCE_INLINE typename Internals::JsonVariantAs<T>::type as() const {
    return _array.get<T>(_index);
  }
  template <typename T>
  FORCE_INLINE bool is() const {
    return _array.is<T>(_index);
  }
  template <typename TValue>
  FORCE_INLINE bool set(const TValue& value) {
    return _array.set(_index, value);
  }
  template <typename TValue>
  FORCE_INLINE bool set(const TValue* value) {
    return _array.set(_index, value);
  }
  template <typename TValue>
  DEPRECATED("Second argument is not supported anymore")
  FORCE_INLINE bool set(const TValue& value, uint8_t) {
    return _array.set(_index, value);
  }
 private:
  JsonArray& _array;
  const size_t _index;
};
#if ARDUINOJSON_ENABLE_STD_STREAM
inline std::ostream& operator<<(std::ostream& os,
                                const JsonArraySubscript& source) {
  return source.printTo(os);
}
#endif
inline JsonArraySubscript JsonArray::operator[](size_t index) {
  return JsonArraySubscript(*this, index);
}
inline const JsonArraySubscript JsonArray::operator[](size_t index) const {
  return JsonArraySubscript(*const_cast<JsonArray*>(this), index);
}
template <typename TImplem>
inline JsonArraySubscript JsonVariantBase<TImplem>::operator[](size_t index) {
  return as<JsonArray>()[index];
}
template <typename TImplem>
inline const JsonArraySubscript JsonVariantBase<TImplem>::operator[](
    size_t index) const {
  return as<JsonArray>()[index];
}
}  // namespace ArduinoJson
#ifdef _MSC_VER
#pragma warning(pop)
#endif
namespace ArduinoJson {
inline JsonArray &JsonArray::createNestedArray() {
  if (!_buffer) return JsonArray::invalid();
  JsonArray &array = _buffer->createArray();
  add(array);
  return array;
}
inline JsonObject &JsonArray::createNestedObject() {
  if (!_buffer) return JsonObject::invalid();
  JsonObject &object = _buffer->createObject();
  add(object);
  return object;
}
}
inline ArduinoJson::JsonArray &ArduinoJson::JsonBuffer::createArray() {
  JsonArray *ptr = new (this) JsonArray(this);
  return ptr ? *ptr : JsonArray::invalid();
}
inline ArduinoJson::JsonObject &ArduinoJson::JsonBuffer::createObject() {
  JsonObject *ptr = new (this) JsonObject(this);
  return ptr ? *ptr : JsonObject::invalid();
}
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4522)
#endif
namespace ArduinoJson {
template <typename TStringRef>
class JsonObjectSubscript
    : public JsonVariantBase<JsonObjectSubscript<TStringRef> > {
  typedef JsonObjectSubscript<TStringRef> this_type;
 public:
  FORCE_INLINE JsonObjectSubscript(JsonObject& object, TStringRef key)
      : _object(object), _key(key) {}
  FORCE_INLINE this_type& operator=(const this_type& src) {
    _object.set(_key, src);
    return *this;
  }
  template <typename TValue>
  FORCE_INLINE
      typename TypeTraits::EnableIf<!TypeTraits::IsArray<TValue>::value,
                                    this_type&>::type
      operator=(const TValue& src) {
    _object.set(_key, src);
    return *this;
  }
  template <typename TValue>
  FORCE_INLINE this_type& operator=(const TValue* src) {
    _object.set(_key, src);
    return *this;
  }
  FORCE_INLINE bool success() const {
    return _object.containsKey(_key);
  }
  template <typename TValue>
  FORCE_INLINE typename Internals::JsonVariantAs<TValue>::type as() const {
    return _object.get<TValue>(_key);
  }
  template <typename TValue>
  FORCE_INLINE bool is() const {
    return _object.is<TValue>(_key);
  }
  template <typename TValue>
  FORCE_INLINE
      typename TypeTraits::EnableIf<!TypeTraits::IsArray<TValue>::value,
                                    bool>::type
      set(const TValue& value) {
    return _object.set(_key, value);
  }
  template <typename TValue>
  FORCE_INLINE bool set(const TValue* value) {
    return _object.set(_key, value);
  }
  template <typename TValue>
  DEPRECATED("Second argument is not supported anymore")
  FORCE_INLINE bool set(const TValue& value, uint8_t) {
    return _object.set(_key, value);
  }
 private:
  JsonObject& _object;
  TStringRef _key;
};
#if ARDUINOJSON_ENABLE_STD_STREAM
template <typename TStringRef>
inline std::ostream& operator<<(std::ostream& os,
                                const JsonObjectSubscript<TStringRef>& source) {
  return source.printTo(os);
}
#endif
}  // namespace ArduinoJson
#ifdef _MSC_VER
#pragma warning(pop)
#endif
namespace ArduinoJson {
template <typename TStringRef>
inline JsonArray &JsonObject::createNestedArray_impl(TStringRef key) {
  if (!_buffer) return JsonArray::invalid();
  JsonArray &array = _buffer->createArray();
  set(key, array);
  return array;
}
template <typename TStringRef>
inline JsonObject &JsonObject::createNestedObject_impl(TStringRef key) {
  if (!_buffer) return JsonObject::invalid();
  JsonObject &object = _buffer->createObject();
  set(key, object);
  return object;
}
}
namespace ArduinoJson {
namespace Polyfills {
inline bool isdigit(char c) {
  return '0' <= c && c <= '9';
}
inline bool issign(char c) {
  return '-' == c || c == '+';
}
}
}
namespace ArduinoJson {
namespace Polyfills {
inline bool isFloat(const char* s) {
  if (!s) return false;
  if (!strcmp(s, "NaN")) return true;
  if (issign(*s)) s++;
  if (!strcmp(s, "Infinity")) return true;
  if (*s == '\0') return false;
  while (isdigit(*s)) s++;
  if (*s == '.') {
    s++;
    while (isdigit(*s)) s++;
  }
  if (*s == 'e' || *s == 'E') {
    s++;
    if (issign(*s)) s++;
    if (!isdigit(*s)) return false;
    while (isdigit(*s)) s++;
  }
  return *s == '\0';
}
}
}
namespace ArduinoJson {
namespace Polyfills {
inline bool isInteger(const char* s) {
  if (!s) return false;
  if (issign(*s)) s++;
  while (isdigit(*s)) s++;
  return *s == '\0';
}
}
}
namespace ArduinoJson {
namespace Polyfills {
template <typename T>
inline T parseFloat(const char* s) {
  typedef TypeTraits::FloatTraits<T> traits;
  typedef typename traits::mantissa_type mantissa_t;
  typedef typename traits::exponent_type exponent_t;
  if (!s) return 0;  // NULL
  bool negative_result = false;
  switch (*s) {
    case '-':
      negative_result = true;
    case '+':
      s++;
  }
  if (*s == 't') return 1;  // true
  if (*s == 'n' || *s == 'N') return traits::nan();
  if (*s == 'i' || *s == 'I')
    return negative_result ? -traits::inf() : traits::inf();
  mantissa_t mantissa = 0;
  exponent_t exponent_offset = 0;
  while (isdigit(*s)) {
    if (mantissa < traits::mantissa_max / 10)
      mantissa = mantissa * 10 + (*s - '0');
    else
      exponent_offset++;
    s++;
  }
  if (*s == '.') {
    s++;
    while (isdigit(*s)) {
      if (mantissa < traits::mantissa_max / 10) {
        mantissa = mantissa * 10 + (*s - '0');
        exponent_offset--;
      }
      s++;
    }
  }
  int exponent = 0;
  if (*s == 'e' || *s == 'E') {
    s++;
    bool negative_exponent = false;
    if (*s == '-') {
      negative_exponent = true;
      s++;
    } else if (*s == '+') {
      s++;
    }
    while (isdigit(*s)) {
      exponent = exponent * 10 + (*s - '0');
      if (exponent + exponent_offset > traits::exponent_max) {
        if (negative_exponent)
          return negative_result ? -0.0f : 0.0f;
        else
          return negative_result ? -traits::inf() : traits::inf();
      }
      s++;
    }
    if (negative_exponent) exponent = -exponent;
  }
  exponent += exponent_offset;
  T result = traits::make_float(static_cast<T>(mantissa), exponent);
  return negative_result ? -result : result;
}
}
}
namespace ArduinoJson {
namespace Polyfills {
template <typename T>
T parseInteger(const char *s) {
  if (!s) return 0;  // NULL
  if (*s == 't') return 1;  // "true"
  T result = 0;
  bool negative_result = false;
  switch (*s) {
    case '-':
      negative_result = true;
    case '+':
      s++;
      break;
  }
  while (isdigit(*s)) {
    result = T(result * 10 + T(*s - '0'));
    s++;
  }
  return negative_result ? T(~result + 1) : result;
}
}
}
namespace ArduinoJson {
inline JsonVariant::JsonVariant(const JsonArray &array) {
  if (array.success()) {
    _type = Internals::JSON_ARRAY;
    _content.asArray = const_cast<JsonArray *>(&array);
  } else {
    _type = Internals::JSON_UNDEFINED;
  }
}
inline JsonVariant::JsonVariant(const JsonObject &object) {
  if (object.success()) {
    _type = Internals::JSON_OBJECT;
    _content.asObject = const_cast<JsonObject *>(&object);
  } else {
    _type = Internals::JSON_UNDEFINED;
  }
}
inline JsonArray &JsonVariant::variantAsArray() const {
  if (_type == Internals::JSON_ARRAY) return *_content.asArray;
  return JsonArray::invalid();
}
inline JsonObject &JsonVariant::variantAsObject() const {
  if (_type == Internals::JSON_OBJECT) return *_content.asObject;
  return JsonObject::invalid();
}
template <typename T>
inline T JsonVariant::variantAsInteger() const {
  using namespace Internals;
  switch (_type) {
    case JSON_UNDEFINED:
      return 0;
    case JSON_POSITIVE_INTEGER:
    case JSON_BOOLEAN:
      return T(_content.asInteger);
    case JSON_NEGATIVE_INTEGER:
      return T(~_content.asInteger + 1);
    case JSON_STRING:
    case JSON_UNPARSED:
      return Polyfills::parseInteger<T>(_content.asString);
    default:
      return T(_content.asFloat);
  }
}
inline const char *JsonVariant::variantAsString() const {
  using namespace Internals;
  if (_type == JSON_UNPARSED && _content.asString &&
      !strcmp("null", _content.asString))
    return NULL;
  if (_type == JSON_STRING || _type == JSON_UNPARSED) return _content.asString;
  return NULL;
}
template <typename T>
inline T JsonVariant::variantAsFloat() const {
  using namespace Internals;
  switch (_type) {
    case JSON_UNDEFINED:
      return 0;
    case JSON_POSITIVE_INTEGER:
    case JSON_BOOLEAN:
      return static_cast<T>(_content.asInteger);
    case JSON_NEGATIVE_INTEGER:
      return -static_cast<T>(_content.asInteger);
    case JSON_STRING:
    case JSON_UNPARSED:
      return Polyfills::parseFloat<T>(_content.asString);
    default:
      return static_cast<T>(_content.asFloat);
  }
}
inline bool JsonVariant::variantIsBoolean() const {
  using namespace Internals;
  if (_type == JSON_BOOLEAN) return true;
  if (_type != JSON_UNPARSED || _content.asString == NULL) return false;
  return !strcmp(_content.asString, "true") ||
         !strcmp(_content.asString, "false");
}
inline bool JsonVariant::variantIsInteger() const {
  using namespace Internals;
  return _type == JSON_POSITIVE_INTEGER || _type == JSON_NEGATIVE_INTEGER ||
         (_type == JSON_UNPARSED && Polyfills::isInteger(_content.asString));
}
inline bool JsonVariant::variantIsFloat() const {
  using namespace Internals;
  return _type == JSON_FLOAT || _type == JSON_POSITIVE_INTEGER ||
         _type == JSON_NEGATIVE_INTEGER ||
         (_type == JSON_UNPARSED && Polyfills::isFloat(_content.asString));
}
#if ARDUINOJSON_ENABLE_STD_STREAM
inline std::ostream &operator<<(std::ostream &os, const JsonVariant &source) {
  return source.printTo(os);
}
#endif
}  // namespace ArduinoJson
template <typename Writer>
inline void ArduinoJson::Internals::JsonSerializer<Writer>::serialize(
    const JsonArray& array, Writer& writer) {
  writer.beginArray();
  JsonArray::const_iterator it = array.begin();
  while (it != array.end()) {
    serialize(*it, writer);
    ++it;
    if (it == array.end()) break;
    writer.writeComma();
  }
  writer.endArray();
}
template <typename Writer>
inline void ArduinoJson::Internals::JsonSerializer<Writer>::serialize(
    const JsonArraySubscript& arraySubscript, Writer& writer) {
  serialize(arraySubscript.as<JsonVariant>(), writer);
}
template <typename Writer>
inline void ArduinoJson::Internals::JsonSerializer<Writer>::serialize(
    const JsonObject& object, Writer& writer) {
  writer.beginObject();
  JsonObject::const_iterator it = object.begin();
  while (it != object.end()) {
    writer.writeString(it->key);
    writer.writeColon();
    serialize(it->value, writer);
    ++it;
    if (it == object.end()) break;
    writer.writeComma();
  }
  writer.endObject();
}
template <typename Writer>
template <typename TKey>
inline void ArduinoJson::Internals::JsonSerializer<Writer>::serialize(
    const JsonObjectSubscript<TKey>& objectSubscript, Writer& writer) {
  serialize(objectSubscript.template as<JsonVariant>(), writer);
}
template <typename Writer>
inline void ArduinoJson::Internals::JsonSerializer<Writer>::serialize(
    const JsonVariant& variant, Writer& writer) {
  switch (variant._type) {
    case JSON_FLOAT:
      writer.writeFloat(variant._content.asFloat);
      return;
    case JSON_ARRAY:
      serialize(*variant._content.asArray, writer);
      return;
    case JSON_OBJECT:
      serialize(*variant._content.asObject, writer);
      return;
    case JSON_STRING:
      writer.writeString(variant._content.asString);
      return;
    case JSON_UNPARSED:
      writer.writeRaw(variant._content.asString);
      return;
    case JSON_NEGATIVE_INTEGER:
      writer.writeRaw('-');
    case JSON_POSITIVE_INTEGER:
      writer.writeInteger(variant._content.asInteger);
      return;
    case JSON_BOOLEAN:
      writer.writeBoolean(variant._content.asInteger != 0);
      return;
    default:  // JSON_UNDEFINED
      return;
  }
}

using namespace ArduinoJson;
