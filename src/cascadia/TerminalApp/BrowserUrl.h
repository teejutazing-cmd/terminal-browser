#pragma once
#include <string>
#include <string_view>
namespace terminal_browser
{
    inline std::wstring NormalizeUrl(std::wstring_view input)
    {
        const auto first = input.find_first_not_of(L" \t\r\n");
        if (first == std::wstring_view::npos)
        {
            return L"https://example.com";
        }
        input = input.substr(first, input.find_last_not_of(L" \t\r\n") - first + 1);
        if (input.find_first_of(L" \t\r\n") != std::wstring_view::npos)
        {
            return {};
        }
        if (input == L"about:blank")
        {
            return std::wstring{ input };
        }
        if (input.starts_with(L"http://") || input.starts_with(L"https://"))
        {
            const auto host = input.substr(input.find(L"://") + 3);
            return host.empty() || host.front() == L'/' ? std::wstring{} : std::wstring{ input };
        }
        if (input.starts_with(L"localhost:") || input.starts_with(L"localhost/") || input == L"localhost" ||
            input.starts_with(L"127.0.0.1:") || input.starts_with(L"127.0.0.1/") || input == L"127.0.0.1")
        {
            return L"http://" + std::wstring{ input };
        }
        // Never turn script, data, file or OS protocol handlers into navigation.
        if (input.find(L':') != std::wstring_view::npos)
        {
            return {};
        }
        return L"https://" + std::wstring{ input };
    }
}
