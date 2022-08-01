#pragma once

#include "jot_ast_visitor.hpp"
#include "jot_token.hpp"
#include "jot_type.hpp"

#include <any>
#include <memory>
#include <vector>

class Statement {
  public:
    virtual std::any accept(StatementVisitor *visitor) = 0;
};

class Expression {
  public:
    virtual std::shared_ptr<JotType> get_type_node() = 0;
    virtual std::any accept(ExpressionVisitor *visitor) = 0;
};

class CompilationUnit {
  public:
    CompilationUnit(std::vector<std::shared_ptr<Statement>> tree_nodes) : tree_nodes(tree_nodes) {}

    std::vector<std::shared_ptr<Statement>> get_tree_nodes() { return tree_nodes; }

  private:
    std::vector<std::shared_ptr<Statement>> tree_nodes;
};

class BlockStatement : public Statement {
  public:
    BlockStatement(std::vector<std::shared_ptr<Statement>> nodes) : nodes(nodes) {}

    std::vector<std::shared_ptr<Statement>> get_nodes() { return nodes; }

    std::any accept(StatementVisitor *visitor) override { return visitor->visit(this); }

  private:
    std::vector<std::shared_ptr<Statement>> nodes;
};

class Parameter {
  public:
    Parameter(Token name, std::shared_ptr<JotType> type) : name(name), type(type) {}

    Token get_name() { return name; }

    std::shared_ptr<JotType> get_type() { return type; }

  private:
    Token name;
    std::shared_ptr<JotType> type;
};

class FieldDeclaration : public Statement {
  public:
    FieldDeclaration(Token name, std::shared_ptr<JotType> type, std::shared_ptr<Expression> value)
        : name(name), type(type), value(value) {}

    Token get_name() { return name; }

    void set_type(std::shared_ptr<JotType> resolved_type) { type = resolved_type; }

    std::shared_ptr<JotType> get_type() { return type; }

    std::shared_ptr<Expression> get_value() { return value; }

    std::any accept(StatementVisitor *visitor) override { return visitor->visit(this); }

  private:
    Token name;
    std::shared_ptr<JotType> type;
    std::shared_ptr<Expression> value;
};

class ExternalPrototype : public Statement {
  public:
    ExternalPrototype(Token token, std::shared_ptr<FunctionPrototype> prototype)
        : extern_token(token), prototype(prototype) {}

    Token get_external_token() { return extern_token; }

    std::shared_ptr<FunctionPrototype> get_prototype() { return prototype; }

    std::any accept(StatementVisitor *visitor) override { return visitor->visit(this); }

  private:
    Token extern_token;
    std::shared_ptr<FunctionPrototype> prototype;
};

class FunctionPrototype : public Statement {
  public:
    FunctionPrototype(Token name, std::vector<std::shared_ptr<Parameter>> parameters,
                      std::shared_ptr<JotType> return_type)
        : name(name), parameters(parameters), return_type(return_type) {}

    Token get_name() { return name; }

    std::vector<std::shared_ptr<Parameter>> get_parameters() { return parameters; }

    std::shared_ptr<JotType> get_return_type() { return return_type; }

    std::any accept(StatementVisitor *visitor) override { return visitor->visit(this); }

  private:
    Token name;
    std::vector<std::shared_ptr<Parameter>> parameters;
    std::shared_ptr<JotType> return_type;
};

class FunctionDeclaration : public Statement {
  public:
    FunctionDeclaration(std::shared_ptr<FunctionPrototype> prototype,
                        std::shared_ptr<Statement> body)
        : prototype(prototype), body(body) {}

    std::shared_ptr<FunctionPrototype> get_prototype() { return prototype; }

    std::shared_ptr<Statement> get_body() { return body; }

    std::any accept(StatementVisitor *visitor) override { return visitor->visit(this); }

  private:
    std::shared_ptr<FunctionPrototype> prototype;
    std::shared_ptr<Statement> body;
};

class EnumDeclaration : public Statement {
  public:
    EnumDeclaration(Token name, std::vector<Token> values) : name(name), values(values) {}

    Token get_name() { return name; }

    std::vector<Token> get_values() { return values; }

    std::any accept(StatementVisitor *visitor) override { return visitor->visit(this); }

  private:
    Token name;
    std::vector<Token> values;
};

class WhileStatement : public Statement {
  public:
    WhileStatement(Token position, std::shared_ptr<Expression> condition,
                   std::shared_ptr<Statement> body)
        : position(position), condition(condition), body(body) {}

    Token get_position() { return position; }

    std::shared_ptr<Expression> get_condition() { return condition; }

    std::shared_ptr<Statement> get_body() { return body; }

    std::any accept(StatementVisitor *visitor) override { return visitor->visit(this); }

  private:
    Token position;
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> body;
};

class ReturnStatement : public Statement {
  public:
    ReturnStatement(Token position, std::shared_ptr<Expression> value)
        : position(position), value(value) {}

    Token get_position() { return position; }

    std::shared_ptr<Expression> return_value() { return value; }

    std::any accept(StatementVisitor *visitor) override { return visitor->visit(this); }

  private:
    Token position;
    std::shared_ptr<Expression> value;
};

class ExpressionStatement : public Statement {
  public:
    ExpressionStatement(std::shared_ptr<Expression> expression) : expression(expression) {}

    std::shared_ptr<Expression> get_expression() { return expression; }

    std::any accept(StatementVisitor *visitor) override { return visitor->visit(this); }

  private:
    std::shared_ptr<Expression> expression;
};

class IfExpression : public Expression {
  public:
    IfExpression(Token if_token, Token else_token, std::shared_ptr<Expression> condition,
                 std::shared_ptr<Expression> if_expression,
                 std::shared_ptr<Expression> else_expression)
        : if_token(if_token), else_token(else_token), condition(condition),
          if_expression(if_expression), else_expression(else_expression) {}

    Token get_if_position() { return if_token; }

    Token get_else_position() { return else_token; }

    std::shared_ptr<Expression> get_condition() { return condition; }

    std::shared_ptr<Expression> get_if_value() { return if_expression; }

    std::shared_ptr<Expression> get_else_value() { return else_expression; }

    std::shared_ptr<JotType> get_type_node() override { return if_expression->get_type_node(); }

    std::any accept(ExpressionVisitor *visitor) override { return visitor->visit(this); }

  private:
    Token if_token;
    Token else_token;
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Expression> if_expression;
    std::shared_ptr<Expression> else_expression;
};

class GroupExpression : public Expression {
  public:
    GroupExpression(Token position, std::shared_ptr<Expression> expression)
        : position(position), expression(expression) {}

    Token get_position() { return position; }

    std::shared_ptr<Expression> get_expression() { return expression; }

    std::shared_ptr<JotType> get_type_node() override { return expression->get_type_node(); }

    std::any accept(ExpressionVisitor *visitor) override { return visitor->visit(this); }

  private:
    Token position;
    std::shared_ptr<Expression> expression;
};

class AssignExpression : public Expression {
  public:
    AssignExpression(std::shared_ptr<Expression> left, Token token,
                     std::shared_ptr<Expression> right)
        : left(left), operator_token(token), right(right) {}

    Token get_operator_token() { return operator_token; }

    std::shared_ptr<Expression> get_right() { return right; }

    std::shared_ptr<Expression> get_left() { return left; }

    std::shared_ptr<JotType> get_type_node() override { return right->get_type_node(); }

    std::any accept(ExpressionVisitor *visitor) override { return visitor->visit(this); }

  private:
    std::shared_ptr<Expression> left;
    Token operator_token;
    std::shared_ptr<Expression> right;
};

class BinaryExpression : public Expression {
  public:
    BinaryExpression(std::shared_ptr<Expression> left, Token token,
                     std::shared_ptr<Expression> right)
        : left(left), operator_token(token), right(right) {}

    Token get_operator_token() { return operator_token; }

    std::shared_ptr<Expression> get_right() { return right; }

    std::shared_ptr<Expression> get_left() { return left; }

    std::shared_ptr<JotType> get_type_node() override { return right->get_type_node(); }

    std::any accept(ExpressionVisitor *visitor) override { return visitor->visit(this); }

  private:
    std::shared_ptr<Expression> left;
    Token operator_token;
    std::shared_ptr<Expression> right;
};

class ComparisonExpression : public Expression {
  public:
    ComparisonExpression(std::shared_ptr<Expression> left, Token token,
                         std::shared_ptr<Expression> right)
        : left(left), operator_token(token), right(right) {}

    Token get_operator_token() { return operator_token; }

    std::shared_ptr<Expression> get_right() { return right; }

    std::shared_ptr<Expression> get_left() { return left; }

    std::shared_ptr<JotType> get_type_node() override {
        return std::make_shared<JotNumber>(operator_token, NumberKind::Integer1);
    }

    std::any accept(ExpressionVisitor *visitor) override { return visitor->visit(this); }

  private:
    std::shared_ptr<Expression> left;
    Token operator_token;
    std::shared_ptr<Expression> right;
};

class LogicalExpression : public Expression {
  public:
    LogicalExpression(std::shared_ptr<Expression> left, Token token,
                      std::shared_ptr<Expression> right)
        : left(left), operator_token(token), right(right) {}

    Token get_operator_token() { return operator_token; }

    std::shared_ptr<Expression> get_right() { return right; }

    std::shared_ptr<Expression> get_left() { return left; }

    std::shared_ptr<JotType> get_type_node() override {
        return std::make_shared<JotNumber>(operator_token, NumberKind::Integer1);
    }

    std::any accept(ExpressionVisitor *visitor) override { return visitor->visit(this); }

  private:
    std::shared_ptr<Expression> left;
    Token operator_token;
    std::shared_ptr<Expression> right;
};

class UnaryExpression : public Expression {
  public:
    UnaryExpression(Token token, std::shared_ptr<Expression> right)
        : operator_token(token), right(right) {}

    Token get_operator_token() { return operator_token; }

    std::shared_ptr<Expression> get_right() { return right; }

    std::shared_ptr<JotType> get_type_node() override { return right->get_type_node(); }

    std::any accept(ExpressionVisitor *visitor) override { return visitor->visit(this); }

  private:
    Token operator_token;
    std::shared_ptr<Expression> right;
};

class CallExpression : public Expression {
  public:
    CallExpression(Token position, std::shared_ptr<Expression> callee,
                   std::vector<std::shared_ptr<Expression>> arguments)
        : position(position), callee(callee), arguments(arguments) {}

    Token get_position() { return position; }

    std::shared_ptr<Expression> get_callee() { return callee; }

    std::vector<std::shared_ptr<Expression>> get_arguments() { return arguments; }

    std::shared_ptr<JotType> get_type_node() override { return callee->get_type_node(); }

    std::any accept(ExpressionVisitor *visitor) override { return visitor->visit(this); }

  private:
    Token position;
    std::shared_ptr<Expression> callee;
    std::vector<std::shared_ptr<Expression>> arguments;
};

class StringExpression : public Expression {
  public:
    StringExpression(Token value) : value(value) {
        auto element_type = std::make_shared<JotNumber>(value, NumberKind::Integer8);
        auto literal = value.get_literal().size();
        type = std::make_shared<JotArrayType>(element_type, literal);
    }

    Token get_value() { return value; }

    std::shared_ptr<JotType> get_type_node() override { return type; }

    std::any accept(ExpressionVisitor *visitor) override { return visitor->visit(this); }

  private:
    Token value;
    std::shared_ptr<JotType> type;
};

class LiteralExpression : public Expression {
  public:
    LiteralExpression(Token name, std::shared_ptr<JotType> type) : name(name), type(type) {}

    Token get_name() { return name; }

    void set_type(std::shared_ptr<JotType> resolved_type) { type = resolved_type; }

    std::shared_ptr<JotType> get_type_node() override { return type; }

    std::any accept(ExpressionVisitor *visitor) override { return visitor->visit(this); }

  private:
    Token name;
    std::shared_ptr<JotType> type;
};

class NumberExpression : public Expression {
  public:
    NumberExpression(Token value)
        : value(value),
          type(std::make_shared<JotNumber>(value, value.get_kind() == TokenKind::Integer
                                                      ? NumberKind::Integer64
                                                      : NumberKind::Float64)) {}

    Token get_value() { return value; }

    std::shared_ptr<JotType> get_type_node() override { return type; }

    std::any accept(ExpressionVisitor *visitor) override { return visitor->visit(this); }

  private:
    Token value;
    std::shared_ptr<JotType> type;
};

class CharacterExpression : public Expression {
  public:
    CharacterExpression(Token value)
        : value(value), type(std::make_shared<JotNumber>(value, NumberKind::Integer8)) {}

    Token get_value() { return value; }

    std::shared_ptr<JotType> get_type_node() override { return type; }

    std::any accept(ExpressionVisitor *visitor) override { return visitor->visit(this); }

  private:
    Token value;
    std::shared_ptr<JotType> type;
};

class BooleanExpression : public Expression {
  public:
    BooleanExpression(Token value)
        : value(value), type(std::make_shared<JotNumber>(value, NumberKind::Integer1)) {}

    Token get_value() { return value; }

    std::shared_ptr<JotType> get_type_node() override { return type; }

    std::any accept(ExpressionVisitor *visitor) override { return visitor->visit(this); }

  private:
    Token value;
    std::shared_ptr<JotType> type;
};

class NullExpression : public Expression {
  public:
    NullExpression(Token value) : value(value), type(std::make_shared<JotNull>(value)) {}

    Token get_value() { return value; }

    std::shared_ptr<JotType> get_type_node() override { return type; }

    std::any accept(ExpressionVisitor *visitor) override { return visitor->visit(this); }

  private:
    Token value;
    std::shared_ptr<JotType> type;
};
