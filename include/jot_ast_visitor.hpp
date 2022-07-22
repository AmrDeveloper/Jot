#pragma once

#include <any>
#include <memory>

class BlockStatement;
class FieldDeclaration;
class FunctionPrototype;
class FunctionDeclaration;
class WhileStatement;
class ReturnStatement;
class ExpressionStatement;

class StatementVisitor {
  public:
    virtual std::any visit(BlockStatement *node) = 0;

    virtual std::any visit(FieldDeclaration *node) = 0;

    virtual std::any visit(FunctionPrototype *node) = 0;

    virtual std::any visit(FunctionDeclaration *node) = 0;

    virtual std::any visit(WhileStatement *node) = 0;

    virtual std::any visit(ReturnStatement *node) = 0;

    virtual std::any visit(ExpressionStatement *node) = 0;
};

class GroupExpression;
class AssignExpression;
class BinaryExpression;
class UnaryExpression;
class CallExpression;
class LiteralExpression;
class NumberExpression;
class CharacterExpression;
class BooleanExpression;
class NullExpression;

class ExpressionVisitor {
  public:
    virtual std::any visit(GroupExpression *node) = 0;

    virtual std::any visit(AssignExpression *node) = 0;

    virtual std::any visit(BinaryExpression *node) = 0;

    virtual std::any visit(UnaryExpression *node) = 0;

    virtual std::any visit(CallExpression *node) = 0;

    virtual std::any visit(LiteralExpression *node) = 0;

    virtual std::any visit(NumberExpression *node) = 0;

    virtual std::any visit(CharacterExpression *node) = 0;

    virtual std::any visit(BooleanExpression *node) = 0;

    virtual std::any visit(NullExpression *node) = 0;
};

class TreeVisitor : public StatementVisitor, public ExpressionVisitor {};