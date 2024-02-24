#include "solaris/core/dispatcher.hpp"
#include "solaris/core/layer.hpp"
#include "solaris/core/layer_stack.hpp"
#include <iostream>
#include <solaris/core/bus.hpp>

using namespace solaris::core;

using std::cout;
using std::endl;

struct EventA {};
struct EventB {};
struct EventC {};

class MainLayer : public Layer<MainLayer, int> {
public:
  void setup(LayerHandlers handlers) {
    handlers.addInstanceHandler<EventA>(&MainLayer::onA);
    handlers.addStaticHandler<EventB>(&MainLayer::onB);
    // ignore EventC
  }

  void onA(Context<EventA> context) {
    cout << "MainLayer: Got A - " << *context << endl;
    ++*context;
    context.next();
  }

  static void onB(Context<EventB> context) {
    cout << "MainLayer: Got B - " << *context << endl;
    ++*context;
    context.next();
    cout << "MainLayer: After delegate B - " << *context << endl;
  }
};

class SecondLayer : public Layer<SecondLayer, int> {
public:
  void setup(LayerHandlers handlers) {
    // ignore EventA
    handlers.addInstanceHandler<EventB>(&SecondLayer::onB);
    handlers.addStaticHandler<EventC>(&SecondLayer::onC);
  }

  void onB(Context<EventB> context) {
    cout << "SecondLayer: Got B - " << *context << endl;
    ++*context;
    context.next();
    cout << "SecondLayer: After delegate B - " << *context << endl;
  }

  static void onC(Context<EventC> context) {
    cout << "SecondLayer: Got C - " << *context << endl;
    ++*context;
    context.next();
  }
};

int main() {
  LayerStack<int> layers{};
  layers.addLayer<MainLayer>();
  layers.addLayer<SecondLayer>();

  auto bus{layers.compileBus()};

  int counter{0};
  bus.dispatch(EventA(), counter);
  bus.dispatch(EventB(), counter);
  bus.dispatch(EventC(), counter);

  cout << "Final counter: " << counter << endl;
}