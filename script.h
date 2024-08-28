#ifndef SCRIPT_H
#define SCRIPT_H

#include <QString>
#include <QVector>
#include <QSharedPointer>
#include <QMap>
#include <QObject>

class Environment;

// Base class for all expression types
class Expression {
public:
    virtual ~Expression() = default;
    virtual QString toString() const = 0;
    virtual QSharedPointer<Expression> evaluate(QSharedPointer<Environment> env) = 0;
};

// Number expression
class Number : public Expression {
    double value;
public:
    Number(double val);
    QString toString() const override {
        return QString::number(value);
    }
    QSharedPointer<Expression> evaluate(QSharedPointer<Environment>) override {
        return QSharedPointer<Number>::create(*this);
    }
    double getValue() const { return value; }
};

// Symbol expression
class Symbol : public Expression {
    QString name;
public:
    Symbol(const QString& n) : name(n) {}
    QString toString() const override {
        return name;
    }
    QSharedPointer<Expression> evaluate(QSharedPointer<Environment> env) override;
    const QString& getName() const { return name; }
};

// List expression
class List : public Expression {
    QVector<QSharedPointer<Expression>> elements;
public:
    List(const QVector<QSharedPointer<Expression>>& elems) : elements(elems) {}
    QString toString() const override;
    QSharedPointer<Expression> evaluate(QSharedPointer<Environment> env) override;
    const QVector<QSharedPointer<Expression>>& getElements() const { return elements; }
};

// Function expression
class Function : public Expression {
    QVector<QString> parameters;
    QSharedPointer<Expression> body;
    QSharedPointer<Environment> closure;
public:
    Function(const QVector<QString>& params, QSharedPointer<Expression> bod, QSharedPointer<Environment> env)
        : parameters(params), body(bod), closure(env) {}
    QString toString() const override {
        return "<function>";
    }
    QSharedPointer<Expression> evaluate(QSharedPointer<Environment>) override {
        return QSharedPointer<Function>::create(*this);
    }
    QSharedPointer<Expression> apply(const QVector<QSharedPointer<Expression>>& args);
};

// Class expression
class Class : public Expression {
    QMap<QString, QSharedPointer<Expression>> methods;
public:
    void addMethod(const QString& name, QSharedPointer<Expression> method);
    QString toString() const override;
    QSharedPointer<Expression> evaluate(QSharedPointer<Environment> env) override;
    QSharedPointer<Expression> getMethod(const QString& name) const;
};

// Instance expression
class Instance : public Expression {
    QSharedPointer<Class> cls;
    QMap<QString, QSharedPointer<Expression>> attributes;
public:
    Instance(QSharedPointer<Class> c) : cls(c) {}
    QString toString() const override {
        return "<instance>";
    }
    QSharedPointer<Expression> evaluate(QSharedPointer<Environment>) override {
        return QSharedPointer<Instance>::create(*this);
    }
    void setAttribute(const QString& name, QSharedPointer<Expression> value) {
        attributes[name] = value;
    }
    QSharedPointer<Expression> getAttribute(const QString& name) const;
};

// Environment to store variable bindings
class Environment {
    QMap<QString, QSharedPointer<Expression>> bindings;
    QSharedPointer<Environment> parent;
public:
    Environment(QSharedPointer<Environment> p = nullptr) : parent(p) {}
    void define(const QString& name, QSharedPointer<Expression> value);
    QSharedPointer<Expression> lookup(const QString& name) const;
};

// Script manager
class Script : public QObject {

public:
    explicit Script(QObject *parent = nullptr);

    QVector<QString> tokenize(const QString& str);
    QSharedPointer<Expression> parse(QVector<QString>::iterator& it, QVector<QString>::iterator end);
    void repl();

private:
    QString scriptEscapeString(const QString& str);
};

#endif // SCRIPT_H
