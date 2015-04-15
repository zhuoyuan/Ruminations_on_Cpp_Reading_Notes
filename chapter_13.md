
### 访问容器中的元素

上一章我们设计并实现了Array<T>的类，由于想让这个类的行为尽量靠近内建数组，我们保留了Array<T>和指向T的指针之间的关系，这种考虑使得Array类存在两个明显的问题：
- 包含元素的Array消失后，他的元素地址还存在
- 允许用户访问他的元素的地址，违背封装的概念

这一章通过引入中间层，不仅成功的保留了指针的表达能力，同时避免了上面的不足

##### **模拟指针**
直接用指针指向Array<T>的元素存在安全隐患，那么最有效的方式还是增加中间层，这里我们可以定义一个Pointer类，这个类具有类似指针的特性
```
template<typename T> class Pointer {
public:
    Pointer(Array<T>& a, unsigned n) : ap(&a), sub(n) { }
    Pointer() : ap(NULL), sub(0) { }
    //...
private:
    Array<T>* ap;
    unsigned sub; //下标
};
```

##### **获取数据**

我们使用Pointer的目的就是要避免暴露元素的地址，这暗示我们重载*操作符时应该返回一个值而不是引用
```
template<typename T> class Pointer {
public:
    //T,而不是T&
    T operator*() const {
        if(ap == NULL)
            throw "* of unbound Pointer";
        return (*ap)[sub];
    }
```
这样做我们将不能对指针赋值，我们不得不定义一个updata的函数，这样又回到了以前的问题，我们失去了定义类似Array<Array<int>>的能力，到底怎么选择取决与我们的类会被如何使用

##### **悬空指针**
现在，如果Array不存在了，还可能存在一个指向他的某个元素的悬空的Pointer，其实还是内存管理的问题. 前面章节我们讨论的句柄已经暗示了解决方案，应该增加一个中间层，将数据和句柄分开，并使用引用计数避免复制

```
template <typename T>
class Array_data { /* 数据 */
    friend class Array<T>;
    friend class Pointer<T>;
    Array_data(unsigned int size = 0) : data(new T[size]), sz(size), use(1) { }
    const T& operator[](unsigned int n) const {
        if(n >= sz)
            throw "Array subscript out of range!";
        return data[n];
    }
    T& operator[](unsigned int n) {
        if(n >= sz)
            throw "Array subscript out of range!";
        return data[n];
    }
    //没有实现，不允许复制操作
    Array_data(const Array_data&);
    Array_data& operator=(const Array_data&);

    T* data;
    unsigned int sz;
    int use;
};

template<typename T>
class Array { /* 句柄 */
    friend class Pointer<T>;
public:
    Array(unsigned int size) : data(new Array_data<T>(size)) { }
    ~Array() {
        if(--data->use == 0) delete data; /* 只有没有Pointer引用Array_data的元素时，Array_data才会被释放 */
    }
    const T& operator[](unsigned int n) const {
        return (*data)[n];
    }
    T& operator[](unsigned int n) {
        return (*data)[n];
    }
private:
    Array(const Array&);
    Array& operator=(const Array&);
    Array_data<T> *data;
};

template<typename T>
class Pointer {
public:
    Pointer(Array<T> &a, unsigned int n = 0) : ap(a.data), sub(n) { }
    Pointer() : ap(nullptr), sub(0) { }
    Pointer(const Pointer<T>& p) : ap(p.ap), sub(p.sub) { if(ap) ++ap->use; }
    ~Pointer() { if(ap &&--ap->use == 0) delete ap; }
    Pointer& operator=(const Pointer<T>& p) {
        if(p.ap) ++p.ap->use;
        if(ap && --ap->use == 0) delete ap;
        ap = p.ap;
        sub = p.sub;
        return *this;
    }
    T& operator*() const {
        if(ap == nullptr)
            throw "* of unbound";
        return (*ap)[sub];
    }
    Pointer& operator++() {
        ++sub;
        return *this;
    }
    Pointer& operator--() {
        --sub;
        return *this;
    }
    Pointer operator++(int) {
        Pointer ret = *this;
        ++sub;
        return ret;
    }
    Pointer operator--(int) {
        Pointer ret = *this;
        --sub;
        return ret;
    }
    Pointer& operator+=(int n) {
        sub += n;
        return *this;
    }
    Pointer& operator-=(int n) {
        sub -= n;
        return *this;
    }
private:
    Array_data<T> *ap;
    unsigned int sub;
};
```

现在，我们可以放心的写下下面的代码了
```
int main(int argc, char *argv[]) {
    Array<int>* ap = new Array<int>(10);
    Pointer<int> p(*ap, 5);
    delete ap;
    *p = 42;
    return 0;
}
```
当我们删除ap时，由于p还存在，因此Array_data对象的引用计数非零，因此对p的解引用操作仍然有效

##### **指向const Array的Pointer**

这个作为思考题，大家考虑一下怎么实现吧

##### **其它的操作**
1. 上一章我们提到了动态增长，大家可以实现自己的resize版本
2. resize时必然伴随着Array的复制，因此我们还应该实现对Array的复制和赋值操作
3. 我们虽然可以使用指针来访问元素并可以使用诸如++,--的操作，但是无法使用Pointer遍历Array，因此还是应该提供一种遍历Array的机制
