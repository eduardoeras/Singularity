#include <string>

class Triangle {
public:
    Triangle (int a, int b, int c) : a_(a), b_(b), c_(c) {}
    const int a_;
    const int b_;
    const int c_;
};

class Tool {
public:
    std::string classify (Triangle t)
    {
        if (is_valid(t))
        {
            if (t.a_ == t.b_ && t.a_ == t.c_)
            {
                return "Equilateral";
            }
            else if (t.a_ != t.b_ && t.a_ != t.c_ && t.b_ != t.c_)
            {
                return "Scalene";
            }
            else
            {
                return "Isosceles";
            }
        }
        else
        {
            return "Invalid";
        }
    }
private:
    bool is_valid (Triangle t)
    {
        if (t.a_ <= 0)
        {
            return false;
        }
        if (t.b_ <= 0)
        {
            return false;
        }
        if (t.c_ <= 0)
        {
            return false;
        }
        if (t.a_ + t.b_ <= t.c_)
        {
            return false;
        }
        if (t.a_ + t.c_ <= t.b_)
        {
            return false;}
        if (t.b_ + t.c_ <= t.a_)
        {
            return false;
        }
        return true;
    }
};