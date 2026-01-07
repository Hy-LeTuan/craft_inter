#pragma once

#include <any>
#include <vector>
#include <expr.hpp>
#include <stmt.hpp>

using Object = std::any;
using VecStmt = std::vector<stmt::Stmt*>;
using VecExpr = std::vector<expr::Expr*>;
