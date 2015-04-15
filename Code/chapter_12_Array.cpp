#include <iostream>
#include <exception>

template<typename T> class Array {
public:
    Array() : data(NULL), sz(0) { }
    Array(unsigned size) : sz(size), data(new T[size]) { }
    ~Array() { delete[] data; }
    const T& operator[](unsigned n) const {
        if(n >= sz || data == NULL)
            throw "Array subscript out of range";
        return data[n];    
    }
    T& operator[](unsigned n) {
        if(n >= sz || data == NULL)
            throw "Array subscript out of range";
        return data[n];
    }
#if 0
    operator const T*() const { /* 允许Array到指向它第一个元素的指针的转换 */
        return data;
    }
    operator T*() { return data; }
#endif
private:
    T* data;
    unsigned sz;
    bool resize(unsigned);
    Array(const Array&); /* 模仿内建数组，禁止复制 */
    Array& operator=(const Array&); /* 禁止赋值 */
};
void f() {
    int *p;
    {
        Array<int> x(20);
        p = &x[10];
    }
    std::cout << *p << std::endl; /* oh, shit !!! */
}
int main(int argc, char *argv[]) {
    Array<int> x(10);
    Array<Array<int>> y(10);
    f();
    return 0;
}
