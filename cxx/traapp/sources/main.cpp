#include <iostream>
#include <traaxx/traaxx.hpp>

int main()
{
    auto r = traaxx::scale(5, 2);
    std::cout                 //
        << "scale(5, 2) = "   //
        << r                  //
        << "\n";
    return 0;
}
