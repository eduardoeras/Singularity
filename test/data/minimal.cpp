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

    std::cout << "before" << std::endl;
    for (int i = 0; i < 10; i++)
    {
        std::cout << "inside" << std::endl;
        for (int i = 0; i < 10; i++)
            {
                std::cout << "inside" << std::endl;
            }
    }
    std::cout << "after" << std::endl;
}