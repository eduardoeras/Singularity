#include<iostream>

void function(int input)
{
    /*std::cout << "Before" << std::endl;
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
    std::cout << "after" << std::endl;*/

    std::cout << "before" << std::endl;
    switch (input)
    {
        case 1:
            std::cout << "one" << std::endl;
            break;
        case 2:
        case 3:
        case 4:
            std::cout << "two three four" << std::endl;
            break;
        case 5:
            std::cout << "five" << std::endl;
        case 6:
            std::cout << "six" << std::endl;
        case 7:
            std::cout << "seven1" << std::endl;
            std::cout << "seven2" << std::endl;
            std::cout << "seven3" << std::endl;
            std::cout << "seven4" << std::endl;
            std::cout << "seven5" << std::endl;
            break;
        default :
            std::cout << "def" << std::endl;
    }
    std::cout << "after" << std::endl;

    switch (input) {
    }

    std::cout << "aftermath" << std::endl;
}