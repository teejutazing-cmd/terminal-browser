// Licensed under the MIT license.
#pragma once
#include "winrt/TerminalApp.h"
#include "winrt/Microsoft.Web.WebView2.Core.h"
#include "BasicPaneEvents.h"

namespace winrt::TerminalApp::implementation
{
    class BrowserPaneContent : public winrt::implements<BrowserPaneContent, IPaneContent>, public BasicPaneEvents
    {
    public:
        BrowserPaneContent(const winrt::hstring& url,
                           const Microsoft::Terminal::Settings::Model::CascadiaSettings& settings,
                           const winrt::guid& profileGuid);
        Windows::UI::Xaml::FrameworkElement GetRoot() { return _root; }
        void UpdateSettings(const Microsoft::Terminal::Settings::Model::CascadiaSettings& settings);
        Windows::Foundation::Size MinimumSize() { return { 320, 200 }; }
        void Focus(Windows::UI::Xaml::FocusState reason);
        void Close();
        Microsoft::Terminal::Settings::Model::INewContentArgs GetNewTerminalArgs(BuildStartupKind kind) const;
        winrt::hstring Title() const { return _title; }
        uint64_t TaskbarState() { return 0; }
        uint64_t TaskbarProgress() { return 0; }
        bool ReadOnly() { return false; }
        winrt::hstring Icon() const { return L"\xE774"; }
        Windows::Foundation::IReference<Windows::UI::Color> TabColor() const noexcept { return _tabColor; }
        Windows::UI::Xaml::Media::Brush BackgroundBrush() { return _root.Background(); }
        til::typed_event<IPaneContent, winrt::hstring> NewTabRequested;

    private:
        Windows::UI::Xaml::Controls::Grid _root;
        Windows::UI::Xaml::Controls::TextBox _address;
        Microsoft::UI::Xaml::Controls::WebView2 _web;
        winrt::hstring _url;
        winrt::guid _profileGuid;
        Windows::Foundation::IReference<Windows::UI::Color> _tabColor{ nullptr };
        winrt::hstring _title{ L"Browser" };
        bool _closed{ false };
        bool _initializing{ false };
        winrt::fire_and_forget _Initialize();
        void _Navigate();
    };
}
