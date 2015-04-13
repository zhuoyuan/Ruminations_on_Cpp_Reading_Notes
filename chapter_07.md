
### 句柄：第二部分

第六章通过使用句柄和引用计数，使得程序能够通过控制引用计数高效的“复制”对象。但是第六章用到的技术有一个明显的缺点：为了把句柄绑定到类T的对象上，必须定义一个具有类型T的成员的新类，当要绑定这样的句柄到一个继承自T的（静态）未知类的对象时，就行不通了。

*原因很简单，我们定义了UPoint *up而不是Point *p，使我们失去了将一个继承自Point的对象绑定到Handle的能力，因此我们必须将引用技术分离出来。*

##### **分离引用计数**

最简单的方法就是使用指针：
```
class Handle {
public:
    //...
private:
    Point* p;
    int* u;
};

Handle::~Handle() {
    if(--*u == 0) {
        delete p;
        delete u;
    }
}
```
我们看到，当需要删除对象时，总是要同时删除引用计数，使得引用计数和Handle耦合在一起，当需求变化时，我们不得不重写Handle类，解决方法还是用类表示概念，实现一个UserCount类，将引用计数抽象化

##### **抽象引用计数**

为了避免日后反复的修改，我们应当假设UseCount类对Handle类一无所知，按照设计类的经验，先从构造函数族开始：

```
class UseCount {
public:
    UseCount();
    UseCount(const UseCount&);
    UseCount& operator=(const UseCount&);
    ~UseCount();
private:
    int *p;
};
```
默认的，引用计数通常从1开始：
```
UseCount::UseCount() : p(new int(1)) { }
```
类似的，从一个UseCount构造另一个应该使两者指向相同的指针，同时递增引用计数：
```
UseCount::UseCount(const UseCount& u) : p(u.p) { ++*p; }
```
析够函数也是显然的：
```
UseCount::~UseCount() { if(--*u == 0) delete p; }
```

下面需要考虑UseCount应该对Handle提供那些接口，我们从Handle的角度考虑一下需要那些操作：
```
class Handle {
public:
    //...
private:
    Point *p;
    UseCount u;
}
```
###### **Handle的构造函数**
我们已经定义了UseCount构造函数，而Handle的构造函数依赖于缺省UseCount的构造函数的行为，因此不再赘述

###### **Handle的析够函数**
析够函数必须知道引用计数是否为0，因此UseCount类需要向Handle类提供一个接口以告知Handle类是否到达释放
对象的时机。
```
bool UseCount::only() { return *p == 1; }
Handle::~Handle() { if(u.only()) delete p;}
```
###### **Handle的赋值操作符**
也很简单，提供一个判断接口，同时负责自身处理
```
bool UseCount::reattach(const UseCount& u) {
    ++*u.p;
    if(--*p == 0) {
        delete p;
        p = u.p;
        return ture;
    }
    p = u.p;
    return false;
}
Handle& operator=(const Handle& h) {
    if(u.reattach(h.u))
        delete p;
    p = h.p;
    return *this;
}
```
##### **存取函数和写时复制**

根据上一章的分析，必须知道当前Handle是否独占Point对象，因此需要UseCount提供一个判断接口
```
bool UseCount::makeonly() {
    if(*p == 1)
        return false;
    --*p;
    p = new int(1);
    return true;
}
Handle& Handle::x(int xv) {
    if(u.makeonly())
        p = new Point(*p);/* 同样存在不能正确复制对象的问题*/
    p->x(xv);
    return *his;
}
```
##### **总结**

1. 通过分离引用计数，可以是我们解决绑定到派生类对象的问题
2. 通过抽象化引用计数，降低了Handle类与引用计数的耦合度，使得扩展变得简单
