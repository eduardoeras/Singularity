#include<iostream>

class Class {
public:
    int function(int input)
    {
        std::cout << "Before while" << std::endl;
        while (1 > input)
        {
            input ++;
            std::cout << "Inside while" << std::endl;
        }
        std::cout << "After while" << std::endl;


        std::cout << "Before do" << std::endl;
        do
        {
            input --;
            std::cout << "Inside do" << std::endl;
        }
        while (1 < input);
        std::cout << "After do" << std::endl;

        std::cout << "Before if cluster" << std::endl;
        if (true)
        {
            std::cout << "Inside first if" << std::endl;
            if (true)
            {
                std::cout << "Inside second if" << std::endl;
                if (true)
                {
                    std::cout << "Inside third if" << std::endl;
                }
            }
        }
        else
        {
            std::cout << "Inside else" << std::endl;
        }
        std::cout << "After if cluster" << std::endl;

        std::cout << "Before for matrix" << std::endl;
        for (int i = 0; i < 10; i++)
        {
            std::cout << "Inside first for" << std::endl;
            for (int i = 0; i < 10; i++)
                {
                    std::cout << "Inside second for" << std::endl;
                }
        }
        std::cout << "After for matrix" << std::endl;

        std::cout << "Before switch case" << std::endl;
        switch (input)
        {
            case 1:
                std::cout << "Case one" << std::endl;
                break;
            case 2:
            case 3:
            case 4:
                std::cout << "Case two three four" << std::endl;
                break;
            case 5:
                std::cout << "Case five" << std::endl;
                return 5;
            case 6:
                std::cout << "Case six" << std::endl;
            case 7:
                std::cout << "Case seven1" << std::endl;
                std::cout << "Case seven2" << std::endl;
                std::cout << "Case seven3" << std::endl;
                std::cout << "Case seven4" << std::endl;
                std::cout << "Case seven5" << std::endl;
                break;
            default :
                std::cout << "Default" << std::endl;
        }
        std::cout << "After switch case" << std::endl;

        std::cout << "Before function call" << std::endl;
        input = input + private_function();
        std::cout << "After function call" << std::endl;

        std::cout << "Before try catch" << std::endl;
        try
        {
            std::cout << "Inside try" << std::endl;
            if (true)
            {
                std::cout << "Inside if inside try" << std::endl;
            }
        }
        catch(std::exception e)
        {
            std::cout << "Inside first catch" << std::endl;
        }
        catch(std::exception e)
        {
            std::cout << "Inside second catch" << std::endl;
        }
        catch(std::exception e)
        {
            std::cout << "Inside third catch" << std::endl;
        }
        catch(std::exception e)
        {
            std::cout << "Inside fourth catch" << std::endl;
        }
        std::cout << "After try catch" << std::endl;

        std::cout << "Before final while" << std::endl;
        while (true)
        {
            std::cout << "Before if inside while" << std::endl;
            if (true)
            {
                break;
            }
            std::cout << "After if inside while" << std::endl;
        }
        std::cout << "After final while" << std::endl;

        std::cout << "After all" << std::endl;

    }

private:
    int private_function ()
    {
        std::cout << "Inside private function" << std::endl;
        return 0;
    }
};