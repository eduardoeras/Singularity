#include<iostream>

class AllConditions {
public:
    AllConditions (int a, int b, int c) : a_(a), b_(b), c_(c) {}

    AllConditions ()
    {
        std::cout << "Empty Constructor" << std::endl;
    }

    int function (int var)
    {
        if (var == 1)
        {
            std::cout << "content_if" << std::endl;
        }
        else if (var = 2)
        {
            std::cout << "content_else_if" << std::endl;
        }
        else
        {
            std::cout << "content_else" << std::endl;
        }


        for (int i=0; i < 2; i++)
        {
            std::cout << "content_for" << std::endl;
        }

        for (int e : {1, 2, 3})
        {
            std::cout << "content_enhanced_for" << std::endl;
        }

        while (var == 2)
        {
            std::cout << "content_while" << std::endl;
            continue;
        }

        do
        {
            std::cout << "content_do" << std::endl;
        } while (var == 3);

        switch(var) {
        case 0 :
            std::cout << "content_case_0_switch" << std::endl;
            break;
        case 1 :
            std::cout << "content_case_1_switch" << std::endl;
            break;
        default :
            std::cout << "content_default" << std::endl;
        }

        return 0;
    }

    private :
        const int a_ = 0;
        const int b_ = 0;
        const int c_ = 0;
};

struct Structure {
    Structure () {
        std::cout << "Empty Structure Constructor" << std::endl;
    }
    int structFunction ()
    {
        if (true) {
            std::cout << "Struct If Content" << std::endl;
        }
        if ((true)) return 0;
        return 1;
    }
};