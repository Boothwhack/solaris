#pragma once

#include <string>

struct ComponentA {
  int value;
};

struct ComponentB {
  long double value;
};

struct ComponentC {
  std::string value;
};

struct Moveable {
  int value;

  explicit Moveable(int v) : value{v} {}
  ~Moveable() { value = 0; }
};
