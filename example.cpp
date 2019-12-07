#include <iostream>

#include "stackallocator.h"

struct Foo
{
    int val;
};

int main()
{
    // create an allocator with 128 bytes of memory on the stack
    MonotonicStackAllocator<128> allocator;

    // allocate an array of 16 objects of type Foo
    Foo* myFirstArray = allocator.AllocObjects<Foo>(16);
    
    // construct and destroy objects in-place
    for (size_t i = 0; i < 16; ++i)
    {
        Foo* f = new(myFirstArray + i) Foo{ static_cast<int>(i) };
        std::cout << "Element " << i << " has value " << myFirstArray[i].val << "\n";
        f->~Foo();
    }

    // allocate an untyped memory chunk and use it as an array of floats
    void* chunk = allocator.Alloc(16 * sizeof(float), alignof(float));
    float* myFloatArray = reinterpret_cast<float*>(chunk);

    myFloatArray[0] = 1.f;
    for (size_t i = 1;i < 16; ++i)
    {
        myFloatArray[i] = myFloatArray[i-1] + i;
    }

    for (size_t i = 0; i < 16; ++i)
    {
        std::cout << "Float " << i << " has value " << myFloatArray[i] << "\n";
    }

    // release the memory
    allocator.FreeAll();

    // allocate an array of 32 objects of type Foo
    Foo* mySecondArray = allocator.AllocObjects<Foo>(32);
    
    // explicit construction and destruction is not necessary
    for (size_t i = 0; i < 32; ++i)
    {
        mySecondArray[i] = Foo{ static_cast<int>(i) };
    }

    for (size_t i = 0; i < 32; ++i)
    {
        std::cout << "Element " << i << " has value " << mySecondArray[i].val << "\n";
    }

    return 0;
}
