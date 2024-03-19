//===------- CppsafeTidyModule.cpp - clang-tidy ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "UnusedReturnValueCheck.h"

#include <clang-tidy/ClangTidyCheck.h>
#include <clang-tidy/ClangTidyModule.h>
#include <clang-tidy/ClangTidyModuleRegistry.h>

namespace clang::tidy {
namespace cppsafe {

class CppsafeModule : public ClangTidyModule {
public:
    void addCheckFactories(ClangTidyCheckFactories& CheckFactories) override
    {
        CheckFactories.registerCheck<UnusedReturnValueCheck>("cppsafe-unused-return-value");
    }
};

// Register the CppsafeModule using this statically initialized variable.
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static ClangTidyModuleRegistry::Add<CppsafeModule> X("cppsafe-module", "Add Cppsafe checks.");

} // namespace cppsafe

// This anchor is used to force the linker to link in the generated object file
// and thus register the CppsafeModule.
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
volatile int CppsafeModuleAnchorSource = 0;

} // namespace clang::tidy
