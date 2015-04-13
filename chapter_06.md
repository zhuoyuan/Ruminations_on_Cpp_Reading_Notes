
### 句柄：第一部分

先回忆一下第五章介绍的代理类，这个类能让我们在一个容器中存储类型不同但相互关联的对象。代理类有个特点，每次创建或者赋值都要复制所代理的对象，这样做的好处是控制简单，每个对象之间相互隔离，互不影响。但是，如果想避免这些复制该怎么做呢，这一章将引入另一个通常叫做句柄的类，它允许在保持代理的多态行为的同时，还可以避免进行不必要的复制。

##### **问题**

为什么要避免复制？
```
1. 有些对象很大，复制起来消耗太大
2. 有些对象根本无法复制
3. 在多态的环境下，我们无法确定对象本身的类型
4. 对于函数返回的情况，引用或指针很难奏效
```

我们能想到最简单的避免复制的方法通常是使用指针，但是当有几个指针指向同一个对象时，就必须考虑什么时候才能释放这个对象，如果删除的太早，会有某个指针仍然指向这个对象，使用这个指针就会引发未定以的行为；如果删除太晚，当丢失最后一个指向这个对象的指针时，就无法释放这个对象了。

现在让我们总结一下我们要解决的问题：
```
1. 保持多态行为
2. 避免复制
3. 安全性
```
C++的解决方法是定义一个适当的类，并将这个类绑定到它所控制的对象上，通常称这种类为句柄(Handle)类，由于这种类的行为类似指针，有时也被称为智能指针(smart pointer)，但也仅仅是类似而已。

##### **Handle类的行为**

在讨论Handle之前，要有一个类，这个类不用太复杂，但要能说明问题
```
class Point {
public:
    Point() : xval(0), yval(0) { }
    Point(int x, int y) : xval(x), yval(y) { }
    int x() const { return xval; }
    int y() const { reutrn yval; }
    Point& x(int xv) { xval = xv; return *this; }
    Point& y(int yv) { yval = yv; return *this; }
private:
    int xval;
    int yval;
};
```

*问题1：从Point类初始化Handle应该完成什么任务？*
```
Point p;
Handle h(p);
```
h(p)应该表达什么含义，是直接将h绑定到p，还是另外创建一个point对象并将h绑定要新对象呢？稍作考虑，直接绑定不会起作用，问题在于p直接处于用户的控制之下，一旦用户删掉p，h应该无效，但是通知所有的h是不可能的，我们已经知道了问题所在，因此，h必须能够控制它所绑定的对象，换言之，h应该创建和释放对象。

*问题2：如何从Handle类获取Point对象*
我们说过Handle类在行为上类似指针，意味着我们似乎应该使用operaor->将Handle的所有操作转发给相应的Point来执行：
```
class Hanle {
public:
    Point* operator->();
    //...
};
```
这样做看起来是可行的，但有两个明显的不足：
```
1. 我们失去了禁止和改写一些操作的能力
2. 暴露了内存分配的策路，使得用户可以轻松得到底层对象的地址
```
因此，必须避免使用operator->，同时明确选择让Handle类支持那些操作


##### **实现**

以上已经讨论了足够多的东西，下面可以开始实现了：
```
class Hanle {
public:
    Handle();
    Handle(int, int);
    Handle(const Point&);
    Handle(const Handle&);
    Handle& operator=(const Handle&);
    ~Handle();

    int x() const;
    int y() const;
    Handle& x(int);
    Handle& y(int);
private:
    //...
};
```
使用句柄的重要原因是避免复制，也就是说，允许多个句柄绑定到同一个对象上，因此，我们必须知道有多少个句柄绑定到了对象上，负责就没法正确的释放对象，解决这个问题，通常使用引用计数。

引用计数可以是Handle的一部分吗？如果是，那么每个句柄必须知道和它绑定到同一个对象上的其它句柄的位置，这样才能更新其它句柄的引用计数；引用计数可以是对象的一部分吗？如果是，我们不得不重写已经存在的类。C++的原则是用类来表示概念，我们可以定义一个类包含对象和引用计数：
```
class UPoint {
    //all private
    friend class Handle;
    Point p;
    int u;
    Upoint() : u(1) { }
    Upoint(int x, int y) : p(x, y), u(1) { }
    Upoint(const Point& pp) : p(pp), u(1) { }
};
```
解决了避免复制的问题，下面我们继续完善Handle：
```
public:
    Handle();
    Handle(int, int);
    Handle(const Point&);
    Handle(const Handle&);
    Handle& operator=(const Handle&);
    ~Handle();

    int x() const;
    int y() const;
    Handle& x(int);
    Handle& y(int);
private:
    Upoint *up;
};
Handle::Handle() : up(new Upoint) { }
Handle::Handle(int x, int y) : up(new Upoint(x, y)) { }
Handle::Handle(const Point& p) : up(new Upoint(p)) { }
Handle::~Handle() { if(--up->u == 0) delete up; }
Handle::Handle(const Handle& h) : up(h.up) { ++up->u; }
Handle& Handle::operator=(const Handle& h) {
    ++h.up->u;
    if(--up->u == 0)
        delete up;
    up = h.up;
    return *this;
}

对于只读函数：
int Handle::x() const { return up->p.x(); }
int Handle::y() const { return up->p.y(); }

对于改动行函数，我们需要作出决定，我们的句柄本身需要值语义还是指针语义！！！
```

##### **写时复制**

我们将Handle设计成“无需对Point对象进行复制”的形式，但还有一个问题，句柄类自身在用户面前需要什么样的行为！
```
Handle h(3, 4);
Handle h2 = h;
h2.x(5);
int n = h.x();
```
如果希望句柄为值语义，那么n应该等于3，否则，n应该等于5，两种方式都可以，关键在于我们自己的选择

如果采用指针语义，永远不用复制UPoint对象：
```
Handle& Handle::x(int xv) { up->p.x(xv); return *this; }
Handle& Handle::y(int yv) { up->p.y(yv); return *this; }
```

如果采用值语义，必须保证改动的那个UPoint对象不能同时被任何其它Handle所引用，由于有引用计数，因此不难判断：
```
Handle& Handle::x(int xv) {
    if(up->u != 1) {
        --up->u;
        up = new UPoint(up->p);
    }
    up->p.x(xv);
    return *this;
}
Handle::y类似
```
这种技术通常被称为Copy on write!

##### **总结**

1. 由于某些原因使得对象的复制很困难，需要一种避免复制的机制（Handle）
2. Handle必须“控制”所绑定的对象
3. 使用引用计数来实现对对象的控制
4. 决定句柄自身的语义：Pointer or Value(copy on write)
