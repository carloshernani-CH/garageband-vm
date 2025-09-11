#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>

struct SimpleNode {
    std::string type;
    std::string value;
    std::vector<SimpleNode*> children;

    SimpleNode(std::string type, std::string value = "") : type(type), value(value) {}
    
    ~SimpleNode() {
        for (SimpleNode* child : children) {
            delete child;
        }
    }
    
    void addChild(SimpleNode* child) {
        children.push_back(child);
    }
    
    void print(int indent = 0) const {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
        std::cout << type;
        if (!value.empty()) {
            std::cout << ": " << value;
        }
        std::cout << std::endl;
        for (const auto& child : children) {
            child->print(indent + 1);
        }
    }
};

#endif // AST_HPP
