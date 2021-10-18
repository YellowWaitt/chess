#pragma once

#include <iterator>

class Random {
    public:
        Random();

        int uniform(int a, int b);

        template <typename Container>
        typename Container::value_type extract(Container& c) {
            auto it = std::begin(c);
            std::advance(it, uniform(0, c.size() - 1));
            auto ret = *it;
            c.erase(it);
            return ret;
        }
        
        template <typename Container>
        inline
        typename Container::value_type random(Container const& c) {
            return c[uniform(0, c.size() - 1)];
        }
        
    private:
        static bool needInit;
        static void init();
};
