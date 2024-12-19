#include "Bar.h"
#include "Foo.h"
#include "FooContextSwitch.h"
#include "ThreadFunctionQueue.h"

#include <csignal>

void signalHandler(int signal);
std::jthread mainThread;

int main()
{
    // Set up signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    const auto MainQueue = std::make_shared<funcall::ThreadFunctionQueue>(
        [](std::string &&message) { printf("%s\n", message.c_str()); });
    MainQueue->start();

    const auto foo = std::make_shared<Foo>();

    auto fooContextSwitch = std::make_shared<FooContextSwitch>();
    fooContextSwitch->setup(foo, MainQueue);

    const auto bar = std::make_shared<Bar>(fooContextSwitch);

    // these calls will be executed on the Main Queue
    bar->bar1(); // this one throws an exception
    bar->bar2(42);
    bar->bar3(std::make_shared<int>(55));

    mainThread = std::jthread([](const std::stop_token &stopToken) {
        while (!stopToken.stop_requested()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    // Wait for main thread to stop
    mainThread.join();
    printf("Main thread stopped\n");

    MainQueue->stop();

    return 0;
}

void signalHandler(const int signal)
{
    printf("Received signal %d\n", signal);
    if (signal == SIGINT || signal == SIGTERM) {
        mainThread.request_stop();
    }
}
