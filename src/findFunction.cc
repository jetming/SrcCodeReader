#include <clang-c/Index.h>
#include <sys/stat.h>
#include <cstdio>
#include <string.h>

static bool exists(const char *FileName) {
  struct stat Stat;
  if (stat(FileName, &Stat)) {
    return false;
  }
  return true;
}

bool printKindSpelling(CXCursor cursor) {
	enum CXCursorKind curKind = clang_getCursorKind(cursor);
	const char *curkindSpelling = clang_getCString(
		clang_getCursorKindSpelling(curKind));
	
	const char s[] = "FunctionDecl";
	if (strcmp(curkindSpelling, s) == 0) {
		printf("The AST node kind spelling is:%s\n", curkindSpelling);
		return true;
	}
	else
		return false;
}

bool printSpelling(CXCursor cursor) {
	const char *astSpelling = clang_getCString(clang_getCursorSpelling(cursor));
	printf("The AST node spelling is:%s\n", astSpelling);
	return true;
}

bool printLocation(CXCursor cursor) {
	CXSourceRange range = clang_getCursorExtent(cursor);
	CXSourceLocation startLocation = clang_getRangeStart(range);
	CXSourceLocation endLocation = clang_getRangeEnd(range);

	CXFile file;
	unsigned int line, column, offset;
	clang_getInstantiationLocation(startLocation, &file, &line, &column, &offset);
	printf("Start: Line: %u Column: %u Offset: %u\n", line, column, offset);
	clang_getInstantiationLocation(endLocation, &file, &line, &column, &offset);
	printf("End: Line: %u Column: %u Offset: %u\n\n", line, column, offset);

	return true;
}

enum CXChildVisitResult printVisitor(CXCursor cursor, CXCursor parent, CXClientData client_data) {
	if (printKindSpelling(cursor)) {
		printSpelling(cursor);
		printLocation(cursor);
	}
	return CXChildVisit_Recurse;
}

int main(int argc, char *argv[]) {
  clang_enableStackTraces();
  if (argc < 2 || !exists(argv[1])) {
    fprintf(stderr, "Invalid Arguments\n");
    return -1;
  }
  auto Index = clang_createIndex(0, 0);
  CXTranslationUnit TU;
  if (CXError_Success != clang_parseTranslationUnit2(
                             Index, nullptr, argv + 1, argc - 1, nullptr, 0,
                             clang_defaultEditingTranslationUnitOptions(),
                             &TU)) {
    fprintf(stderr, "Unable to parse File");
    clang_disposeIndex(Index);
    return -1;
  }
  CXCursor C = clang_getTranslationUnitCursor(TU);
  clang_visitChildren(C, printVisitor, NULL);

  clang_disposeTranslationUnit(TU);
  clang_disposeIndex(Index);
}
