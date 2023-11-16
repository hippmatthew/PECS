#include "core.hpp"
#include "objects.hpp"

namespace pecs
{

class Engine
{
class Main
{
            public:
                Main(const Engine& e) : engine(&e) {}
                
                virtual ~Main()
                {
                    for (Object& object : objects)
                        object.destroyObject(*engine);
                }
                
                virtual void operator()() = 0;

                void instantiateObject(Object& o)
                {
                    objects.push_back(std::move(o));
                }

            protected:
                std::vector<Object> objects;

            private:
                const Engine * engine = nullptr;
};
};

}
