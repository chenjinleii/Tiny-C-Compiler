//
// Created by kaiser on 18-12-8.
//

#include "error.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"
#include "code_gen.h"
#include "obj_gen.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_set>
#include <sstream>
#include <sys/wait.h>

void ShowHelpInfo();
void ShowVersionInfo();
bool FileExists(const std::string &file_name);
std::string RemoveExtension(const std::string &file_name);
bool CommandSuccess(std::int32_t status);
void RunTcc(const std::string &input_file, std::ostringstream &obj_files,
            std::vector<std::string> &files_to_delete, bool optimization);
void RunTest();

int main(int argc, char *argv[]) {
#ifndef __linux
    std::cerr << "Only support linux system\n";
    std::exit(EXIT_FAILURE);
#else
    if (argc == 1) {
        ShowHelpInfo();
        std::exit(EXIT_SUCCESS);
    }

    std::unordered_set<std::string> input_files;
    std::unordered_set<std::string> args;
    std::string program_name("a.out");

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == 'o') {
                if (argv[i + 1]) {
                    program_name = argv[i + 1];
                    ++i;
                } else {
                    tcc::ErrorReportAndExit("error: No program name entered.\n");
                }
            } else {
                args.emplace(argv[i]);
            }
        } else {
            if (FileExists(argv[i])) {
                input_files.emplace(argv[i]);
            } else {
                tcc::ErrorReportAndExit("error: {}: This file does not exist.\n", argv[i]);
            }
        }
    }

    if (args.find("-h") != std::end(args)) {
        ShowHelpInfo();
        std::exit(EXIT_SUCCESS);
    } else if (args.find("-v") != std::end(args)) {
        ShowVersionInfo();
        std::exit(EXIT_SUCCESS);
    } else if (args.find("-t") != std::end(args)) {
        RunTest();
        std::exit(EXIT_SUCCESS);
    }

    bool optimization{false};

    for (const auto &arg:args) {
        switch (arg[1]) {
            case 'O':optimization = true;
                break;
            default: {
                tcc::ErrorReportAndExit("Unknown compilation option.\n");
                break;
            }
        }
    }

    if (auto size{std::size(input_files)};size == 0) {
        tcc::ErrorReportAndExit("fatal error: no input files.\n");
    } else if (size > 1) {
        tcc::ErrorReportAndExit("Do not support multiple files at this time.\n");
    }

    std::vector<std::string> files_to_delete;
    std::ostringstream obj_files;

    for (const auto &input_file:input_files) {
        RunTcc(input_file, obj_files, files_to_delete, optimization);
    }

    std::string cmd("gcc -std=c99 -o " + program_name + obj_files.str());
    if (auto status{std::system(cmd.c_str())};!CommandSuccess(status)) {
        tcc::ErrorReportAndExit("Link Failed\n");
    }

    for (const auto &file:files_to_delete) {
        std::filesystem::remove(std::filesystem::path{file});
    }

    if (FileExists(program_name)) {
        std::cout << "Compiled Successfully\n";
        std::cout << "The name of the executable is " + program_name << '\n';
        std::exit(EXIT_SUCCESS);
    } else {
        tcc::ErrorReportAndExit("Compile Failed\n");
    }

#endif
}

void ShowHelpInfo() {
    std::cout << "Usage: tcc [options] a file\n"
                 "Options: \n"
                 "-h\t\t\tDisplay Display help information\n"
                 "-v\t\t\tDisplay version information.\n"
                 "-o\t\t\tSpecify program name\n"
                 "-O\t\t\tTurn on optimization\n"
                 "-t\t\t\tTest mode (developer use)\n";
}

void ShowVersionInfo() {
    std::cout << "Tiny C Compiler by Kaiser.\n";
}

bool FileExists(const std::string &file_name) {
    return std::filesystem::exists(std::filesystem::path{file_name});
}

std::string RemoveExtension(const std::string &file_name) {
    return file_name.substr(0, file_name.find('.'));
}

bool CommandSuccess(std::int32_t status) {
    return status != -1 && WIFEXITED(status) && !WEXITSTATUS(status);
}

void RunTcc(const std::string &input_file, std::ostringstream &obj_files,
            std::vector<std::string> &files_to_delete, bool optimization) {
    std::string processed_file(RemoveExtension(input_file) + ".i");
    files_to_delete.push_back(processed_file);

    std::string cmd("gcc -std=c99 -o " + processed_file + " -E " + input_file);
    if (auto status{std::system(cmd.c_str())};!CommandSuccess(status)) {
        tcc::ErrorReportAndExit("Preprocessing Failure\n");
    }

    tcc::Scanner scanner{processed_file, input_file};

    tcc::Parser parse{scanner.Scan()};
    auto root{parse.Parse()};

    tcc::CodeGenContext context(optimization);
    context.GenerateCode(*root);

    std::string obj_file(RemoveExtension(input_file) + ".o");
    files_to_delete.push_back(obj_file);
    obj_files << ' ' << obj_file;

    tcc::ObjGen(context, obj_file);
}

void RunTest() {
    std::cout << "Test Mode\n";
    std::string input_file("/home/kaiser/CLionProjects/Tiny-C-Compiler/test/test.c");

    std::string processed_file(RemoveExtension(input_file) + ".i");
    std::string cmd("gcc -std=c99 -o " + processed_file + " -E " + input_file);
    if (auto status{std::system(cmd.c_str())};!CommandSuccess(status)) {
        tcc::ErrorReportAndExit("Preprocessing Failure.\n");
    }

    std::ofstream ofs{"../test/token"};
    if (!ofs) {
        tcc::ErrorReportAndExit("Can not open token file.\n");
    }
    auto token_sequence{tcc::Scanner::Test(processed_file, input_file, ofs)};

    std::ofstream ast{"../test/ast.json"};
    if (!ast) {
        tcc::ErrorReportAndExit("Can not open json file.\n");
    }
    auto ast_root{tcc::Parser::Test(token_sequence, ast)};

    tcc::CodeGenContext context(false);
    context.Debug(*ast_root, "../test/ir");
    std::cout << "LLVM IR Successfully Generate\n";

    tcc::ObjGen(context, "test.o");

    if (auto status{std::system("gcc -std=c99 -o test test.o")};!CommandSuccess(status)) {
        tcc::ErrorReportAndExit("Link failed\n");
    }

    std::cout << "Compiled Successfully\n";
    std::cout << "The program runs-----------------------------------------------\n\n";

    if (auto status{std::system("./test")};!CommandSuccess(status)) {
        tcc::ErrorReportAndExit("Program Startup Failed\n");
    }
}
