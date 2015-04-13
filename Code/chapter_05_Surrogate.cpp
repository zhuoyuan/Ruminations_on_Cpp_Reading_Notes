/*
1. 基类是抽象类
2. 控制内存分配
3. 把不同类型的对象放入同一个容器中

解决方案：代理类
*/
#include <iostream>
#include <exception>

using namespace std;

class Vehicle {
public:
    virtual double weight() const = 0;
    virtual Vehicle* copy() const = 0;
    virtual ~Vehicle() { }
};

class VehicleSurrogate {
public:
    VehicleSurrogate();
    VehicleSurrogate(const Vehicle&);
    VehicleSurrogate(const VehicleSurrogate&);
    VehicleSurrogate& operator=(const VehicleSurrogate&);
    ~VehicleSurrogate();
    /* public interfaces from Vehicle */
    double weight() const;
    
private:
    Vehicle *vp;
};

VehicleSurrogate::VehicleSurrogate() : vp(NULL) { }
VehicleSurrogate::VehicleSurrogate(const Vehicle& v) : vp(v.copy()) { }
VehicleSurrogate::VehicleSurrogate(const VehicleSurrogate& vs) {
    if(vs.vp != NULL)
        vp = vs.vp->copy();
    else
        vp = NULL;
}

/*TODO: Not exception safe*/
VehicleSurrogate& VehicleSurrogate::operator=(const VehicleSurrogate& vs) {
    if(this != &vs) {
        delete vp;
        vp = (vs.vp ? vs.vp->copy() : NULL);
    }
    return *this;
}
VehicleSurrogate::~VehicleSurrogate() { delete vp; }
double VehicleSurrogate::weight() const {
    if(vp) 
        return vp->weight();
    else
        throw "empty VehicleSurrogate.weight!";
}
int main(int argc, char *argv[]) {
    return 0;
}
