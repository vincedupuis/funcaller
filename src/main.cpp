#include "ThreadedFunctionQueue.h"
#include "Foo.h"

#include <csignal>
#include <utility>

auto MainQueue = std::make_shared<funcall::ThreadedFunctionQueue>();
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

    MainQueue->start();
    printf("Main queue started\n");

    const auto foo = std::make_shared<Foo>();

    auto fooContextSwitch = std::make_shared<FooContextSwitch>();
    fooContextSwitch->setup(foo, MainQueue);

    const auto bar = std::make_shared<Bar>(fooContextSwitch);

    // these calls will be executed in the main thread
    bar->bar1();
    bar->bar2(42);
    bar->bar3(std::make_shared<int>(55));

    mainThread = std::jthread([] (const std::stop_token& stopToken) {
        while (!stopToken.stop_requested()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    // Wait for main thread to stop
    mainThread.join();
    printf("Main thread stopped\n");

    MainQueue->stop();
    printf("Main queue stopped\n");

    return 0;
}

void signalHandler(int signal)
{
    printf("Received signal %d\n", signal);
    if (signal == SIGINT || signal == SIGTERM)
    {
        mainThread.request_stop();
    }
}
