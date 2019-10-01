#include<iostream>

void function()
{
    std::cout << "Before If" << std::endl;
    if (true)
    {
        std::cout << "Inside If 1" << std::endl;
        std::cout << "Inside If 2" << std::endl;
        std::cout << "Inside If 3" << std::endl;
    }
    else if (true)
    {
        std::cout << "Inside ElseIf 1" << std::endl;
        std::cout << "Inside ElseIf 2" << std::endl;
        std::cout << "Inside ElseIf 3" << std::endl;
    }
    else
    {
        std::cout << "Inside Else 1" << std::endl;
        std::cout << "Inside Else 2" << std::endl;
        std::cout << "Inside Else 3" << std::endl;

    }
    std::cout << "After If" << std::endl;
}