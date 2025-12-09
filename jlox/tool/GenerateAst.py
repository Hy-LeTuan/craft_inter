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

        _ = file.write(f"std::any visit{typeName + baseName}({typeName}* {baseName.lower()});\n")

    _ = file.write(f"}};\n")


def defineType(file: TextIOWrapper, baseName: str, className: str, fields: str):
    _ = file.write(f"{className}::{className}({fields})\n")
    _ = file.write(f"  : Expr()\n")

    for field in fields.split(", "):
        [fieldType, fieldName] = field.split()
        fieldType = fieldType[:len(fieldType) - 2]

        _ = file.write(
            f"  , {fieldName}{{ std::make_unique<{fieldType}>(std::move({fieldName})) }}\n")

    _ = file.write("{\n")
    _ = file.write("}\n")

    _ = file.write("\n")

    _ = file.write(f"std::any {className}::accept(Visitor& visitor)\n")
    _ = file.write(f"{{\n")
    _ = file.write(f"return visitor.visit{className + baseName}(this);\n")
    _ = file.write(f"}}\n")


def declareType(file: TextIOWrapper, baseName: str, className: str, fields: str):
    _ = file.write(f"class {className} : public {baseName}\n")
    _ = file.write("{\n")
    _ = file.write("public:\n")
    _ = file.write(f"{className}({fields});\n")
    _ = file.write(f"std::any accept(Visitor& visitor) override;\n")
    _ = file.write("\n")

    _ = file.write("private:\n")
    for field in fields.split(", "):
        [fieldType, fieldName] = field.split()
        fieldType = fieldType[:len(fieldType) - 2]
        _ = file.write(f"const std::unique_ptr<{fieldType}> {fieldName};\n")

    _ = file.write("};\n")
    _ = file.write("\n")


def defineAst(outputDir: str, baseName: str, types: list[str]):
    srcPath: str = outputDir + "/src/" + baseName.lower() + ".cpp"
    headerPath: str = outputDir + "/include/" + baseName.lower() + ".hpp"

    srcFile = open(srcPath, 'w', encoding="utf-8")
    headerFile = open(headerPath, 'w', encoding="utf-8")

    _ = headerFile.write("#pragma once\n")
    _ = headerFile.write("\n")
    _ = headerFile.write("#include <token.hpp>\n")
    _ = headerFile.write("#include <memory>\n")
    _ = headerFile.write("#include <any>\n")
    _ = headerFile.write("\n")

    _ = headerFile.write(f"class Visitor;\n")
    _ = headerFile.write(f"\n")

    _ = headerFile.write(f"class {baseName}\n")
    _ = headerFile.write("{\n")
    _ = headerFile.write("public:\n")
    _ = headerFile.write("Expr() = default;\n")
    _ = headerFile.write("std::any virtual accept(Visitor& visitor);\n")
    _ = headerFile.write("};\n")
    _ = headerFile.write("\n")

    _ = srcFile.write("#include <any>\n")
    _ = srcFile.write("#include <expr.hpp>\n")
    _ = srcFile.write("#include <token.hpp>\n")
    _ = srcFile.write("\n")

    for type in types:
        [className, fields] = type.split(":")

        className = className.strip()
        fields = fields.strip()

        declareType(headerFile, baseName, className, fields)
        _ = headerFile.write("\n")

        defineType(srcFile, baseName,className, fields)
        _ = srcFile.write("\n")

        continue

    declareVisitor(headerFile, baseName, types)

    headerFile.close()
    srcFile.close()


def main():
    if len(sys.argv) < 2:
        print("Usage: GenerateAst <output directory>")
        sys.exit(64)

    outputDir: str = sys.argv[1]

    defineAst(outputDir, "Expr", [
        "Binary   : Expr&& left, Token&& op, Expr&& right",
        "Grouping : Expr&& expression",
        "Literal  : LiteralValue&& value",
        "Unary    : Token&& op, Expr&& right"
    ])


if __name__ == "__main__":
    main()
