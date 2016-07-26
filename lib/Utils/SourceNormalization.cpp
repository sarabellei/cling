//------------------------------------------------------------------------------
// CLING - the C++ LLVM-based InterpreterG :)
// author:  Lukasz Janyst <ljanyst@cern.ch>
//
// This file is dual-licensed: you can choose to license it under the University
// of Illinois Open Source License or the GNU Lesser General Public License. See
// LICENSE.TXT for details.
//------------------------------------------------------------------------------

#include "cling/Utils/SourceNormalization.h"

#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Lexer.h"

#include <utility>

using namespace clang;

namespace {
///\brief A Lexer that exposes preprocessor directives.
class MinimalPPLexer: public Lexer {
public:
  ///\brief Construct a Lexer from LangOpts and source.
  MinimalPPLexer(const LangOptions &LangOpts, llvm::StringRef source):
    Lexer(SourceLocation(), LangOpts,
          source.begin(), source.begin(), source.end()) {}

  bool inPPDirective() const { return ParsingPreprocessorDirective; }

  ///\brief Lex, forwarding to Lexer::LexFromRawLexer, and keeping track of
  /// preprocessor directives to provide a tok::eod corresponding to a
  /// tok::hash.
  bool Lex(Token& Tok) {
    bool ret = LexFromRawLexer(Tok);
    if (inPPDirective()) {
      // Saw a PP directive; probe for eod to end PP parsing mode.
      if (Tok.is(tok::eod))
        ParsingPreprocessorDirective = false;
    } else {
      if (Tok.is(tok::hash)) {
        // Found a PP directive, request tok::eod to be generated.
        ParsingPreprocessorDirective = true;
      }
    }
    return ret;
  }

  ///\brief Advance to token with given token kind.
  ///
  /// \param Tok - Token to advance.
  /// \param kind - Token kind where to stop lexing.
  /// \return - Result of most recent call to Lex().
  bool AdvanceTo(Token& Tok, tok::TokenKind kind) {
    while (!Lex(Tok)) {
      if (Tok.is(kind))
        return false;
    }
    return true;
  }
};

size_t getFileOffset(const Token& Tok) {
  return Tok.getLocation().getRawEncoding();
}

}

size_t
cling::utils::isUnnamedMacro(llvm::StringRef source,
                             clang::LangOptions& LangOpts) {
  // Find the first token that is not a non-cpp directive nor a comment.
  // If that token is a '{' we have an unnamed macro.

  MinimalPPLexer Lex(LangOpts, source);
  Token Tok;
  while (!Lex.Lex(Tok)) {
    if (Lex.inPPDirective())
      continue; // Skip PP directives.

    const tok::TokenKind kind = Tok.getKind();
    if (kind == tok::comment) continue; // ignore comments
    if (kind == tok::l_brace)
      return getFileOffset(Tok);

    return std::string::npos;
  }

  // Empty file?

  return std::string::npos;
}



size_t cling::utils::getWrapPoint(std::string& source,
                                  const clang::LangOptions& LangOpts) {
  // TODO: For future reference.
  // Parser* P = const_cast<clang::Parser*>(m_IncrParser->getParser());
  // Parser::TentativeParsingAction TA(P);
  // TPResult result = P->isCXXDeclarationSpecifier();
  // TA.Revert();
  // return result == TPResult::True();

  MinimalPPLexer Lex(LangOpts, source);
  Token Tok;

  size_t wrapPoint;

  while (!Lex.Lex(Tok)) {
    if (Lex.inPPDirective()) {
      wrapPoint = getFileOffset(Tok);
      continue; // Skip PP directives; they just move the wrap point.
    }

    const tok::TokenKind kind = Tok.getKind();

    if (kind == tok::raw_identifier && !Tok.needsCleaning()) {
      StringRef keyword(Tok.getRawIdentifier());
      if (keyword.equals("using")) {
        // FIXME: Using definitions and declarations should be decl extracted.
        // Until we have that, don't wrap them if they are the only input.
        if (Lex.AdvanceTo(Tok, tok::semi)) {
          // EOF while looking for semi. Don't wrap.
          return std::string::npos;
        }
        // There is "more" - let's assume this input consists of a using
        // declaration or definition plus some code that should be wrapped.
        return getFileOffset(Tok);
      }
      if (keyword.equals("extern"))
        return std::string::npos;
      if (keyword.equals("namespace"))
        return std::string::npos;
      if (keyword.equals("template"))
        return std::string::npos;
      // Else there is something else here that needs to be wrapped.
      return wrapPoint;
    }
  }

  // We have only had PP directives; no need to wrap.
  return std::string::npos;
}