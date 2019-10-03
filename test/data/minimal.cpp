#include<iostream>

void function()
{
    std::cout << "Before" << std::endl;
    if (true)
    {
        std::cout << "Inside" << std::endl;
        if (true)
        {
            std::cout << "Inside" << std::endl;
            if (true)
            {
                std::cout << "Inside" << std::endl;
            }
        }
    }
    else
    {
        std::cout << "Inside Else" << std::endl;
    }
    std::cout << "After" << std::endl;
}