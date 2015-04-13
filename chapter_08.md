
### 一个面向对象程序范例

*面向对象编程有3要素：数据抽象，继承和动态绑定，这一章通过一个小例子来展示这三个要素*

##### **问题描述**

实现打印表达式树的Expr类：
```
#include <iostream.h>

int main(int argc, char *argv[]) {
    Expr t = Expr("*", Expr("-", 5), Expr(*, 3, 4));
    cout << t << endl;
    t = Expr("*", t, t);
    cout << t << endl;
    return 0;
}
result:
((-5) * (3 + 4))
(((-5) * (3 + 4)) * ((-5) * (3 + 4)))
```

##### **关键**
```
1. 定义抽象基类Expr_node
2. 定义派生类Int_node, Unary_node, Binary_node
3. 通过句柄Expr来管理内存分配
```

思考：如果增加一个需求，要求能够打印表达式的值呢？

[SourceCode](./Code/chapter_08_Expr.cpp)
