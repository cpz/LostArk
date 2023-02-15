#ifndef FINDPATTERN_H
#define FINDPATTERN_H

#include "..\Include.h"

#ifdef LAZY_IMPORTER_HPP
#define PATTERN_USE_LAZY_IMPORTER
#endif

DWORD_PTR PatternScan(DWORD_PTR dwStart, DWORD_PTR dwSize, const char* pszSig, const char* pszMask);
PBYTE PatternScan(const std::string& image_name, const std::string& pattern);
#endif