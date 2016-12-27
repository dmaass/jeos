#ifndef FLAG_H
#define FLAG_H

class Flag {
public:
    Flag() : value(false) {}
    bool get() { return value; }
    void set() { value = true; }
    void unset() { value = false; }
    void toggle() { value = !value; }
private:
    bool value;
};

#endif // FLAG_H
