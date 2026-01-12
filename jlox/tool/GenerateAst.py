from io import TextIOWrapper
import sys

def declareVisitor(file: TextIOWrapper, baseName: str, types: list[str]):
    _ = file.write(f"class Visitor\n")
    _ = file.write(f"{{\n")
    _ = file.write(f"public:\n")

    for type in types:
        [typeName, fields] = type.split(":")

        typeName = typeName.strip()
        fields = fields.strip()

        _ = file.write(
            f"virtual std::any visit{typeName + baseName}(const {typeName}* {baseName.lower()}) = 0;\n")

    _ = file.write(f"}};\n")


def declareType(file: TextIOWrapper, baseName: str, className: str, fields: str):
    _ = file.write(f"class {className} : public {baseName}\n")
    _ = file.write("{\n")
    _ = file.write("public:\n")
    _ = file.write(f"{className}({fields});\n")
    _ = file.write(f"~{className}() override;\n")
    _ = file.write(f"\n")
    _ = file.write(f"std::any accept(Visitor* visitor) const override;\n")
    _ = file.write("\n")
    for field in fields.split(", "):
        _ = file.write(f"const {field};\n")

    _ = file.write("};\n")
    _ = file.write("\n")


def defineType(file: TextIOWrapper, baseName: str, className: str, fields: str):
    _ = file.write(f"{className}::{className}({fields})\n")

    for i, field in enumerate(fields.split(", ")):
        [fieldType, fieldName] = field.split()
        fieldType = fieldType[:len(fieldType) - 2]

        delimiter = ":" if i == 0 else ","
        _ = file.write(
            f"  {delimiter} {fieldName}{{ {fieldName} }}\n")

    _ = file.write("{\n")
    _ = file.write("}\n")

    _ = file.write("\n")

    _ = file.write(f"{className}::~{className}()\n")
    _ = file.write(f"{{\n")
    for field in fields.split(", "):
        fieldName = field.split()[1]
        _ = file.write(f"if ({fieldName})\n")
        _ = file.write(f"{{\n")
        _ = file.write(f"delete {fieldName};\n")
        _ = file.write(f"}}\n")
    _ = file.write(f"}}\n")

    _ = file.write("\n")

    _ = file.write(f"std::any {className}::accept(Visitor* visitor) const\n")
    _ = file.write(f"{{\n")
    _ = file.write(f"return visitor->visit{className + baseName}(this);\n")
    _ = file.write(f"}}\n")


def defineAst(outputDir: str, baseName: str, types: list[str]):
    srcPath: str = outputDir + "/src/" + baseName.lower() + ".cpp"
    headerPath: str = outputDir + "/include/" + baseName.lower() + ".hpp"

    srcFile = open(srcPath, 'w', encoding="utf-8")
    headerFile = open(headerPath, 'w', encoding="utf-8")

    _ = headerFile.write("#pragma once\n")
    _ = headerFile.write("\n")
    _ = headerFile.write("#include <token.hpp>\n")

    if baseName.lower() != "expr":
        _ = headerFile.write("#include <expr.hpp>\n")

    _ = headerFile.write("\n")
    _ = headerFile.write("#include <any>\n")
    _ = headerFile.write("#include <vector>\n")

    _ = headerFile.write("\n")

    _ = headerFile.write("using std::vector;\n")
    if baseName.lower() != "expr":
        _ = headerFile.write("using expr::Expr;\n")
    _ = headerFile.write("\n")

    # open namespace
    _ = headerFile.write(f"namespace {baseName.lower()}\n")
    _ = headerFile.write("{\n")

    _ = headerFile.write(f"class Visitor;\n")
    _ = headerFile.write(f"\n")

    _ = headerFile.write(f"class {baseName}\n")
    _ = headerFile.write("{\n")
    _ = headerFile.write("public:\n")
    _ = headerFile.write(f"{baseName}() = default;\n")
    _ = headerFile.write(f"virtual ~{baseName}() = 0;\n")
    _ = headerFile.write("\n")

    _ = headerFile.write(f"{baseName}(const {baseName}&) = delete;\n")
    _ = headerFile.write(
        f"{baseName}& operator=(const {baseName}&) = delete;\n")
    _ = headerFile.write(f"{baseName}({baseName}&&) = delete;\n")
    _ = headerFile.write(f"{baseName}& operator=({baseName}&&) = delete;\n")
    _ = headerFile.write("\n")

    _ = headerFile.write(
        "virtual std::any accept(Visitor* visitor) const = 0;\n")
    _ = headerFile.write("};\n")
    _ = headerFile.write("\n")

    _ = srcFile.write(f"#include <{baseName.lower()}.hpp>\n")
    _ = srcFile.write("#include <token.hpp>\n")
    _ = srcFile.write("\n")
    _ = srcFile.write("#include <any>\n")
    _ = srcFile.write("\n")

    _ = srcFile.write(f"namespace {baseName.lower()}\n")
    _ = srcFile.write("{\n")
    _ = srcFile.write("\n")

    _ = srcFile.write(f"{baseName}::~{baseName}()\n")
    _ = srcFile.write(f"{{\n")
    _ = srcFile.write(f"}}\n")
    _ = srcFile.write(f"\n")

    for type in types:
        [className, fields] = type.split(":")

        className = className.strip()
        fields = fields.strip()

        declareType(headerFile, baseName, className, fields)
        _ = headerFile.write("\n")

        defineType(srcFile, baseName, className, fields)
        _ = srcFile.write("\n")

        continue

    declareVisitor(headerFile, baseName, types)

    # close namespace
    _ = headerFile.write("}\n")
    _ = srcFile.write("}\n")

    headerFile.close()
    srcFile.close()


def main():
    if len(sys.argv) < 2:
        print("Usage: GenerateAst <output directory>")
        sys.exit(64)

    outputDir: str = sys.argv[1]

    defineAst(outputDir, "Expr", [
        "Binary   : Expr* left, Token* op, Expr* right",
        "Call : Expr* callee, Token* paren, vector<Expr*>* arguments",
        "Get : Expr* object, Token* name",
        "Assign: Token* name, Expr* value",
        "Grouping : Expr* expression",
        "Literal  : LiteralValue* value",
        "Logical : Expr* left, Token* op, Expr* right",
        "Unary    : Token* op, Expr* right",
        "Variable : Token* name"
    ])

    # defineAst(outputDir, "Stmt", [
    #     "Expression : Expr* expression",
    #     "If : Expr* condition, Stmt* thenBranch, Stmt* elseBranch",
    #     "Function : Token* name, vector<Token*>* params, vector<Stmt*>* body",
    #     "Block : vector<Stmt*>* statements",
    #     "Class : Token* name, vector<Stmt*>* methods",
    #     "Print      : Expr* expression",
    #     "Return : Token* keyword, Expr* value",
    #     "Var        : Token* name, Expr* initializer",
    #     "While        : Expr* condition, Stmt* body"
    # ])


if __name__ == "__main__":
    main()
