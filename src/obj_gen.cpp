//
// Created by kaiser on 18-12-8.
//

#include "code_gen.h"

#include <llvm/Support/Host.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/ADT/Optional.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/LegacyPassManager.h>

#include <string>
#include <iostream>
#include <system_error>

namespace tcc {

void ObjGen(CodeGenContext &context, const std::string &obj_file) {
    // 初始化
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    // 获取当前计算机的目标三元组
    auto target_triple{llvm::sys::getDefaultTargetTriple()};
    context.the_module_->setTargetTriple(target_triple);

    // 如果我们找不到所请求的目标,则打印错误并退出
    std::string error;
    auto target{llvm::TargetRegistry::lookupTarget(target_triple, error)};

    if (!target) {
        std::cerr << error << '\n';
        std::exit(EXIT_FAILURE);
    }

    // TargetMachine类提供了对指定的计算机的完整机器描述
    // 使用通用CPU,无任何其他功能,选项或重定位模型
    std::string cpu("generic");
    std::string features;
    llvm::TargetOptions opt;
    llvm::Optional<llvm::Reloc::Model> rm;

    // 配置模块,指定目标机器和数据布局
    // 这不是必须的,但是这对优化有好处
    auto the_target_machine{target->createTargetMachine(target_triple, cpu, features, opt, rm)};
    context.the_module_->setDataLayout(the_target_machine->createDataLayout());

    // 定义要将文件写入的位置
    std::error_code error_code;
    llvm::raw_fd_ostream dest{obj_file, error_code, llvm::sys::fs::F_None};

    if (error_code) {
        std::cerr << "Could not open file: " << error_code.message() << '\n';
        std::exit(EXIT_FAILURE);
    }

    // 定义一个生成目标代码的pass并运行
    llvm::legacy::PassManager pass;
    auto file_type = llvm::TargetMachine::CGFT_ObjectFile;

    if (the_target_machine->addPassesToEmitFile(pass, dest, nullptr, file_type)) {
        std::cerr << "TheTargetMachine can't emit a file of this type\n";
        std::exit(EXIT_FAILURE);
    }

    pass.run(*context.the_module_);
    dest.flush();
}

}