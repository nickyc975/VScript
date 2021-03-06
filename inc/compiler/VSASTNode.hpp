#ifndef VS_ASTNODE_H
#define VS_ASTNODE_H

#include <vector>

#include "compiler/VSTokenizer.hpp"
#include "objects/VSObject.hpp"

typedef enum {
    AST_CONST,
    AST_IDENT,
    AST_TUPLE_DECL,
    AST_LIST_DECL,
    AST_DICT_DECL,
    AST_SET_DECL,
    AST_LAMBDA_DECL,
    AST_IDX_EXPR,
    AST_DOT_EXPR,
    AST_FUNC_CALL,
    AST_B_OP_EXPR,
    AST_U_OP_EXPR,
    AST_ASSIGN_EXPR,
    AST_PAIR_EXPR,
    AST_EXPR_LST,
    AST_INIT_DECL,
    AST_INIT_DECL_LIST,
    AST_FUNC_DECL,
    AST_CLASS_DECL,
    AST_METH_DECL,
    AST_ELIF_LIST,
    AST_IF_STMT,
    AST_WHILE_STMT,
    AST_FOR_STMT,
    AST_CONTINUE,
    AST_BREAK,
    AST_RETURN,
    AST_CPD_STMT,
    AST_PROGRAM
} AST_NODE_TYPE;

class VSASTNode : public VSObject {
public:
    AST_NODE_TYPE node_type;
};

class IdentNode : public VSASTNode {
public:
    VSObject *name;

    IdentNode(VSObject *name) : name(name) {
        this->node_type = AST_IDENT;
        INCREF(name);
    }
    ~IdentNode() {
        DECREF_EX(this->name);
    }
};

class ConstNode : public VSASTNode {
public:
    VSObject *value;

    ConstNode(VSObject *value) : value(value) {
        this->node_type = AST_CONST;
        INCREF(value);
    }

    ~ConstNode() {
        DECREF_EX(this->value);
    }
};

class ContainerNode : public VSASTNode {
public:
    std::vector<VSASTNode *> values;

    ContainerNode() {
        this->values = std::vector<VSASTNode *>();
    }

    ~ContainerNode() {
        for (auto value : this->values) {
            DECREF(value);
        }
    }

    void append(VSASTNode *value) {
        this->values.push_back(value);
        INCREF(value);
    }
};

class TupleDeclNode : public ContainerNode {
public:
    TupleDeclNode() {
        this->node_type = AST_TUPLE_DECL;
    }
    ~TupleDeclNode() = default;
};

class ListDeclNode : public ContainerNode {
public:
    ListDeclNode() {
        this->node_type = AST_LIST_DECL;
    }
    ~ListDeclNode() = default;
};

class DictDeclNode : public ContainerNode {
public:
    DictDeclNode() {
        this->node_type = AST_DICT_DECL;
    }
    ~DictDeclNode() = default;
};

class SetDeclNode : public ContainerNode {
public:
    SetDeclNode() {
        this->node_type = AST_SET_DECL;
    }
    ~SetDeclNode() = default;
};

class IdxExprNode : public VSASTNode {
public:
    VSASTNode *obj;
    VSASTNode *index;

    IdxExprNode(VSASTNode *obj, VSASTNode *index) : obj(obj), index(index) {
        this->node_type = AST_IDX_EXPR;
        INCREF(obj);
        INCREF(index);
    }
    ~IdxExprNode() {
        DECREF_EX(this->obj);
        DECREF_EX(this->index);
    }
};

class DotExprNode : public VSASTNode {
public:
    VSASTNode *obj;
    IdentNode *attrname;

    DotExprNode(VSASTNode *obj, IdentNode *attrname) : obj(obj), attrname(attrname) {
        this->node_type = AST_DOT_EXPR;
        INCREF(obj);
        INCREF(attrname);
    }
    ~DotExprNode() {
        DECREF_EX(this->obj);
        DECREF_EX(this->attrname);
    }
};

class FuncCallNode : public VSASTNode {
public:
    VSASTNode *func;
    VSASTNode *args;

    FuncCallNode(VSASTNode *func, VSASTNode *args) : func(func), args(args) {
        this->node_type = AST_FUNC_CALL;
        INCREF(func);
        INCREF(args);
    }
    ~FuncCallNode() {
        DECREF_EX(this->func);
        DECREF_EX(this->args);
    }
};

class BOPNode : public VSASTNode {
public:
    TOKEN_TYPE opcode;
    VSASTNode *l_operand;
    VSASTNode *r_operand;

    BOPNode(TOKEN_TYPE opcode, VSASTNode *l_operand, VSASTNode *r_operand) : opcode(opcode), l_operand(l_operand), r_operand(r_operand) {
        this->node_type = AST_B_OP_EXPR;
        INCREF(l_operand);
        INCREF(r_operand);
    }
    ~BOPNode() {
        DECREF_EX(this->l_operand);
        DECREF_EX(this->r_operand);
    }
};

class UOPNode : public VSASTNode {
public:
    TOKEN_TYPE opcode;
    VSASTNode *operand;

    UOPNode(TOKEN_TYPE opcode, VSASTNode *operand) : opcode(opcode), operand(operand) {
        this->node_type = AST_U_OP_EXPR;
        INCREF(operand);
    }
    ~UOPNode() {
        DECREF_EX(this->operand);
    }
};

class AssignExprNode : public VSASTNode {
public:
    TOKEN_TYPE opcode;
    VSASTNode *lval;
    VSASTNode *rval;

    AssignExprNode(TOKEN_TYPE opcode, VSASTNode *lval, VSASTNode *rval) : opcode(opcode), lval(lval), rval(rval) {
        this->node_type = AST_ASSIGN_EXPR;
        INCREF(lval);
        INCREF(rval);
    }
    ~AssignExprNode() {
        DECREF_EX(this->lval);
        DECREF_EX(this->rval);
    }
};

class PairExprNode : public VSASTNode {
public:
    VSASTNode *key;
    VSASTNode *value;

    PairExprNode(VSASTNode *key, VSASTNode *value) : key(key), value(value) {
        this->node_type = AST_PAIR_EXPR;
        INCREF(key);
        INCREF(value);
    }
    ~PairExprNode() {
        DECREF_EX(this->key);
        DECREF_EX(this->value);
    }
};

class ExprListNode : public ContainerNode {
public:
    ExprListNode() {
        this->node_type = AST_EXPR_LST;
    }
    ~ExprListNode() = default;
};

class InitDeclNode : public VSASTNode {
public:
    IdentNode *name;
    VSASTNode *init_val;

    InitDeclNode(IdentNode *name, VSASTNode *init_val) : name(name), init_val(init_val) {
        this->node_type = AST_INIT_DECL;
        INCREF(name);
        INCREF(init_val);
    }
    ~InitDeclNode() {
        DECREF_EX(this->name);
        DECREF_EX(this->init_val);
    }
};

class InitDeclListNode : public VSASTNode {
public:
    TOKEN_TYPE specifier;
    std::vector<InitDeclNode *> decls;

    InitDeclListNode(TOKEN_TYPE specifier) {
        this->node_type = AST_INIT_DECL_LIST;
        this->specifier = specifier;
        this->decls = std::vector<InitDeclNode *>();
    }
    ~InitDeclListNode() {
        for (auto decl : this->decls) {
            DECREF(decl);
        }
    }

    void add_decl(InitDeclNode *decl) {
        this->decls.push_back(decl);
        INCREF(decl);
    }
};

class FuncDeclNode : public VSASTNode {
public:
    bool va_args;
    IdentNode *name;
    std::vector<VSASTNode *> args;
    VSASTNode *body;

    FuncDeclNode(IdentNode *name) : name(name) {
        this->va_args = false;
        this->node_type = name != NULL ? AST_FUNC_DECL : AST_LAMBDA_DECL;
        this->args = std::vector<VSASTNode *>();
        this->body = NULL;
        INCREF(name);
    }
    ~FuncDeclNode() {
        DECREF_EX(this->name);
        DECREF_EX(this->body);
        for (auto arg : this->args) {
            DECREF(arg);
        }
    }

    void add_arg(VSASTNode *arg) {
        this->args.push_back(arg);
        INCREF(arg);
    }

    void set_body(VSASTNode *body) {
        DECREF_EX(this->body);
        this->body = body;
        INCREF(this->body);
    }
};

class IfStmtNode : public VSASTNode {
public:
    VSASTNode *cond;
    VSASTNode *truestmt;
    VSASTNode *falsestmt;

    IfStmtNode(VSASTNode *cond, VSASTNode *truestmt, VSASTNode *falsestmt) : cond(cond), truestmt(truestmt), falsestmt(falsestmt) {
        this->node_type = AST_IF_STMT;
        INCREF(cond);
        INCREF(truestmt);
        INCREF(falsestmt);
    }
    ~IfStmtNode() {
        DECREF_EX(cond);
        DECREF_EX(truestmt);
        DECREF_EX(falsestmt);
    }
};

class ElifListNode : public VSASTNode {
public:
    std::vector<IfStmtNode *> elifs;
    VSASTNode *elsestmt;

    ElifListNode() {
        this->node_type = AST_ELIF_LIST;
        this->elifs = std::vector<IfStmtNode *>();
        this->elsestmt = NULL;
    }

    ~ElifListNode() {
        for (auto elif : this->elifs) {
            DECREF(elif);
        }
        DECREF_EX(this->elsestmt);
    }

    void add_elif(IfStmtNode *elif) {
        this->elifs.push_back(elif);
        INCREF(elif);
    }

    void set_else(VSASTNode *elsenode) {
        DECREF_EX(this->elsestmt);
        this->elsestmt = elsenode;
        INCREF(this->elsestmt);
    }
};

class WhileStmtNode : public VSASTNode {
public:
    VSASTNode *cond;
    VSASTNode *body;

    WhileStmtNode(VSASTNode *cond, VSASTNode *body) : cond(cond), body(body) {
        this->node_type = AST_WHILE_STMT;
        INCREF(cond);
        INCREF(body);
    }
    ~WhileStmtNode() {
        DECREF_EX(this->cond);
        DECREF_EX(this->body);
    }
};

class ForStmtNode : public VSASTNode {
public:
    VSASTNode *init;
    VSASTNode *cond;
    VSASTNode *incr;
    VSASTNode *body;

    ForStmtNode(VSASTNode *init, VSASTNode *cond, VSASTNode *incr, VSASTNode *body) : init(init), cond(cond), incr(incr), body(body) {
        this->node_type = AST_FOR_STMT;
        INCREF(init);
        INCREF(cond);
        INCREF(incr);
        INCREF(body);
    }
    ~ForStmtNode() {
        DECREF_EX(this->init);
        DECREF_EX(this->cond);
        DECREF_EX(this->incr);
        DECREF_EX(this->body);
    }
};

class ContinueStmtNode : public VSASTNode {
public:
    ContinueStmtNode() {
        this->node_type = AST_CONTINUE;
    }
    ~ContinueStmtNode() = default;
};

class BreakStmtNode : public VSASTNode {
public:
    BreakStmtNode() {
        this->node_type = AST_BREAK;
    }
    ~BreakStmtNode() = default;
};

class ReturnStmtNode : public VSASTNode {
public:
    VSASTNode *retval;

    ReturnStmtNode(VSASTNode *retval) : retval(retval) {
        this->node_type = AST_RETURN;
        INCREF(retval);
    }
    ~ReturnStmtNode() {
        DECREF_EX(this->retval);
    }
};

class CpdStmtNode : public ContainerNode {
public:
    CpdStmtNode() {
        this->node_type = AST_CPD_STMT;
    }
    ~CpdStmtNode() = default;
};

class ProgramNode : public ContainerNode {
public:
    ProgramNode() {
        this->node_type = AST_PROGRAM;
    }
    ~ProgramNode() = default;
};

#endif