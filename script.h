#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>
#include <vector>
#include <memory>
#include <map>

class Environment;

// Base class for all expression types
class Expression {
public:
    virtual ~Expression() = default;
    virtual std::string toString() const = 0;
    virtual std::shared_ptr<Expression> evaluate(std::shared_ptr<Environment> env) = 0;
};

// Number expression
class Number : public Expression {
    double value;
public:
    Number(double val);
    std::string toString() const override {
        return std::to_string(value);
    }
    std::shared_ptr<Expression> evaluate(std::shared_ptr<Environment>) override {
        return std::make_shared<Number>(*this);
    }
    double getValue() const { return value; }

};

// Symbol expression
class Symbol : public Expression {
    std::string name;
public:
    Symbol(const std::string& n) : name(n) {}
    std::string toString() const override {
        return name;
    }
    std::shared_ptr<Expression> evaluate(std::shared_ptr<Environment> env) override;
    const std::string& getName() const { return name; }
};

// List expression
class List : public Expression {
    std::vector<std::shared_ptr<Expression>> elements;
public:
    List(const std::vector<std::shared_ptr<Expression>>& elems) : elements(elems) {}
    std::string toString() const override {
        std::string result = "(";
        for (size_t i = 0; i < elements.size(); ++i) {
            if (i > 0) result += " ";
            result += elements[i]->toString();
        }
        return result + ")";
    }
    std::shared_ptr<Expression> evaluate(std::shared_ptr<Environment> env) override;
    const std::vector<std::shared_ptr<Expression>>& getElements() const { return elements; }
};

// Function expression
class Function : public Expression {
    std::vector<std::string> parameters;
    std::shared_ptr<Expression> body;
    std::shared_ptr<Environment> closure;
public:
    Function(const std::vector<std::string>& params, std::shared_ptr<Expression> bod, std::shared_ptr<Environment> env)
        : parameters(params), body(bod), closure(env) {}
    std::string toString() const override {
        return "<function>";
    }
    std::shared_ptr<Expression> evaluate(std::shared_ptr<Environment>) override {
        return std::make_shared<Function>(*this);
    }
    std::shared_ptr<Expression> apply(const std::vector<std::shared_ptr<Expression>>& args);
};

// Class expression
class Class : public Expression {
    std::map<std::string, std::shared_ptr<Expression>> methods;
public:
    void addMethod(const std::string& name, std::shared_ptr<Expression> method);
    std::string toString() const override;
    std::shared_ptr<Expression> evaluate(std::shared_ptr<Environment> env) override;
    std::shared_ptr<Expression> getMethod(const std::string& name) const;
};

// Instance expression
class Instance : public Expression {
    std::shared_ptr<Class> cls;
    std::map<std::string, std::shared_ptr<Expression>> attributes;
public:
    Instance(std::shared_ptr<Class> c) : cls(c) {}
    std::string toString() const override {
        return "<instance>";
    }
    std::shared_ptr<Expression> evaluate(std::shared_ptr<Environment>) override {
        return std::make_shared<Instance>(*this);
    }
    void setAttribute(const std::string& name, std::shared_ptr<Expression> value) {
        attributes[name] = value;
    }
    std::shared_ptr<Expression> getAttribute(const std::string& name) const {
        auto it = attributes.find(name);
        if (it != attributes.end()) {
            return it->second;
        }
        return cls->getMethod(name);
    }
};

// Environment to store variable bindings
class Environment {
    std::map<std::string, std::shared_ptr<Expression>> bindings;
    std::shared_ptr<Environment> parent;
public:
    Environment(std::shared_ptr<Environment> p = nullptr) : parent(p) {}
    void define(const std::string& name, std::shared_ptr<Expression> value);
    std::shared_ptr<Expression> lookup(const std::string& name) const;
};

// Script manager
class Script {
    std::vector<std::string> tokenize(const std::string& str);
    std::shared_ptr<Expression> parse(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator end);
    void repl();
    std::wstring ScriptEscapeString(const std::wstring& str);
};

#endif // SCRIPT_H
