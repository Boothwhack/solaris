#include <iostream>
#include <solaris/core/bus.hpp>
#include <solaris/core/dispatcher.hpp>
#include <solaris/core/layer.hpp>
#include <solaris/core/layer_stack.hpp>
#include <solaris/core/queue.hpp>
#include <solaris/framework/resources.hpp>

using namespace solaris::core;
using namespace solaris::framework;

using std::cout;
using std::endl;

struct EventA {};
struct EventB {};
struct EventC {};

class Counter {
  int m_Counter{0};

public:
  void increment() {
    ++m_Counter;
  }

  [[nodiscard]] int getCounter() const {
    return m_Counter;
  }
};

class MainLayer : public Layer<MainLayer, Resources> {
public:
  void setup(Handlers handlers) {
    handlers.addInstanceHandler<EventA>(&MainLayer::onA);
    handlers.addStaticHandler<EventB>(&MainLayer::onB);
    // ignore EventC
  }

  void onA(Context<EventA> context) {
    cout << "MainLayer: Got A - " << context->get<Counter>().getCounter() << endl;
    context->get<Counter>().increment();
    context.next();
  }

  static void onB(Context<EventB> context) {
    cout << "MainLayer: Got B - " << context->get<Counter>().getCounter() << endl;
    context->get<Counter>().increment();
    context.next();
    cout << "MainLayer: After delegate B - " << context->get<Counter>().getCounter() << endl;
  }
};

class SecondLayer : public Layer<SecondLayer, Resources> {
public:
  void setup(Handlers handlers) {
    // ignore EventA
    handlers.addInstanceHandler<EventB>(&SecondLayer::onB);
    handlers.addStaticHandler<EventC>(&SecondLayer::onC);
  }

  void onB(Context<EventB> context) {
    cout << "SecondLayer: Got B - " << context->get<Counter>().getCounter() << endl;
    context->get<Counter>().increment();
    context.next();
    cout << "SecondLayer: After delegate B - " << context->get<Counter>().getCounter() << endl;
  }

  static void onC(Context<EventC> context) {
    cout << "SecondLayer: Got C - " << context->get<Counter>().getCounter() << endl;
    context->get<Counter>().increment();
    context.next();
  }
};

int main() {
  ResourceOwners<> resources{};
  auto withCounter{resources.withResource<Counter>()};

  LayerStack<Resources> layers{};
  layers.addLayer<MainLayer>();
  layers.addLayer<SecondLayer>();

  auto bus{layers.compileBus()};

  Queue<Resources> queue{};
  queue.enqueue<EventA>();
  queue.enqueue<EventB>();
  queue.enqueue<EventC>();
  queue.dispatchOn(bus, withCounter);

  cout << "Final counter: " << withCounter.get<Counter>().getCounter() << endl;
}