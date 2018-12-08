//
// Created by kaiser on 18-12-8.
//

#include "scanner.h"
#include "parser.h"
#include "ast.h"
#include "code_gen.h"
#include "obj_gen.h"

#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <algorithm>
#include <unordered_set>
#include <memory>
#include <sstream>

void ShowHelpInfo();
bool FileExists(const std::string &input_file);
void ShowVersionInfo();
std::string RemoveExtension(const std::string &file_name);
void RunTcc(const std::string &input_file,
            std::ostringstream &obj_files, std::vector<std::string> &files_to_delete);

int main(int argc, char *argv[]) {
    if (argc == 1) {
        ShowHelpInfo();
        std::exit(EXIT_SUCCESS);
    }

    std::unordered_set<std::string> input_files;
    std::unordered_set<std::string> args;
    std::string program_name("a.out");

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            if (FileExists(argv[i])) {
                input_files.emplace(argv[i]);
            } else {
                std::cerr << "error: " << argv[i] << ": This file does not exist.\n";
                std::exit(EXIT_FAILURE);
            }
        } else {
            if (argv[i][1] == 'o') {
                if (argv[i + 1]) {
                    program_name = argv[i + 1];
                } else {
                    std::cerr << "error: " << "No program name entered.\n";
                    std::exit(EXIT_FAILURE);
                }
            } else {
                args.emplace(argv[i]);
            }
        }
    }

    if (args.find("-v") != std::end(args)) {
        ShowVersionInfo();
        std::exit(EXIT_SUCCESS);
    }

    if (std::size(input_files) == 0) {
        std::cerr << "fatal error: no input files.\n";
    }

    //TODO 支持更多编译参数
    for (const auto &arg:args) {
        switch (arg[1]) {
            default:break;
        }
    }

    std::vector<std::string> files_to_delete;
    std::ostringstream obj_files;

    for (const auto &input_file:input_files) {
        RunTcc(input_file, obj_files, files_to_delete);
    }

    std::string cmd("gcc -std=c99 -o a.out " + obj_files.str());
    std::system(cmd.c_str());

    for (const auto &file:files_to_delete) {
        std::filesystem::remove(std::filesystem::path{file});
    }
}

void ShowHelpInfo() {
    std::cout << "Usage: tcc [options] file...\n"
                 "Options: \n"
                 "-v\t\t\tDisplay version information.\n";
}

bool FileExists(const std::string &input_file) {
    return std::filesystem::exists(std::filesystem::path{input_file});
}

void ShowVersionInfo() {
    std::cout << "Tiny C Compiler by Kaiser.\n";
}

std::string RemoveExtension(const std::string &file_name) {
    return file_name.substr(0, file_name.find('.'));
}

void RunTcc(const std::string &input_file,
            std::ostringstream &obj_files, std::vector<std::string> &files_to_delete) {
    std::string processed_file(RemoveExtension(input_file) + ".i");
    files_to_delete.push_back(processed_file);

    std::string cmd("gcc -std=c99 -o " + processed_file + " -E " + input_file);
    std::system(cmd.c_str());

    Scanner scanner{processed_file};
    auto token_sequence{scanner.GetTokenSequence()};

    //Parser parse;
    //parse.parse();

    std::unique_ptr<Block> program_block;

    CodeGenContext context;
    context.GenerateCode(*program_block);

    std::string obj_file(RemoveExtension(input_file) + ".o");
    files_to_delete.push_back(obj_file);
    obj_files << obj_file << ' ';

    ObjGen(context, obj_file);
}