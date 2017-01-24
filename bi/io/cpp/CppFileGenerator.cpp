/**
 * @file
 */
#include "bi/io/cpp/CppFileGenerator.hpp"

#include "bi/io/cpp/CppModelGenerator.hpp"
#include "bi/io/cpp/CppDispatcherGenerator.hpp"
#include "bi/io/cpp/CppParameterGenerator.hpp"
#include "bi/io/cpp/CppOutputGenerator.hpp"
#include "bi/io/cpp/CppReturnGenerator.hpp"
#include "bi/io/cpp/misc.hpp"
#include "bi/program/all.hpp"
#include "bi/exception/all.hpp"

#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"

#include <sstream>
#include <iomanip>
#include <typeinfo>

bi::CppFileGenerator::CppFileGenerator(std::ostream& base, const int level,
    const bool header) :
    CppBaseGenerator(base, level, header) {
  //
}

void bi::CppFileGenerator::visit(const File* o) {
  line("/**");
  line(" * @file");
  line(" *");
  line(" * Automatically generated by Birch.");
  line(" */");

  if (header) {
    /* include guard */
    line("#pragma once\n");

    /* standard headers */
    line("#include \"bi/bi.hpp\"");
  } else {
    /* include main header file */
    boost::filesystem::path file(o->path);
    file.replace_extension(".hpp");
    line("#include \"" << file.filename().string() << "\"\n");

    /* dispatcher code */
    CppDispatcherGenerator auxDispatcher(o->scope.get(), base, level, header);
    auxDispatcher << o;
  }

  /* main code */
  *this << o->root;
}

void bi::CppFileGenerator::visit(const Import* o) {
  if (header) {
    line("#include \"" << hpp(o->path->file()) << "\"");
  }
}

void bi::CppFileGenerator::visit(const VarDeclaration* o) {
  if (header) {
    line("namespace bi {");
    line("extern " << o->param->type << ' ' << o->param->name << ';');
    line("}\n");
  } else {
    start(o->param->type << " bi::" << o->param->name);
    if (!o->param->value->isEmpty()) {
      middle(" = " << o->param->value);
    }
    finish(';');
  }
}

void bi::CppFileGenerator::visit(const FuncParameter* o) {
  if (!o->braces->isEmpty()) {
    bool op = (o->isUnary() || o->isBinary())
        && isTranslatable(o->name->str());
    // ^ prefix and infix operators go in bi namespace, not bi::function

    if (header) {
      line("namespace bi {");
      //if (!op) {
      //  in();
      //  line("namespace function {");
      //  out();
      //}
    }

    /* type */
    start(o->type << ' ');

    /* name */
    if (!header) {
      middle("bi::");
      //if (!op) {
      //  middle("function::");
      //}
    }
    if ((o->isBinary() || o->isUnary()) && isTranslatable(o->name->str())
        && !o->parens->isRich()) {
      middle("operator" << translate(o->name->str()));
    } else {
      middle(o->mangled);
    }

    /* parameters */
    CppParameterGenerator auxParameter(base, level, header);
    auxParameter << o;

    if (header) {
      finish(';');
    } else {
      finish(" {");
      in();

      /* output parameters */
      CppOutputGenerator auxOutput(base, level, header);
      auxOutput << o;

      /* body */
      CppBaseGenerator aux(base, level, header);
      aux << o->braces;

      /* return statement */
      if (!o->result->isEmpty()) {
        CppReturnGenerator auxReturn(base, level, header);
        auxReturn << o;
      }

      out();
      finish("}\n");
    }
    if (header) {
      //if (!op) {
      //  in();
      //  line("}");
      //  out();
      //}
      line("}\n");
    }
  }
}

void bi::CppFileGenerator::visit(const ModelParameter* o) {
  if (*o->op == "=") {
    if (header) {
      ModelReference* base = dynamic_cast<ModelReference*>(o->base.get());
      assert(base);
      line("namespace bi {");
      in();
      line("namespace model {");
      out();
      line("template<class Group = StackGroup>");
      line("using " << o->name << " = " << base->name << "<Group>;");
      in();
      line("}");
      out();
      line("}\n");
    }
  } else if (!o->braces->isEmpty()) {
    if (header) {
      line("namespace bi {");
      in();
      line("namespace model {");
      out();
    }
    CppModelGenerator auxModel(base, level, header);
    auxModel << o;
    if (header) {
      in();
      line("}");
      out();
      line("}\n");
    }
  }
}

void bi::CppFileGenerator::visit(const ProgParameter* o) {
  if (header) {
    line("namespace bi {");
    in();
    line("namespace program {");
    out();
    line("extern \"C\" void " << o->name << "(int argc, char** argv);");
    in();
    line("}");
    out();
    line("}\n");
  } else {
    line("void bi::program::" << o->name << "(int argc, char** argv) {");
    in();
    if (o->inputs.size() > 0) {
      /* option variables */
      for (auto iter = o->inputs.begin(); iter != o->inputs.end(); ++iter) {
        line(*iter << ';');
      }
      line("");

      /* option flags */
      line("enum {");
      in();
      for (auto iter = o->inputs.begin(); iter != o->inputs.end(); ++iter) {
        std::string flag = (*iter)->name->str() + "_ARG";
        boost::to_upper(flag);
        start(flag);
        if (iter == o->inputs.begin()) {
          middle(" = 256");
        }
        finish(',');
      }
      out();
      line("};");

      /* long options */
      line("int c, option_index;");
      line("option long_options[] = {");
      in();
      for (auto iter = o->inputs.begin(); iter != o->inputs.end(); ++iter) {
        const std::string& name = (*iter)->name->str();
        if (name.length() > 1) {
          std::string flag = name + "_ARG";
          boost::to_upper(flag);
          std::string option = name;
          boost::replace_all(option, "_", "-");

          line(
              "{\"" << option << "\", required_argument, 0, " << flag << " },");
        }
      }
      line("{0, 0, 0, 0}");
      out();
      line("};");

      /* short options */
      start("const char* short_options = \"");
      for (auto iter = o->inputs.begin(); iter != o->inputs.end(); ++iter) {
        const std::string& name = (*iter)->name->str();
        if (name.length() == 1) {
          middle(name << ':');
        }
      }
      finish("\";");

      /* read in options with getopt_long */
      line("opterr = 0; // handle error reporting ourselves");
      line(
          "c = getopt_long(argc, argv, short_options, long_options, &option_index);");
      line("while (c != -1) {");
      in();
      line("switch (c) {");
      in();

      for (auto iter = o->inputs.begin(); iter != o->inputs.end(); ++iter) {
        const std::string& name = (*iter)->name->str();
        std::string flag = name + "_ARG";
        boost::to_upper(flag);

        start("case ");
        if (name.length() > 1) {
          middle(flag);
        } else {
          middle('\'' << name << '\'');
        }
        finish(':');
        in();
        start(name << " = ");
        Type* type = (*iter)->type->strip();

        if (typeid(*type) == typeid(RandomType)) {
          type = dynamic_cast<const RandomType*>(type)->left.get();
        }
        if (typeid(*type) == typeid(ModelReference)) {
          std::string typeName = dynamic_cast<const ModelReference*>(type)->name->str();
          if (typeName.compare("Boolean") == 0) {
            middle("bi::make_bool(atoi(optarg))");
          } else if (typeName.compare("Integer") == 0) {
            middle("bi::make_int(atoi(optarg))");
          } else if (typeName.compare("Real") == 0) {
            middle("bi::make_real(atof(optarg))");
          } else if (typeName.compare("String") == 0) {
            middle("bi::make_string(optarg)");
          } else {
            throw UnsupportedOptionTypeException(type);
          }
        } else {
          throw UnsupportedOptionTypeException(type);
        }
        finish(';');
        line("break;");
        out();
      }
      line("default:");
      in();
      line("throw UnknownOptionException(argv[optind - 1]);");
      line("break;");
      out();
      out();
      line('}');
      line(
          "c = getopt_long(argc, argv, short_options, long_options, &option_index);");
      out();
      line("}\n");
    }

    /* body of program */
    if (!o->braces->isEmpty()) {
      CppBaseGenerator aux(base, level, header);
      aux << o->braces;
    }

    out();
    line("}\n");
  }
}

std::string bi::CppFileGenerator::hpp(const std::string& path) {
  boost::filesystem::path file = path;
  file.replace_extension(".hpp");
  return file.string();
}
