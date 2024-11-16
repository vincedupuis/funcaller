#include "ThreadedFunctionQueue.h"
#include "Foo.h"

#include <spdlog/spdlog.h>
#include <csignal>
#include <utility>

using namespace spdlog;

auto MainQueue = std::make_shared<util::ThreadedFunctionQueue>();
void signalHandler(int signal);
std::jthread mainThread;

class Bar {
public:
    explicit Bar(std::shared_ptr<IFoo> foo)
        : foo(std::move(foo))
    {
    }

    void bar1() const {
        foo->foo1();
    }

    void bar2(int a) const {
        foo->foo2(a);
    }

    void bar3(std::shared_ptr<int> ptr) const {
        foo->foo3(std::move(ptr));
    }

private:
    std::shared_ptr<IFoo> foo;
};

int main()
{
    // Set up signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    info("Starting main queue");
    MainQueue->start();

    auto foo = std::make_shared<Foo>();

    auto fooContextSwitch = std::make_shared<FooContextSwitch>();
    fooContextSwitch->configure(MainQueue, foo);

    auto bar = std::make_shared<Bar>(fooContextSwitch);
    bar->bar1();
    bar->bar2(42);
    bar->bar3(std::make_shared<int>(55));

    mainThread = std::jthread([] (const std::stop_token& stopToken) {
        while (!stopToken.stop_requested()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
    mainThread.join();

    info("Stopping main queue");
    MainQueue->stop();

    return 0;
}

void signalHandler(int signal)
{
    info("Received signal: {}", signal);
    if (signal == SIGINT || signal == SIGTERM)
    {
        mainThread.request_stop();
    }
}
