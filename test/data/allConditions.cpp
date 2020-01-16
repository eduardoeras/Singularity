#include<iostream>

class AllConditions {
public:
    AllConditions (int a, int b, int c) : a_(a), b_(b), c_(c) {}

    AllConditions ()
    {
        std::cout << "Empty Constructor" << std::endl;
    }

    int function (int input_variable)
    {
        if (input_variable == 1)
        {
            std::cout << "content_if" << std::endl;
            return true;
        }
        else if (input_variable = 2)
        {
            std::cout << "content_else_if" << std::endl;
        }
        else
        {
            std::cout << "content_else" << std::endl;
        }


        for (int i = 0; i < 2; i++)
        {
            std::cout << "content_for" << std::endl;
        }

        for (int e : {1, 2, 3})
        {
            std::cout << "content_enhanced_for" << std::endl;
        }

        while (input_variable == 2)
        {
            std::cout << "content_while" << std::endl;
            continue;
        }

        do
        {
            std::cout << "content_do_1" << std::endl;
            std::cout << "content_do_2" << std::endl;
            std::cout << "content_do_3" << std::endl;
            input_variable ++;
        } while (input_variable < 3);

        switch(input_variable) {
        case 1 :
            std::cout << "content_switch_case_1" << std::endl;
            break;
        case 2 :
            std::cout << "content_switch_case_2" << std::endl;
            return false;
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
    int structFunction (bool function_input)
    {
        if (true) {
            std::cout << "Struct function If Content" << std::endl;
        }
        if ((function_input)) return 0;
        return 1;
    }
};