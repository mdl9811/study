//
// Copyright (c) 2023 mdl. All rights reserved.
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#ifdef _WIN32

#undef _UNICODE
#undef UNICODE

#include <windows.h>

#pragma comment(lib, "kernel32.lib")

int main(int argc, char* argv[]) {
  if (argc == 3) {
    BOOL result = CopyFile(argv[1], argv[2], FALSE);
    if (result) {
      HANDLE hFile = CreateFile(argv[2], FILE_WRITE_ATTRIBUTES,
                                FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
      if (hFile != INVALID_HANDLE_VALUE) {
        FILETIME ft;
        SYSTEMTIME st;

        GetSystemTime(&st);
        SystemTimeToFileTime(&st, &ft);
        SetFileTime(hFile, (LPFILETIME)NULL, (LPFILETIME)NULL, &ft);
        CloseHandle(hFile);
      }
      return 0;
    }
    return 1;
  }
}

#else
int main(int argc, char* argv[]) {
  if (argc == 3) {
    char buffer[1024 * 1024];
    size_t bytes;

    FILE* fi = fopen(argv[1], "rb");
    if (fi) {
      FILE* out = fopen(argv[2], "wb");
      if (out) {
        while (0 < (bytes = fread(buffer, 1, sizeof(buffer), fi)))
          fwrite(buffer, 1, bytes, out);
        return 0;
      }
    }
  }
  return 1;
}
#endif
