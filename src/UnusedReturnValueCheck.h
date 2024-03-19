//===--- UnusedReturnValueCheck.h - clang-tidy ------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_CPPSAFE_UNUSEDRETURNVALUECHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_CPPSAFE_UNUSEDRETURNVALUECHECK_H

#include <clang-tidy/ClangTidyCheck.h>

namespace clang::tidy::cppsafe {

// copied from
// https://github.com/llvm/clangir/blob/main/clang-tools-extra/clang-tidy/bugprone/UnusedReturnValueCheck.cpp
class UnusedReturnValueCheck : public ClangTidyCheck {
public:
    UnusedReturnValueCheck(StringRef Name, ClangTidyContext* Context);
    void registerMatchers(ast_matchers::MatchFinder* Finder) override;
    void storeOptions(ClangTidyOptions::OptionMap& Opts) override;
    void check(const ast_matchers::MatchFinder::MatchResult& Result) override;

private:
    std::string IgnoredFunctions;
    std::string IgnoredFunctionRegexStr;
    bool AllowCastToVoid;
    std::optional<llvm::Regex> IgnoredFunctionRegex;
};

} // namespace clang::tidy::cppsafe

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_CPPSAFE_UNUSEDRETURNVALUECHECK_H
