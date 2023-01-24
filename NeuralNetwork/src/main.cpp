//
// Created by George Ford on 1/20/23.
//
#include <vector>
#include <string>
#include <iostream>

#include "Layer.hpp"

using namespace std;

struct Obj {
    int num1 {0};
    int num2 {1};
    char* s {nullptr};
    explicit Obj(int num) : num1(num), num2(num+1) {
        char append[100];
        strcpy(append, "string[");
        strncat(append, ::to_string(num1).c_str(), 100 - 7);
        strcat(append, "]");
        s = (char*) malloc(strlen(append) + 1);
        s = strcpy(s, append);
        puts("Reg CTOR...");
    }
    Obj(const Obj& other) : num1(other.num1), num2(other.num2){
        s = (char*) malloc(strlen(other.s) + 1);
        puts("COPY CTOR...");
    }
    Obj(Obj&& other) : num1(std::move(other.num1)), num2(std::move(other.num2)) {
        s = other.s;
        other.s = nullptr;
        puts("MOVE CTOR...");
    }
    ~Obj() {
        puts("In DTOR...");
        if(s != nullptr && strlen(s))
            free(s);
    }
};
ostream& operator<<(ostream &out, const Obj& obj) {
    out << obj.num1 << ", " << obj.num2 <<", " << obj.s ;
    return out;
}

int main() {
    vector<Obj> objs;
    for (int i = 0; i < 2; ++i) {
        objs.push_back(Obj(i));
    }
}