#ifndef MEASURE_H
#define MEASURE_H

#include <time.h>

#include <string>

namespace activebsp
{
    struct timespec now();

    double diff_to_sec(const struct timespec * beg, const struct timespec * end);


    class Measure
    {
    private:
        std::string _name;

        struct timespec _start;
        struct timespec _end;

    public:
        Measure(std::string name);

        void start();
        void end();

        std::string getName() const;
        double getTime() const;
    };

}

#endif // MEASURE_H
