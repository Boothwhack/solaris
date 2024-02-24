#include <iostream>
#include <solaris/core/bus.hpp>
#include <solaris/core/dispatcher.hpp>
#include <solaris/core/layer.hpp>
#include <solaris/core/layer_stack.hpp>
#include <solaris/core/queue.hpp>

using namespace solaris::core;

using std::cout;
using std::endl;

struct EventA {};
struct EventB {};
struct EventC {};

class MainLayer : public Layer<MainLayer, int> {
public:
  void setup(Handlers handlers) {
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
  void setup(Handlers handlers) {
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
  Queue<int> queue{};
  queue.enqueue<EventA>();
  queue.enqueue<EventB>();
  queue.enqueue<EventC>();
  queue.dispatchOn(bus, counter);

  cout << "Final counter: " << counter << endl;
}