#include <unistd.h>
#include <iostream>
#include <fstream>

int main()
{
    int counter = 0;
    while (true)
    {
        std::cout << counter++ << std::endl;
        sleep(1);
    }

    return 0;
}
