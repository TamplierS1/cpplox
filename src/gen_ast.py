# This script generates data-classes that represent non-terminals in a context-free grammar


def find_whitespace(string):
    index = 0
    # find the index of whitespace in string
    for char in string:
        if char == ' ':
            break
        index += 1
    return index


def generate_visitor(types):
    with open("../include/syntax_tree/visitor_expr.h", 'w') as file:
        header_guard = "#ifndef VISITOR_H\n#define VISITOR_H\n\n"
        file.write(header_guard)

        file.write("#include \"value.h\"\n")

        file.write('\n')
        file.write("namespace cpplox::ast\n")
        file.write("{\n\n")

        # forward declarations
        for tp in types:
            file.write("class " + tp + ';\n')

        file.write('\n')
        file.write("class Visitor\n")
        file.write('{\n')
        file.write("public:\n")

        for tp in types:
            file.write("virtual Value visit(" + tp + "* expr) = 0;\n")

        file.write("};\n}\n\n")

        # closing header guard
        file.write("#endif // VISITOR_H\n")


def generate_ast(class_name, base_class, includes, fields):
    with open("../include/syntax_tree/" + class_name.lower() + ".h", 'w') as file:
        header_guard = "#ifndef " + class_name.upper() + "_H\n" \
                                                         "#define " + class_name.upper() + "_H\n"
        file.write(header_guard + '\n')

        for inc in includes:
            file.write("#include \"" + inc + ".h\"\n")

        file.write('\n')
        file.write("namespace cpplox::ast\n")
        file.write("{\n")
        file.write("class " + class_name + " : public " + base_class + '\n')
        file.write("{\n")
        file.write("public:\n")
        file.write(class_name + "(")

        # parameters
        for i, field in enumerate(fields):
            index = find_whitespace(field)
            # print something like 'const Expression& expr'

            # don't use pointers in Literal class
            if class_name == "Literal":
                field = field[:index] + " " + field[index + 1:]
            else:
                field = field[:index] + "* " + field[index + 1:]
            file.write(field)

            # last field
            if i == len(fields) - 1:
                break
            file.write(", ")

        file.write(")\n")

        # init fields in constructor
        file.write(": ")
        for i, field in enumerate(fields):
            index = find_whitespace(field)
            field_name = field[index + 1:]
            file.write("m_" + field_name + "(std::make_shared<" + field[:index] + ">(" + field_name + "))")

            # last field
            if i == len(fields) - 1:
                break
            file.write(", ")

        file.write("\n{}\n\n")

        # 'accept' method
        file.write("Value accept(Visitor* visitor) override\n")
        file.write("{\n")
        file.write("return visitor->visit(this);\n")
        file.write("}\n\n")

        # fields
        for field in fields:
            index = find_whitespace(field)
            field_name = field[index + 1:]
            file.write("std::shared_ptr<" + field[:index] + "> m_" + field_name + ";\n")

        file.write("};\n\n")
        # close namespace
        file.write("}\n")
        # closing header guard
        file.write("#endif // " + class_name.upper() + "_H")


if __name__ == "__main__":
    # binary.h
    generate_ast("Binary", "Expression",
                 ["expression", "token"], ["Expression left", "cpplox::Token op", "Expression right"])
    # grouping.h
    generate_ast("Grouping", "Expression",
                 ["expression"], ["Expression expression"])
    # unary.h
    generate_ast("Unary", "Expression",
                 ["expression"], ["cpplox::Token op", "Expression right"])
    # literal.h
    generate_ast("Literal", "Expression",
                 ["expression", "../literal"], ["cpplox::Literal value"])
    # visitor.h
    generate_visitor(["Binary", "Grouping", "Literal", "Unary"])
