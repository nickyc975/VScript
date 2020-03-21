#include "printers.hpp"
#include "objects/VSListObject.hpp"
#include "objects/VSStringObject.hpp"

static int indent = 0;

// static void fprint_const_node(FILE *file, ASTNode *node);
// static void fprint_ident_node(FILE *file, ASTNode *node);
// static void fprint_b_expr_node(FILE *file, ASTNode *node);
// static void fprint_u_expr_node(FILE *file, ASTNode *node);
// static void fprint_expr_lst_node(FILE *file, ASTNode *node);
// static void fprint_decl_node(FILE *file, ASTNode *node);
// static void fprint_decl_lst_node(FILE *file, ASTNode *node);
// static void fprint_assign_node(FILE *file, ASTNode *node);
// static void fprint_lst_val_node(FILE *file, ASTNode *node);
// static void fprint_lst_idx_node(FILE *file, ASTNode *node);
// static void fprint_func_call_node(FILE *file, ASTNode *node);
// static void fprint_func_decl_node(FILE *file, ASTNode *node);
// static void fprint_if_stmt_node(FILE *file, ASTNode *node);
// static void fprint_elif_lst_node(FILE *file, ASTNode *node);
// static void fprint_while_stmt_node(FILE *file, ASTNode *node);
// static void fprint_for_stmt_node(FILE *file, ASTNode *node);
// static void fprint_continue_node(FILE *file, ASTNode *node);
// static void fprint_break_node(FILE *file, ASTNode *node);
// static void fprint_cpd_stmt_node(FILE *file, ASTNode *node);
// static void fprint_return_node(FILE *file, ASTNode *node);

static void fprint_indent(FILE *file) {
    int i = 0;
    for (; i < indent; i++) {
        fprintf(file, "    ");
    }
}

void init_printer() {
    indent = 0;
}

// static void fprint_const_node(FILE *file, ASTNode *node)
// {
//     fprint_indent(file);
//     fprintf(file, "const: ");
//     switch (node->value->type)
//     {
//     case NONE:
//         fprintf(file, "none\n");
//         break;
//     case BOOL:
//         fprintf(file, "%s\n", node->value->bool_val ? "true" : "false");
//         break;
//     case CHAR:
//         fprintf(file, "%c\n", node->value->char_val);
//         break;
//     case INT:
//         fprintf(file, "%lld\n", node->value->int_val);
//         break;
//     case FLOAT:
//         fprintf(file, "%lf\n", node->value->float_val);
//         break;
//     case STRING:
//         fprintf(file, "%s\n", node->value->str_val->c_str());
//         break;
//     default:
//         fprintf(file, "\n");
//         break;
//     }
// }

// static void fprint_ident_node(FILE *file, ASTNode *node)
// {
//     fprint_indent(file);
//     fprintf(file, "ident: %s\n", node->name->c_str());
// }

// static void fprint_b_expr_node(FILE *file, ASTNode *node)
// {
//     fprint_indent(file);
//     fprintf(file, "b_expr: \n");
//     indent++;
//     fprint_astree(file, node->l_operand);
//     fprint_indent(file);
//     fprintf(file, "b_opcode: %s\n", TOKEN_STR[node->b_opcode]);
//     fprint_astree(file, node->r_operand);
//     indent--;
// }

// static void fprint_u_expr_node(FILE *file, ASTNode *node)
// {
//     fprint_indent(file);
//     fprintf(file, "u_expr: \n");
//     indent++;
//     fprint_indent(file);
//     fprintf(file, "u_opcode: %s\n", TOKEN_STR[node->u_opcode]);
//     fprint_astree(file, node->operand);
//     indent--;
// }

// static void fprint_expr_lst_node(FILE *file, ASTNode *node)
// {
//     fprint_indent(file);
//     fprintf(file, "expr_lst: \n");
//     indent++;
//     for (ASTNode *child : *node->expr_list)
//     {

//         fprint_astree(file, child);
//     }
//     indent--;
// }

// static void fprint_decl_node(FILE *file, ASTNode *node)
// {
//     fprint_indent(file);
//     fprintf(file, "decl: \n");
//     indent++;
//     fprint_astree(file, node->var_name);
//     if (node->init_val)
//     {
//         fprint_astree(file, node->init_val);
//     }
//     indent--;
// }

// static void fprint_decl_lst_node(FILE *file, ASTNode *node)
// {
//     fprint_indent(file);
//     fprintf(file, "decl_lst: \n");
//     indent++;
//     for (ASTNode *child : *node->decl_list)
//     {

//         fprint_astree(file, child);
//     }
//     indent--;
// }

// static void fprint_assign_node(FILE *file, ASTNode *node)
// {
//     fprint_indent(file);
//     fprintf(file, "assign: \n");
//     indent++;
//     fprint_astree(file, node->assign_var);
//     fprint_indent(file);
//     fprintf(file, "assign_opcode: %s\n", TOKEN_STR[node->assign_opcode]);
//     fprint_astree(file, node->assign_val);
//     indent--;
// }

// static void fprint_lst_val_node(FILE *file, ASTNode *node)
// {
//     fprint_indent(file);
//     fprintf(file, "lst_val: \n");
//     indent++;
//     for (ASTNode *child : *node->list_val)
//     {
//         fprint_astree(file, child);
//     }
//     indent--;
// }

// static void fprint_lst_idx_node(FILE *file, ASTNode *node)
// {
//     fprint_indent(file);
//     fprintf(file, "lst_idx: \n");
//     indent++;
//     fprint_indent(file);
//     fprintf(file, "list name: \n");
//     indent++;
//     fprint_astree(file, node->list_name);
//     indent--;
//     fprint_indent(file);
//     fprintf(file, "list index: \n");
//     indent++;
//     fprint_astree(file, node->list_index);
//     indent--;
//     indent--;
// }

// static void fprint_func_call_node(FILE *file, ASTNode *node)
// {
//     fprint_indent(file);
//     fprintf(file, "func_call: \n");
//     indent++;
//     fprint_indent(file);
//     fprintf(file, "func name: \n");
//     indent++;
//     fprint_astree(file, node->func_name);
//     indent--;
//     fprint_indent(file);
//     fprintf(file, "func args: \n");
//     indent++;
//     fprint_astree(file, node->arg_list);
//     indent--;
//     indent--;
// }

// static void fprint_func_decl_node(FILE *file, ASTNode *node)
// {
//     fprint_indent(file);
//     fprintf(file, "func_decl: \n");
//     indent++;
//     fprint_astree(file, node->func_name);
//     for (auto param : *node->func_params)
//     {
//         fprint_astree(file, param);
//     }
//     fprint_astree(file, node->func_body);
//     indent--;
// }

// static void fprint_if_stmt_node(FILE *file, ASTNode *node)
// {
//     fprint_indent(file);
//     fprintf(file, "if_stmt: \n");
//     indent++;
//     fprint_astree(file, node->if_cond);
//     if (node->true_smt != NULL)
//     {
//         fprint_astree(file, node->true_smt);
//     }
//     if (node->false_smt != NULL)
//     {
//         fprint_astree(file, node->false_smt);
//     }
//     indent--;
// }

// static void fprint_elif_lst_node(FILE *file, ASTNode *node)
// {
//     fprint_indent(file);
//     fprintf(file, "elif_lst: \n");
//     indent++;
//     for (ASTNode *child : *node->elif_list)
//     {
//         fprint_astree(file, child);
//         // fprintf(file, "%d\n", child->node_type);
//         // fprintf(file, "%d\n", child->stmts_num);
//     }
//     indent--;
//     if (node->else_node != NULL)
//     {
//         fprint_astree(file, node->else_node);
//     }
// }

// static void fprint_while_stmt_node(FILE *file, ASTNode *node)
// {
//     fprint_indent(file);
//     fprintf(file, "while_stmt: \n");
//     indent++;
//     fprint_astree(file, node->while_cond);
//     fprint_astree(file, node->while_body);
//     indent--;
// }

// static void fprint_for_stmt_node(FILE *file, ASTNode *node)
// {
//     fprint_indent(file);
//     fprintf(file, "for_stmt: \n");
//     indent++;
//     fprint_astree(file, node->for_init);
//     fprint_astree(file, node->for_cond);
//     fprint_astree(file, node->for_incr);
//     fprint_astree(file, node->for_body);
//     indent--;
// }

// static void fprint_continue_node(FILE *file, ASTNode *node)
// {
//     fprint_indent(file);
//     fprintf(file, "continue\n");
// }

// static void fprint_break_node(FILE *file, ASTNode *node)
// {
//     fprint_indent(file);
//     fprintf(file, "break\n");
// }

// static void fprint_cpd_stmt_node(FILE *file, ASTNode *node)
// {
//     fprint_indent(file);
//     fprintf(file, "stmts: \n");
//     indent++;
//     for (ASTNode *child : *node->statements)
//     {
//         fprint_astree(file, child);
//         // fprintf(file, "%d\n", child->node_type);
//         // fprintf(file, "%d\n", child->stmts_num);
//     }
//     indent--;
// }

// static void fprint_return_node(FILE *file, ASTNode *node)
// {
//     fprint_indent(file);
//     fprintf(file, "return: \n");
//     indent++;
//     fprint_astree(file, node->ret_val);
//     indent--;
// }

// void fprint_tokens(FILE *file, std::vector<Token *> tokens)
// {
//     for (Token *token : tokens)
//     {
//         fprintf(file, "%s\t%lld\t%lld\t", TOKEN_STR[token->type], token->ln, token->col);
//         switch (token->type)
//         {
//             case TK_IDENTIFIER:
//                 fprintf(file, "%s\n", token->identifier->c_str());
//                 break;
//             case TK_CONSTANT:
//                 fprintf(file, "%s\t", TYPE_STR[token->value->type]);
//                 switch (token->value->type)
//                 {
//                     case NONE:
//                         fprintf(file, "none\n");
//                         break;
//                     case BOOL:
//                         fprintf(file, "%s\n",token->value->bool_val ? "true" : "false");
//                         break;
//                     case CHAR:
//                         fprintf(file, "%c\n", token->value->char_val);
//                         break;
//                     case INT:
//                         fprintf(file, "%lld\n", token->value->int_val);
//                         break;
//                     case FLOAT:
//                         fprintf(file, "%lf\n", token->value->float_val);
//                         break;
//                     case STRING:
//                         fprintf(file, "%s\n", token->value->str_val->c_str());
//                         break;
//                     default:
//                         break;
//                 }
//                 break;
//             case TK_SEMICOLON:
//             case TK_COMMA:
//             case TK_L_PAREN:
//             case TK_R_PAREN:
//             case TK_L_BRACK:
//             case TK_R_BRACK:
//             case TK_L_CURLY:
//             case TK_R_CURLY:
//             default:
//                 fprintf(file, "\n");
//                 break;
//         }
//     }
// }

// void fprint_astree(FILE *file, ASTNode *astree)
// {
//     switch (astree->type)
//     {
//     case AST_CONST:
//         fprint_const_node(file, astree);
//         break;
//     case AST_IDENT:
//         fprint_ident_node(file, astree);
//         break;
//     case AST_B_EXPR:
//         fprint_b_expr_node(file, astree);
//         break;
//     case AST_U_EXPR:
//         fprint_u_expr_node(file, astree);
//         break;
//     case AST_EXPR_LST:
//         fprint_expr_lst_node(file, astree);
//         break;
//     case AST_INIT_DECL:
//         fprint_decl_node(file, astree);
//         break;
//     case AST_INIT_DECL_LIST:
//         fprint_decl_lst_node(file, astree);
//         break;
//     case AST_ASSIGN_EXPR:
//         fprint_assign_node(file, astree);
//         break;
//     case AST_LIST_VAL:
//         fprint_lst_val_node(file, astree);
//         break;
//     case AST_LIST_IDX:
//         fprint_lst_idx_node(file, astree);
//         break;
//     case AST_FUNC_CALL:
//         fprint_func_call_node(file, astree);
//         break;
//     case AST_FUNC_DECL:
//         fprint_func_decl_node(file, astree);
//         break;
//     case AST_IF_STMT:
//         fprint_if_stmt_node(file, astree);
//         break;
//     case AST_ELIF_LIST:
//         fprint_elif_lst_node(file, astree);
//         break;
//     case AST_WHILE_STMT:
//         fprint_while_stmt_node(file, astree);
//         break;
//     case AST_FOR_STMT:
//         fprint_for_stmt_node(file, astree);
//         break;
//     case AST_CONTINUE:
//         fprint_continue_node(file, astree);
//         break;
//     case AST_BREAK:
//         fprint_break_node(file, astree);
//         break;
//     case AST_PROGRAM:
//     case AST_CPD_STMT:
//         fprint_cpd_stmt_node(file, astree);
//         break;
//     case AST_RETURN:
//         fprint_return_node(file, astree);
//         break;
//     default:
//         break;
//     }
// }

void fprint_code(FILE *file, VSCodeObject *code) {
    int count = 0;
    VSObject *object;
    fprint_indent(file);
    fprintf(file, "%s: \n", STRING_TO_C_STRING(code->name).c_str());
    for (auto inst : code->code) {
        fprint_indent(file);
        fprintf(file, "%d: %s\t", count, OPCODE_STR[inst.opcode]);
        switch (inst.opcode) {
            case OP_LOAD_LOCAL:
            case OP_LOAD_LOCAL_CELL:
            case OP_STORE_LOCAL:
                object = LIST_GET(code->lvars, inst.operand);
                fprintf(file, "%s\n", STRING_TO_C_STRING(object).c_str());
                break;
            case OP_LOAD_FREE:
            case OP_LOAD_FREE_CELL:
            case OP_STORE_FREE:
                object = LIST_GET(code->freevars, inst.operand);
                fprintf(file, "%s\n", STRING_TO_C_STRING(object).c_str());
                break;
            case OP_LOAD_CELL:
            case OP_STORE_CELL:
                object = LIST_GET(code->cellvars, inst.operand);
                fprintf(file, "%s\n", STRING_TO_C_STRING(object).c_str());
                break;
            case OP_LOAD_ATTR:
            case OP_STORE_ATTR:
                object = LIST_GET(code->names, inst.operand);
                fprintf(file, "%s\n", STRING_TO_C_STRING(object).c_str());
                break;
            case OP_LOAD_CONST:
                object = LIST_GET(code->consts, inst.operand);
                if (VS_TYPEOF(object)->t_type == T_CODE) {
                    fprintf(file, "%s\n", STRING_TO_C_STRING(((VSCodeObject *)object)->name).c_str());
                } else {
                    VSObject *strobj = VS_TYPEOF(object)->__str__(object);
                    fprintf(file, "%s\n", STRING_TO_C_STRING(strobj).c_str());
                    DECREF_EX(strobj);
                }
                break;
            case OP_LOAD_BUILTIN:
                // break;
            case OP_JIF:
            case OP_JMP:
            case OP_BUILD_TUPLE:
            case OP_BUILD_LIST:
            case OP_BUILD_DICT:
            case OP_BUILD_SET:
                fprintf(file, "%u\n", inst.operand);
                break;
            default:
                fprintf(file, "\n");
                break;
        }
        count++;
    }

    indent++;
    for (vs_size_t i = 0; i < code->nconsts; i++) {
        VSObject *obj = LIST_GET(code->consts, i);
        if (VS_TYPEOF(obj)->t_type == T_CODE) {
            fprint_code(file, (VSCodeObject *)obj);
        }
    }
    indent--;
}
