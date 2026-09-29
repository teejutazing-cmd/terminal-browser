#include "../../src/cascadia/TerminalApp/BrowserUrl.h"
#include <iostream>
#include <cstdlib>

int main()
{
    struct Case { const wchar_t* input; const wchar_t* expected; };
    const Case cases[] = {
        { L"", L"https://example.com" },
        { L"  example.com/path?q=1  ", L"https://example.com/path?q=1" },
        { L"https://example.org/a#b", L"https://example.org/a#b" },
        { L"http://localhost:8765/test", L"http://localhost:8765/test" },
        { L"localhost:8765", L"http://localhost:8765" },
        { L"127.0.0.1:8765/a", L"http://127.0.0.1:8765/a" },
        { L"about:blank", L"about:blank" },
        { L"javascript:alert(1)", L"" },
        { L"file:///C:/Windows/win.ini", L"" },
        { L"data:text/html,test", L"" },
        { L"https://", L"" },
        { L"not a url", L"" },
        { L"example.com\n.evil.test", L"" },
    };
    for (const auto& c : cases)
    {
        const auto actual = terminal_browser::NormalizeUrl(c.input);
        if (actual != c.expected)
        {
            std::wcerr << L"FAIL: " << c.input << L" -> " << actual << L" (wanted " << c.expected << L")\n";
            return EXIT_FAILURE;
        }
    }
    std::cout << "PASS: 13 browser URL cases\n";
}
