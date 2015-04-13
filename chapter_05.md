
###代理类

##### **问题：如何设计一种容器，使它可以包含类型不同而彼此相关的对象呢？**

假设有一个表示不同类型交通工具的类派生层次：

```
class Vehicle {
public:
    virtual double weitht() const = 0;
    virtual void start() = 0;
    //...
};

class RoadVehicle : public Vehicle { /*...*/ }
class AutoVehicle : pubcli RoadVehicle { /*...*/ }
class Aircraft : public Vehicle { /*...*/ }
class Helicopter : public Aircraft { /*...*/ } 
```

这些类有一些共同的特性，但是有的Vehicle具有一些其它Vechile没有的特性，现在我们想要跟踪一系列不同种类的Vechile，一个直观的想法是用数组来实现，于是我们写下：

```
Vehicle parking_lot[1000];
```
稍作思考我们会立即否定这种方案，Vehicle是一个抽象类，不能实例化。但是，当我们深入思考一下，即使我们将基类中的所有纯虚函数删掉，我们的目的也无法达到，当我们试图将一个派生类对象赋给基类对象时，编译器实际上做了一个类型转换，丢弃了所有在基类中没有的成员.

##### **一种经典的解决方案**

这种情况下，我们可能需要增加一个间接层，最直接的间接层形式就是存储指针，于是我们可以写下：
```
Vechicle *parking_lot[1000];
AutoVehicle x = /*... */;
Parking_lot[i] = &x;
```
问题好像解决了，但是深入思考一下(不要烦，C++love那些喜欢思考的程序员)，会发现问题：当我们总是用静态对象给数组赋值时，问题不大，但是当我们用类似
```
parking_lot[i] = new AutoVehicle();
```
的语句时，就要小心了，因为这时需要我们自己管理内存，如果你接着写下下面的代码：
```
if(p != q) {
    delete parking_lot[p];
    parking_lot[p] = parking_lot[q];
}
```
出现什么情况，parking_lot[q]会被释放两次，造成内存异常，聪明的你也许会说，我可以这样写：
```
if(p != q) {
    delete parking_lot[p];
    parking_lot[p] = new Vehicle(parking_lot[q]);
}
```
问题回到了刚开始数组的问题，Vehicle是抽象类，无法实例化这样的对象，而且，即使有，也不是我们想要的！！！原因上一章说过，对象的创建和复制不是运行时多态的，因此我们创建的实际上是一个基类对象。

##### **接着想办法**
上面的解决方案已经给了我们提示，中间层能够工作，只是直接用指针不够完美，我们不能正确复制编译是类型未知的对象。C++是怎么处理类型未知的对象呢？虚函数。因此，我们可以定义一个序函数：
```
class Vechicle {
public:
    //...
    virtual Vechicle* copy() const = 0;
    virtual ~Vechicle() { } /* don't forget */
};

我们的派生类实现了这个函数后，就可能实现正确复制对象了

Vechicle* AutoVehicle::copy() const { return new AutoVehicle(*this); }

上面的程序可以改为：
if(p != q) {
    delete parking_lot[p];
    parking_lot[p] = parking_lot[q]->copy();
}
```
至此，我们终于实现了当初的目的

##### **我们的方法是不是够好**
上面的代码可以工作，但是我们不得不显示分配和释放内存，这对与程序员来说是个极大的挑战。那么有没有一种方法既能使我们避免显示的处理内存分配，又能保持类Vechicle运行时绑定的属性呢？

解决这个问题的关键是用类来表示概念,简单点说，增加中间层，让中间层解决一切问题！！！

接下来我们定义一个称为代理类的类，这个类行为和Vechicle类相似，能够潜在表示所有继承自Vechicle类的对象的东西，这种增加中间层的设计原则在以后的章节中屡屡出现。

```
class VehicleSurrogate {
public:
    VehicleSurrogate();
    VehicleSurrogate(const Vechicle&);
    ～VehicleSurrogate();
    VehicleSurrogate(const VehicleSurrogate&);
    VehicleSurrogate& operator=(const VehicleSurrogate&);\
private:
    Vehicle *vp;
};
```

使用缺省构造函数是我们可以定义对象数组，但是我们必须解决一个问题，如果代理类所代理的基类是一个抽象类，我们如何规定缺省操作呢？一个经验的做法是让基类指针为空。

接下来的工作就很简单了，我们只要小心翼翼的实现这个类就可以了

一旦我们实现了这个代理类，我们就可以轻松的写下下面的代码了：
```
VehicleSurrogate parking_lot[1000];
AutoVehicle x;
parking_lot[i] = x;
```
最后一条语句等价于
```
parking_lot[i] = VehicleSurrogate(x);
```
这个语句实际上创建了一个x的副本，并将VehicleSurrogate对象绑定到这个副本上，然后将这个对象赋给parking_lot的一个元素，当最后销毁parking_lot数组时，所有这些副本都将被清除

#### **总结**
将继承和容器共用时，需要解决两个问题：控制内存分配和把不同类型的对象放入同一个容器中。采用C++技术，用类表示概念，可以同时兼顾这两个问题。在实现过程中，我们提出代理类，这个类的每个对象都代表另外一个对象，该对象可以是位于一个完整继承层次中的任何类的对象，通过在容器中使用代理对象而不是对象本身的方式，解决了我们的问题。

关键词：用类表示概念(中间类)，内存分配，容器，继承

[SourceCode](./Code/chapter_05_Surrogate.cpp)
