#include <iostream>
#include <solaris/framework/bus.hpp>

using namespace solaris::framework;

using std::cout;
using std::endl;

struct EventA {};
struct EventB {};
struct EventC {};

int main() {
    Bus<int> bus{};

    bus.getDispatcherFor<EventA>().addHandler([](auto context) { cout << "Got A" << endl; ++*context; });
    int counter;
    bus.dispatch(EventA{}, counter);
    cout << "Final counter: " << counter << endl;
}