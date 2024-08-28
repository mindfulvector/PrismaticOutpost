#include "script.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>

#include <QStringLiteral>

void Class::addMethod(const std::string& name, std::shared_ptr<Expression> method) {
    methods[name] = method;
}

std::string Class::toString() const {
    return "<class>";
}

std::shared_ptr<Expression> Class::evaluate(std::shared_ptr<Environment>) {
    return std::make_shared<Class>(*this);
}

std::shared_ptr<Expression> Class::getMethod(const std::string& name) const {
    auto it = methods.find(name);
    if (it != methods.end()) {
        return it->second;
    }
    std::wstring wzName = std::wstring(name.begin(), name.end());
    std::wstring wzMessage  = QStringLiteral("Method not found: `%1`").arg(wzName));
    MessageBox(NULL, wzMessage .c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
    throw std::runtime_error("Method not found: " + name);
}

void Environment::define(const std::string& name, std::shared_ptr<Expression> value) {
    bindings[name] = value;
}

std::shared_ptr<Expression> Environment::lookup(const std::string& name) const {
    auto it = bindings.find(name);
    if (it != bindings.end()) {
        return it->second;
    }
    if (parent) {
        return parent->lookup(name);
    }
    std::wstring wzName = std::wstring(name.begin(), name.end());
    std::wstring wzMessage  = format(L"Undefined symbol:", wzName);
    MessageBox(NULL, wzMessage .c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
    throw std::runtime_error("Undefined symbol: " + name);
}


// Implementation of Symbol::evaluate
std::shared_ptr<Expression> Symbol::evaluate(std::shared_ptr<Environment> env) {
    return env->lookup(name);
}

// Implementation of List::evaluate
std::shared_ptr<Expression> List::evaluate(std::shared_ptr<Environment> env) {
    if (elements.empty()) {
        std::wstring wzMessage  = L"Cannot evaluate empty list";
        MessageBox(NULL, wzMessage .c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
        throw std::runtime_error("Cannot evaluate empty list");
    }

    auto first = elements[0]->evaluate(env);
    if (auto symbol = std::dynamic_pointer_cast<Symbol>(elements[0])) {
        if (symbol->getName() == "define") {
            if (elements.size() != 3) {
                std::wstring wzMessage  = L"Incorrect number of arguments for define";
                MessageBox(NULL, wzMessage.c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
                throw std::runtime_error("Incorrect number of arguments for define");
            }
            auto name = std::dynamic_pointer_cast<Symbol>(elements[1]);
            if (!name) {
                std::wstring wzMessage  = L"First argument to define must be a symbol";
                MessageBox(NULL, wzMessage.c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
                throw std::runtime_error("First argument to define must be a symbol");
            }
            auto value = elements[2]->evaluate(env);
            env->define(name->getName(), value);
            return value;
        }
        else if (symbol->getName() == "lambda") {
            if (elements.size() != 3) {
                std::wstring wzMessage  = L"Incorrect number of arguments for lambda";
                MessageBox(NULL, wzMessage.c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
                throw std::runtime_error("Incorrect number of arguments for lambda");
            }
            auto params = std::dynamic_pointer_cast<List>(elements[1]);
            if (!params) {
                std::wstring wzMessage  = L"Second argument to lambda must be a list of parameters";
                MessageBox(NULL, wzMessage.c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
                throw std::runtime_error("Second argument to lambda must be a list of parameters");
            }
            std::vector<std::string> paramNames;
            for (const auto& param : params->getElements()) {
                auto paramSymbol = std::dynamic_pointer_cast<Symbol>(param);
                if (!paramSymbol) {
                    std::wstring wzMessage  = L"Lambda parameters must be symbols";
                    MessageBox(NULL, wzMessage.c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
                    throw std::runtime_error("Lambda parameters must be symbols");
                }
                paramNames.push_back(paramSymbol->getName());
            }
            return std::make_shared<Function>(paramNames, elements[2], env);
        }
        else if (symbol->getName() == "class") {
            if (elements.size() < 2) {
                std::wstring wzMessage  = L"Incorrect number of arguments for class";
                MessageBox(NULL, wzMessage.c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
                throw std::runtime_error("Incorrect number of arguments for class");
            }
            auto cls = std::make_shared<Class>();
            for (size_t i = 1; i < elements.size(); i += 2) {
                auto methodName = std::dynamic_pointer_cast<Symbol>(elements[i]);
                if (!methodName || i + 1 >= elements.size()) {
                    std::wstring wzMessage  = L"Invalid class definition";
                    MessageBox(NULL, wzMessage.c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
                    throw std::runtime_error("Invalid class definition");
                }
                auto methodBody = elements[i + 1]->evaluate(env);
                cls->addMethod(methodName->getName(), methodBody);
            }
            return cls;
        }
        else if (symbol->getName() == "new") {
            if (elements.size() != 2) {
                std::wstring wzMessage  = L"Incorrect number of arguments for new";
                MessageBox(NULL, wzMessage.c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
                throw std::runtime_error("Incorrect number of arguments for new");
            }
            auto cls = std::dynamic_pointer_cast<Class>(elements[1]->evaluate(env));
            if (!cls) {
                std::wstring wzMessage  = L"First argument to new must be a class";
                MessageBox(NULL, wzMessage.c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
                throw std::runtime_error("First argument to new must be a class");
            }
            return std::make_shared<Instance>(cls);
        }
    }

    std::vector<std::shared_ptr<Expression>> evaluatedArgs;
    for (size_t i = 1; i < elements.size(); ++i) {
        evaluatedArgs.push_back(elements[i]->evaluate(env));
    }

    if (auto function = std::dynamic_pointer_cast<Function>(first)) {
        return function->apply(evaluatedArgs);
    }
    else if (auto instance = std::dynamic_pointer_cast<Instance>(first)) {
        if (evaluatedArgs.empty()) {
            std::wstring wzMessage  = L"Method name must be provided when calling instance method";
            MessageBox(NULL, wzMessage.c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
            throw std::runtime_error("Method name must be provided when calling instance method");
        }
        auto methodName = std::dynamic_pointer_cast<Symbol>(elements[1]);
        if (!methodName) {
            std::wstring wzMessage  = L"Method name must be a symbol";
            MessageBox(NULL, wzMessage.c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
            throw std::runtime_error("Method name must be a symbol");
        }
        auto method = instance->getAttribute(methodName->getName());
        if (auto methodFunction = std::dynamic_pointer_cast<Function>(method)) {
            std::vector<std::shared_ptr<Expression>> methodArgs = { instance };
            methodArgs.insert(methodArgs.end(), evaluatedArgs.begin() + 1, evaluatedArgs.end());
            return methodFunction->apply(methodArgs);
        }
        std::wstring wzMessage  = L"Invalid method call";
        MessageBox(NULL, wzMessage.c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
        throw std::runtime_error("Invalid method call");
    }
    std::wstring wzMessage  = L"Invalid function call";
    MessageBox(NULL, wzMessage.c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
    throw std::runtime_error("Invalid function call");
}

// Implementation of Function::apply
std::shared_ptr<Expression> Function::apply(const std::vector<std::shared_ptr<Expression>>& args) {
    if (args.size() != parameters.size()) {
        std::wstring wzMessage  = L"Incorrect number of arguments";
        MessageBox(NULL, wzMessage.c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
        throw std::runtime_error("Incorrect number of arguments");
    }
    auto env = std::make_shared<Environment>(closure);
    for (size_t i = 0; i < parameters.size(); ++i) {
        env->define(parameters[i], args[i]);
    }
    return body->evaluate(env);
}

// Tokenize input string
std::vector<std::string> Script::tokenize(const std::string& str) {
    std::vector<std::string> tokens;
    std::string token;
    bool inString = false;
    for (char c : str) {
        if (c == '"') {
            inString = !inString;
            token += c;
        }
        else if (inString) {
            token += c;
        }
        else if (c == '(' || c == ')') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            tokens.push_back(std::string(1, c));
        }
        else if (std::isspace(c)) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        }
        else {
            token += c;
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}

// Parse tokens into an expression
std::shared_ptr<Expression> Script::parse(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator end) {
    if (it == end) {
        std::wstring wzMessage  = L"Unexpected end of input";
        MessageBox(NULL, wzMessage.c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
        throw std::runtime_error("Unexpected end of input");
    }

    std::string token = *it++;
    if (token == "(") {
        std::vector<std::shared_ptr<Expression>> elements;
        while (it != end && *it != ")") {
            elements.push_back(parse(it, end));
        }
        if (it == end) {
            std::wstring wzMessage  = L"Mismatched parentheses";
            MessageBox(NULL, wzMessage.c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
            throw std::runtime_error("Mismatched parentheses");
        }
        ++it; // consume the ')'
        return std::make_shared<List>(elements);
    }
    else if (token == ")") {
        std::wstring wzMessage  = L"Unexpected ')'";
        MessageBox(NULL, wzMessage.c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
        throw std::runtime_error("Unexpected ')'");
    }
    else if (token[0] == '"' && token[token.length() - 1] == '"') {
        // Handle string literals
        return std::make_shared<Symbol>(token.substr(1, token.length() - 2));
    }
    else {
        // Try to parse as number, otherwise treat as symbol
        try {
            return std::make_shared<Number>(std::stod(token));
        }
        catch (const std::invalid_argument&) {
            return std::make_shared<Symbol>(token);
        }
    }
}

// Read-Eval-Print Loop
void Script::repl() {
    auto globalEnv = std::make_shared<Environment>();

    // Define built-in functions
    globalEnv->define("+", std::make_shared<Function>(
                               std::vector<std::string>{"a", "b"},
                               std::make_shared<List>(std::vector<std::shared_ptr<Expression>>{
                                   std::make_shared<Symbol>("+"),
                                   std::make_shared<Symbol>("a"),
                                   std::make_shared<Symbol>("b")
                               }),
                               globalEnv
                               ));

    while (true) {
        std::cout << "> ";
        std::string input;
        std::getline(std::cin, input);
        if (input == "exit") break;

        try {
            auto tokens = tokenize(input);
            auto it = tokens.begin();
            auto expr = parse(it, tokens.end());
            auto result = expr->evaluate(globalEnv);
            std::cout << result->toString() << std::endl;
        }
        catch (const std::exception& e) {
            //std::wstring wzMessage  = format(L"Error: {}", e.what());
            //MessageBox(NULL, wzMessage .c_str(), L"Script Error", MB_OK | MB_ICONEXCLAMATION);
            std::cout << "Error: " << e.what() << std::endl;
        }
    }
}

/*
int main() {
    std::cout << "Enhanced Scheme Interpreter" << std::endl;
    std::cout << "Type 'exit' to quit" << std::endl;
    repl();
    return 0;
}
*/


std::wstring ScriptEscapeString(const std::wstring& str) {
    std::wstringstream ss;
    for (wchar_t c : str) {
        switch (c) {
        case L'"': ss << L"\\\""; break;
        case L'\\': ss << L"\\\\"; break;
        case L'\n': ss << L"\\n"; break;
        case L'\r': ss << L"\\r"; break;
        case L'\t': ss << L"\\t"; break;
        default: ss << c;
        }
    }
    return ss.str();
}

