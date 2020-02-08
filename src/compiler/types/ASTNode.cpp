#include "ASTNode.hpp"

ASTNode::ASTNode(ASTNODE_TYPE node_type, ASTNODE_TYPE ext_node_type)
    : node_type(node_type), ext_node_type(ext_node_type)
{
}

ASTNode::~ASTNode()
{
}