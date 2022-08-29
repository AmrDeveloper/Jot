#pragma once

#include "jot_ast.hpp"
#include "jot_ast_visitor.hpp"
#include "jot_symboltable.hpp"
#include "jot_type.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>

#include <any>
#include <map>
#include <memory>
#include <stack>
#include <vector>

class DeferCall {
  public:
    virtual void generate_call() = 0;
};

// LLVM Context, Builder and Current Module
static llvm::LLVMContext llvm_context;
static llvm::IRBuilder<> Builder(llvm_context);
static std::unique_ptr<llvm::Module> llvm_module;
static std::map<std::string, std::shared_ptr<FunctionPrototype>> functions_table;
static std::map<std::string, llvm::Function *> llvm_functions;
static std::vector<std::shared_ptr<DeferCall>> defers_stack;
static std::stack<llvm::BasicBlock *> break_block_stack;

// LLVM Integer types
static auto llvm_int1_type = llvm::Type::getInt1Ty(llvm_context);
static auto llvm_int8_type = llvm::Type::getInt8Ty(llvm_context);
static auto llvm_int16_type = llvm::Type::getInt16Ty(llvm_context);
static auto llvm_int32_type = llvm::Type::getInt32Ty(llvm_context);
static auto llvm_int64_type = llvm::Type::getInt64Ty(llvm_context);

static auto llvm_int64_ptr_type = llvm::Type::getInt64PtrTy(llvm_context);

// LLVM Floating pointer types
static auto llvm_float32_type = llvm::Type::getFloatTy(llvm_context);
static auto llvm_float64_type = llvm::Type::getDoubleTy(llvm_context);

// LLVM Void type
static auto llvm_void_type = llvm::Type::getVoidTy(llvm_context);

// LLVM 1 bit integer with zero value (false)
static auto false_value = Builder.getInt1(false);

// LLVM 32 bit integer with zero value
static auto zero_int32_value = Builder.getInt32(0);

class DeferFunctionCall : public DeferCall {
  public:
    DeferFunctionCall(llvm::Function *function, std::vector<llvm::Value *> arguments)
        : function(function), arguments(arguments) {}
    llvm::Function *function;
    std::vector<llvm::Value *> arguments;

    void generate_call() override { Builder.CreateCall(function, arguments); }
};

class DeferFunctionPtrCall : public DeferCall {
  public:
    DeferFunctionPtrCall(llvm::FunctionType *function_type, llvm::Value *callee,
                         std::vector<llvm::Value *> arguments)
        : function_type(function_type), callee(callee), arguments(arguments) {}
    llvm::FunctionType *function_type;
    llvm::Value *callee;
    std::vector<llvm::Value *> arguments;

    void generate_call() override { Builder.CreateCall(function_type, callee, arguments); }
};

class JotLLVMBackend : public TreeVisitor {
  public:
    JotLLVMBackend() {
        alloca_inst_global_scope = std::make_shared<JotSymbolTable>();
        alloca_inst_scope = alloca_inst_global_scope;
    }

    std::unique_ptr<llvm::Module> compile(std::string module_name,
                                          std::shared_ptr<CompilationUnit> compilation_unit);

    std::any visit(BlockStatement *node) override;

    std::any visit(FieldDeclaration *node) override;

    std::any visit(FunctionPrototype *node) override;

    std::any visit(FunctionDeclaration *node) override;

    std::any visit(EnumDeclaration *node) override;

    std::any visit(IfStatement *node) override;

    std::any visit(WhileStatement *node) override;

    std::any visit(ReturnStatement *node) override;

    std::any visit(DeferStatement *node) override;

    std::any visit(BreakStatement *node) override;

    std::any visit(ExpressionStatement *node) override;

    std::any visit(IfExpression *node) override;

    std::any visit(GroupExpression *node) override;

    std::any visit(AssignExpression *node) override;

    std::any visit(BinaryExpression *node) override;

    std::any visit(ShiftExpression *node) override;

    std::any visit(ComparisonExpression *node) override;

    std::any visit(LogicalExpression *node) override;

    std::any visit(UnaryExpression *node) override;

    std::any visit(CallExpression *node) override;

    std::any visit(CastExpression *node) override;

    std::any visit(IndexExpression *node) override;

    std::any visit(EnumAccessExpression *node) override;

    std::any visit(LiteralExpression *node) override;

    std::any visit(NumberExpression *node) override;

    std::any visit(ArrayExpression *node) override;

    std::any visit(StringExpression *node) override;

    std::any visit(CharacterExpression *node) override;

    std::any visit(BooleanExpression *node) override;

    std::any visit(NullExpression *node) override;

  private:
    bool has_break_statement = false;

    llvm::Value *llvm_node_value(std::any any_value);

    llvm::Value *llvm_resolve_value(std::any any_value);

    llvm::Value *llvm_number_value(std::string value_litearl, NumberKind size);

    llvm::Value *llvm_boolean_value(bool value);

    llvm::Value *llvm_character_value(char character);

    llvm::Type *llvm_type_from_jot_type(std::shared_ptr<JotType> type);

    llvm::AllocaInst *create_entry_block_alloca(llvm::Function *function,
                                                const std::string var_name, llvm::Type *type);

    llvm::Function *lookup_function(std::string name);

    void execute_defer_calls();

    void clear_defer_calls_stack();

    void push_alloca_inst_scope();

    void pop_alloca_inst_scope();

    std::shared_ptr<JotSymbolTable> alloca_inst_global_scope;
    std::shared_ptr<JotSymbolTable> alloca_inst_scope;
};