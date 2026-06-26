#include "app/application.h"

#include <exception>
#include <iostream>

namespace {

constexpr int EXIT_STD_EXCEPTION = -1;
constexpr int EXIT_UNKNOWN_EXCEPTION = -2;

}  // namespace

int main(int argc, char* argv[])
{
    Application app;
    try
    {
        app.configure();
        return app.start(argc, argv);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Ошибка: " << e.what() << '\n';
        return EXIT_STD_EXCEPTION;
    }
    catch(...)
    {
        std::cerr << "Неизвестная ошибка.\n";
        return EXIT_UNKNOWN_EXCEPTION;
    }
}
