#include <iostream>
#include <string>

using namespace std;

class Expr;

class Expr_Node {
friend class Expr;
friend ostream& operator<<(ostream&, Expr_Node&);
public:
    virtual void print(ostream&) const = 0;
protected:
    Expr_Node() : use(1) { }
    virtual ~Expr_Node() { }
private:
    int use;
};

class Expr {
    friend class Expr_Node;
    friend ostream& operator<<(ostream&, const Expr&);
public:
    Expr() = default;
    Expr(int);
    Expr(const string&, Expr);
    Expr(const string&, Expr, Expr);
    Expr(const Expr&);
    Expr& operator=(const Expr&);
    ~Expr() { if(--p->use == 0 ) delete p; }
private:
    Expr_Node *p;
};

ostream& operator<<(ostream &os, Expr_Node& e) {
    e.print(os);
    return os;
}

class Int_Node : public Expr_Node {
    friend class Expr;
    Int_Node(const int k) : val(k) { }
    void print(ostream& os) const { os << val; }
    int val;
};

class Unary_Node : public Expr_Node {
    friend class Expr;
    Unary_Node(const string& a, Expr b) : op(a), opnd(b) { }
    void print(ostream &os) const { os << "(" << op << opnd << ")"; }
    string op;
    Expr opnd;
};

class Binary_Node : public Expr_Node {
    friend class Expr;
    Binary_Node(const string& a, Expr b, Expr c) : op(a), opnd_left(b), opnd_right(c) { }
    void print(ostream &os) const { os << "(" << opnd_left << " " << op << " " << opnd_right << ")"; }
    string op;
    Expr opnd_left;
    Expr opnd_right;
};

Expr::Expr(int a) : p(new Int_Node(a)) { /*p = new Int_Node(a);*/ }
Expr::Expr(const string& a, Expr b) : p(new Unary_Node(a, b)) { /*p = new Unary_Node(a, b);*/ }
Expr::Expr(const string& a, Expr b, Expr c) : p(new Binary_Node(a, b, c)) { /*p = new Binary_Node(a, b, c);*/ }
Expr::Expr(const Expr& t) : p(t.p) { p->use++; }
Expr& Expr::operator=(const Expr& t) {
    t.p->use++;
    if(--p->use == 0) delete p;
    p = t.p;
    return *this;
}

ostream& operator<<(ostream& os, const Expr& t) {
    t.p->print(os);
    return os;
}

int main(int argc, char *argv[]) {
    Expr t = Expr("*", Expr("-", 5), Expr("+", 3, 4));
    t = Expr("*", t, t);
    cout << t << endl;
    return 0;
}
