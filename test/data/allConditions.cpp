class AllConditions {
public:
    void function (int var)
    {
        if (var == 1)
        {
            std::cout << "content_if" << std::endl;
        }

        for (int i=0; i < 2; i++)
        {
            std::cout << "content_for" << std::endl;
        }

        while (var == 2)
        {
            std::cout << "content_while" << std::endl;
        }

        do
        {
            std::cout << "content_do" << std::endl;
        } while (var == 3);

        switch(var) {
        case 0 :
            std::cout << "content_case_switch" << std::endl;
            break;
        default :
            std::cout << "content_default" << std::endl;
        }
    }
};