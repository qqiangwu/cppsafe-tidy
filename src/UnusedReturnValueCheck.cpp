//===--- UnusedReturnValueCheck.cpp - clang-tidy --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "UnusedReturnValueCheck.h"

#include <clang-tidy/utils/Matchers.h>
#include <clang-tidy/utils/OptionsUtils.h>
#include <clang/AST/ASTContext.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>

using namespace clang::ast_matchers;
using namespace clang::ast_matchers::internal;

namespace clang::tidy::cppsafe {

namespace {

// Matches functions that are instantiated from a class template member function
// matching InnerMatcher. Functions not instantiated from a class template
// member function are matched directly with InnerMatcher.
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
AST_MATCHER_P(FunctionDecl, isInstantiatedFrom, Matcher<FunctionDecl>, InnerMatcher)
{
    FunctionDecl* InstantiatedFrom = Node.getInstantiatedFromMemberFunction();
    return InnerMatcher.matches(InstantiatedFrom ? *InstantiatedFrom : Node, Finder, Builder);
}
} // namespace

UnusedReturnValueCheck::UnusedReturnValueCheck(llvm::StringRef Name, ClangTidyContext* Context)
    : ClangTidyCheck(Name, Context)
    , IgnoredFunctions(Options.get("IgnoredFunctions", ""))
    , IgnoredFunctionRegexStr(Options.get("IgnoredFunctionRegex", ""))
    , AllowCastToVoid(Options.get("AllowCastToVoid", true))
{
    if (!IgnoredFunctionRegexStr.empty()) {
        IgnoredFunctionRegex.emplace(IgnoredFunctionRegexStr);
    }
}

void UnusedReturnValueCheck::storeOptions(ClangTidyOptions::OptionMap& Opts)
{
    Options.store(Opts, "IgnoredFunctions", IgnoredFunctions);
    Options.store(Opts, "IgnoredFunctionRegex", IgnoredFunctionRegexStr);
    Options.store(Opts, "AllowCastToVoid", AllowCastToVoid);
}

void UnusedReturnValueCheck::registerMatchers(MatchFinder* Finder)
{
    static constexpr auto PredefinedIgnoredFunctions = std::to_array<StringRef>({
        // posix
        "::sleep",
        "::usleep",
        // c std
        "::memcpy",
        "::std::memcpy",
        "::memset",
        "::std::memset",
        "::memmove",
        "::std::memmove",
        "printf",
        "fprintf",
        "sprintf",
        "snprintf",
        "strcpy",
        "strncpy",
        "signal",
        "fputs",
        // cpp std
        "::std::copy",
        "::std::copy_if",
        "::std::copy_n",
        "::std::copy_backward",
        "::std::transform",
        "erase",
        "erase_if",
        "emplace_back",
        "fetch_add",
        "fetch_sub",
        // fmtlib
        "::fmt::format_to",
    });
    auto FunVec = utils::options::parseStringList(IgnoredFunctions);
    FunVec.insert(FunVec.end(), PredefinedIgnoredFunctions.begin(), PredefinedIgnoredFunctions.end());

    auto MatchedDirectCallExpr = expr(
        callExpr(callee(functionDecl(
                     unless(returns(anyOf(voidType(), lValueReferenceType(pointee(unless(isConstQualified())))))),
                     isInstantiatedFrom(unless(hasAnyName(FunVec))))),
            unless(callee(cxxMethodDecl(
                hasAnyName("insert", "emplace"), returns(hasDeclaration(typedefNameDecl(hasName("iterator"))))))),
            unless(callee(cxxMethodDecl(hasParent(recordDecl(hasAnyName("::std::atomic", "__atomic_base")))))))
            .bind("match"));

    auto CheckCastToVoid = AllowCastToVoid ? castExpr(unless(hasCastKind(CK_ToVoid))) : castExpr();
    auto MatchedCallExpr = expr(anyOf(MatchedDirectCallExpr,
        explicitCastExpr(
            unless(cxxFunctionalCastExpr()), CheckCastToVoid, hasSourceExpression(MatchedDirectCallExpr))));

    auto UnusedInCompoundStmt = compoundStmt(forEach(MatchedCallExpr),
        // The checker can't currently differentiate between the
        // return statement and other statements inside GNU statement
        // expressions, so disable the checker inside them to avoid
        // false positives.
        unless(hasParent(stmtExpr())));
    auto UnusedInIfStmt = ifStmt(eachOf(hasThen(MatchedCallExpr), hasElse(MatchedCallExpr)));
    auto UnusedInWhileStmt = whileStmt(hasBody(MatchedCallExpr));
    auto UnusedInDoStmt = doStmt(hasBody(MatchedCallExpr));
    auto UnusedInForStmt
        = forStmt(eachOf(hasLoopInit(MatchedCallExpr), hasIncrement(MatchedCallExpr), hasBody(MatchedCallExpr)));
    auto UnusedInRangeForStmt = cxxForRangeStmt(hasBody(MatchedCallExpr));
    auto UnusedInCaseStmt = switchCase(forEach(MatchedCallExpr));

    Finder->addMatcher(stmt(anyOf(UnusedInCompoundStmt, UnusedInIfStmt, UnusedInWhileStmt, UnusedInDoStmt,
                           UnusedInForStmt, UnusedInRangeForStmt, UnusedInCaseStmt)),
        this);
}

void UnusedReturnValueCheck::check(const MatchFinder::MatchResult& Result)
{
    if (const auto* Matched = Result.Nodes.getNodeAs<CallExpr>("match")) {
        if (const auto* CE = Matched->getDirectCallee()) {
            if (IgnoredFunctionRegex) {
                if (IgnoredFunctionRegex->match(CE->getQualifiedNameAsString())) {
                    return;
                }
            }

            const bool Inogred = llvm::any_of(CE->specific_attrs<clang::AnnotateAttr>(),
                [](const clang::AnnotateAttr* Attr) { return Attr->getAnnotation() == "cppsafe::may_discard"; });
            if (Inogred) {
                return;
            }
        }

        diag(Matched->getBeginLoc(),
            "the value returned by this function should not be disregarded; "
            "neglecting it may lead to errors")
            << Matched->getSourceRange();

        if (!AllowCastToVoid) {
            return;
        }

        diag(Matched->getBeginLoc(), "cast the expression to void to silence this warning", DiagnosticIDs::Note);
    }
}

} // namespace clang::tidy::cppsafe
