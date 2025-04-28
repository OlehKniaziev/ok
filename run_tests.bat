@echo off

for %%f in (.\tests\*.cpp) do (
    (clang++ -fsanitize=undefined -g -std=c++20 -Wall -Wextra -pedantic -Werror -o "%%f.exe" %%f) || goto fail_compilation
    ".\%%f.exe" || goto fail_test_suite
)

exit /B 0

:fail_compilation
echo Compilation failed
exit /B 1
:fail_test_suite
echo Test failed
exit /B 1
