#include "script.h"
#include <QDebug>
#include <QTextStream>
#include <QCoreApplication>
#include <QtMath>

Number::Number(double val) : value(val) {}

QString List::toString() const {
    QString result = "(";
    for (int i = 0; i < elements.size(); ++i) {
        if (i > 0) result += " ";
        result += elements[i]->toString();
    }
    return result + ")";
}

void Class::addMethod(const QString& name, QSharedPointer<Expression> method) {
    methods[name] = method;
}

QString Class::toString() const {
    return "<class>";
}

QSharedPointer<Expression> Class::evaluate(QSharedPointer<Environment>) {
    return QSharedPointer<Class>::create(*this);
}

QSharedPointer<Expression> Class::getMethod(const QString& name) const {
    auto it = methods.find(name);
    if (it != methods.end()) {
        return it.value();
    }
    qCritical() << "Method not found:" << name;
    throw std::runtime_error(QString("Method not found: %1").arg(name).toStdString());
}

QSharedPointer<Expression> Instance::getAttribute(const QString& name) const {
    auto it = attributes.find(name);
    if (it != attributes.end()) {
        return it.value();
    }
    return cls->getMethod(name);
}

void Environment::define(const QString& name, QSharedPointer<Expression> value) {
    bindings[name] = value;
}

QSharedPointer<Expression> Environment::lookup(const QString& name) const {
    auto it = bindings.find(name);
    if (it != bindings.end()) {
        return it.value();
    }
    if (parent) {
        return parent->lookup(name);
    }
    qCritical() << "Undefined symbol:" << name;
    throw std::runtime_error(QString("Undefined symbol: %1").arg(name).toStdString());
}

QSharedPointer<Expression> Symbol::evaluate(QSharedPointer<Environment> env) {
    return env->lookup(name);
}

QSharedPointer<Expression> List::evaluate(QSharedPointer<Environment> env) {
    if (elements.isEmpty()) {
        qCritical() << "Cannot evaluate empty list";
        throw std::runtime_error("Cannot evaluate empty list");
    }

    auto first = elements[0]->evaluate(env);
    if (auto symbol = qSharedPointerDynamicCast<Symbol>(elements[0])) {
        if (symbol->getName() == "define") {
            if (elements.size() != 3) {
                qCritical() << "Incorrect number of arguments for define";
                throw std::runtime_error("Incorrect number of arguments for define");
            }
            auto name = qSharedPointerDynamicCast<Symbol>(elements[1]);
            if (!name) {
                qCritical() << "First argument to define must be a symbol";
                throw std::runtime_error("First argument to define must be a symbol");
            }
            auto value = elements[2]->evaluate(env);
            env->define(name->getName(), value);
            return value;
        }
        else if (symbol->getName() == "lambda") {
            if (elements.size() != 3) {
                qCritical() << "Incorrect number of arguments for lambda";
                throw std::runtime_error("Incorrect number of arguments for lambda");
            }
            auto params = qSharedPointerDynamicCast<List>(elements[1]);
            if (!params) {
                qCritical() << "Second argument to lambda must be a list of parameters";
                throw std::runtime_error("Second argument to lambda must be a list of parameters");
            }
            QVector<QString> paramNames;
            for (const auto& param : params->getElements()) {
                auto paramSymbol = qSharedPointerDynamicCast<Symbol>(param);
                if (!paramSymbol) {
                    qCritical() << "Lambda parameters must be symbols";
                    throw std::runtime_error("Lambda parameters must be symbols");
                }
                paramNames.append(paramSymbol->getName());
            }
            return QSharedPointer<Function>::create(paramNames, elements[2], env);
        }
        else if (symbol->getName() == "class") {
            if (elements.size() < 2) {
                qCritical() << "Incorrect number of arguments for class";
                throw std::runtime_error("Incorrect number of arguments for class");
            }
            auto cls = QSharedPointer<Class>::create();
            for (int i = 1; i < elements.size(); i += 2) {
                auto methodName = qSharedPointerDynamicCast<Symbol>(elements[i]);
                if (!methodName || i + 1 >= elements.size()) {
                    qCritical() << "Invalid class definition";
                    throw std::runtime_error("Invalid class definition");
                }
                auto methodBody = elements[i + 1]->evaluate(env);
                cls->addMethod(methodName->getName(), methodBody);
            }
            return cls;
        }
        else if (symbol->getName() == "new") {
            if (elements.size() != 2) {
                qCritical() << "Incorrect number of arguments for new";
                throw std::runtime_error("Incorrect number of arguments for new");
            }
            auto cls = qSharedPointerDynamicCast<Class>(elements[1]->evaluate(env));
            if (!cls) {
                qCritical() << "First argument to new must be a class";
                throw std::runtime_error("First argument to new must be a class");
            }
            return QSharedPointer<Instance>::create(cls);
        }
    }

    QVector<QSharedPointer<Expression>> evaluatedArgs;
    for (int i = 1; i < elements.size(); ++i) {
        evaluatedArgs.append(elements[i]->evaluate(env));
    }

    if (auto function = qSharedPointerDynamicCast<Function>(first)) {
        return function->apply(evaluatedArgs);
    }
    else if (auto instance = qSharedPointerDynamicCast<Instance>(first)) {
        if (evaluatedArgs.isEmpty()) {
            qCritical() << "Method name must be provided when calling instance method";
            throw std::runtime_error("Method name must be provided when calling instance method");
        }
        auto methodName = qSharedPointerDynamicCast<Symbol>(elements[1]);
        if (!methodName) {
            qCritical() << "Method name must be a symbol";
            throw std::runtime_error("Method name must be a symbol");
        }
        auto method = instance->getAttribute(methodName->getName());
        if (auto methodFunction = qSharedPointerDynamicCast<Function>(method)) {
            QVector<QSharedPointer<Expression>> methodArgs = { instance };
            methodArgs.append(evaluatedArgs.mid(1));
            return methodFunction->apply(methodArgs);
        }
        qCritical() << "Invalid method call";
        throw std::runtime_error("Invalid method call");
    }
    qCritical() << "Invalid function call";
    throw std::runtime_error("Invalid function call");
}

QSharedPointer<Expression> Function::apply(const QVector<QSharedPointer<Expression>>& args) {
    if (args.size() != parameters.size()) {
        qCritical() << "Incorrect number of arguments";
        throw std::runtime_error("Incorrect number of arguments");
    }
    auto env = QSharedPointer<Environment>::create(closure);
    for (int i = 0; i < parameters.size(); ++i) {
        env->define(parameters[i], args[i]);
    }
    return body->evaluate(env);
}

Script::Script(QObject *parent) : QObject(parent) {}

QVector<QString> Script::tokenize(const QString& str) {
    QVector<QString> tokens;
    QString token;
    bool inString = false;
    for (QChar c : str) {
        if (c == '"') {
            inString = !inString;
            token += c;
        }
        else if (inString) {
            token += c;
        }
        else if (c == '(' || c == ')') {
            if (!token.isEmpty()) {
                tokens.append(token);
                token.clear();
            }
            tokens.append(QString(c));
        }
        else if (c.isSpace()) {
            if (!token.isEmpty()) {
                tokens.append(token);
                token.clear();
            }
        }
        else {
            token += c;
        }
    }
    if (!token.isEmpty()) {
        tokens.append(token);
    }
    return tokens;
}

QSharedPointer<Expression> Script::parse(QVector<QString>::iterator& it, QVector<QString>::iterator end) {
    if (it == end) {
        qCritical() << "Unexpected end of input";
        throw std::runtime_error("Unexpected end of input");
    }

    QString token = *it++;
    if (token == "(") {
        QVector<QSharedPointer<Expression>> elements;
        while (it != end && *it != ")") {
            elements.append(parse(it, end));
        }
        if (it == end) {
            qCritical() << "Mismatched parentheses";
            throw std::runtime_error("Mismatched parentheses");
        }
        ++it; // consume the ')'
        return QSharedPointer<List>::create(elements);
    }
    else if (token == ")") {
        qCritical() << "Unexpected ')'";
        throw std::runtime_error("Unexpected ')'");
    }
    else if (token.startsWith('"') && token.endsWith('"')) {
        // Handle string literals
        return QSharedPointer<Symbol>::create(token.mid(1, token.length() - 2));
    }
    else {
        // Try to parse as number, otherwise treat as symbol
        bool ok;
        double value = token.toDouble(&ok);
        if (ok) {
            return QSharedPointer<Number>::create(value);
        }
        else {
            return QSharedPointer<Symbol>::create(token);
        }
    }
}

void Script::repl() {
    auto globalEnv = QSharedPointer<Environment>::create();

    // Define built-in functions
    globalEnv->define("+", QSharedPointer<Function>::create(
                               QVector<QString>{"a", "b"},
                               QSharedPointer<List>::create(QVector<QSharedPointer<Expression>>{
                                   QSharedPointer<Symbol>::create("+"),
                                   QSharedPointer<Symbol>::create("a"),
                                   QSharedPointer<Symbol>::create("b")
                               }),
                               globalEnv
                               ));

    QTextStream qin(stdin);
    QTextStream qout(stdout);

    while (true) {
        qout << "> " << Qt::flush;
        QString input = qin.readLine();
        if (input == "exit") break;

        try {
            auto tokens = tokenize(input);
            auto it = tokens.begin();
            auto expr = parse(it, tokens.end());
            auto result = expr->evaluate(globalEnv);
            qout << result->toString() << Qt::endl;
        }
        catch (const std::exception& e) {
            qCritical() << "Error:" << e.what();
        }
    }
}

QString Script::scriptEscapeString(const QString& str) {
    QString result;
    for (QChar c : str) {
        switch (c.unicode()) {
        case '"': result += "\\\""; break;
        case '\\': result += "\\\\"; break;
        case '\n': result += "\\n"; break;
        case '\r': result += "\\r"; break;
        case '\t': result += "\\t"; break;
        default: result += c;
        }
    }
    return result;
}

/*
// Example usage
int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    qInfo() << "Enhanced Scheme Interpreter";
    qInfo() << "Type 'exit' to quit";

    Script script;
    script.repl();

    return 0;
}
// */
