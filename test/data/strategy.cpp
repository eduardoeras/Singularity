#include <iostream>

class Strategy {
public:
    virtual void Print() const = 0;
};

class StrategyA : public Strategy {
public:
    void Print() const override
    {
        std::cout << "A" << std::endl;
    }
};

class StrategyB : public Strategy {
public:
    void Print() const override
    {
        std::cout << "B" << std::endl;
    }
};

class MyClass {
public:
    MyClass(const Strategy& strategy) : strategy_(strategy){}
    
    void Print() const
    {
        strategy_.Print();
    }
private:
    const Strategy& strategy_;
};

int main()
{
    MyClass(StrategyA()).Print();
    MyClass(StrategyB()).Print();
    return 0;
}
