// Licensed under the MIT license.
#include "pch.h"
#include "BrowserPaneContent.h"
#include "BrowserUrl.h"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Microsoft::Terminal::Settings::Model;
using namespace winrt::Microsoft::Web::WebView2::Core;

namespace winrt::TerminalApp::implementation
{
    BrowserPaneContent::BrowserPaneContent(const winrt::hstring& url,
                                           const CascadiaSettings& settings,
                                           const winrt::guid& profileGuid) :
        _url(terminal_browser::NormalizeUrl(url)),
        _profileGuid(profileGuid)
    {
        if (_url.empty())
        {
            _url = L"https://example.com";
        }
        UpdateSettings(settings);
        RowDefinition addressRow;
        addressRow.Height({ 1, GridUnitType::Auto });
        _root.RowDefinitions().Append(addressRow);
        _root.RowDefinitions().Append(RowDefinition{});
        _address.Text(_url);
        _address.PlaceholderText(L"Enter a URL and press Enter");
        _address.Margin({ 8, 6, 8, 6 });
        _address.ContextFlyout(Microsoft::Terminal::UI::TextMenuFlyout{});
        Automation::AutomationProperties::SetName(_address, L"Address");
        _address.KeyDown([weak = get_weak()](const auto&, const Input::KeyRoutedEventArgs& e) {
            if (e.Key() == Windows::System::VirtualKey::Enter)
            {
                if (const auto self = weak.get())
                {
                    self->_Navigate();
                    e.Handled(true);
                }
            }
        });
        Grid::SetRow(_web, 1);
        _root.Children().Append(_address);
        _root.Children().Append(_web);
        Input::KeyboardAccelerator addressShortcut;
        addressShortcut.Key(Windows::System::VirtualKey::L);
        addressShortcut.Modifiers(Windows::System::VirtualKeyModifiers::Control);
        addressShortcut.Invoked([weak = get_weak()](const auto&, const Input::KeyboardAcceleratorInvokedEventArgs& args) {
            if (const auto self = weak.get(); self && !self->_closed)
            {
                self->_address.Focus(FocusState::Keyboard);
                self->_address.SelectAll();
                args.Handled(true);
            }
        });
        _root.KeyboardAccelerators().Append(addressShortcut);
        _root.Loaded([weak = get_weak()](const auto&, const auto&) {
            if (const auto self = weak.get())
            {
                self->_Initialize();
            }
        });
    }

    void BrowserPaneContent::UpdateSettings(const CascadiaSettings& settings)
    {
        const auto profile = settings.FindProfile(_profileGuid);
        const auto appearance = TerminalSettings::CreateWithProfile(
            settings, profile ? profile : settings.ProfileDefaults(), nullptr).DefaultSettings();

        // Feed the original tab theme's "terminalBackground" color from the
        // same profile/scheme resolver used by TermControl, without a backend.
        _root.Background(Media::SolidColorBrush{ til::color{ appearance.DefaultBackground() } });
        _tabColor = nullptr;
        if (const auto color = appearance.TabColor())
        {
            _tabColor = static_cast<Windows::UI::Color>(til::color{ color.Value() });
        }
        TabColorChanged.raise(*this, nullptr);
    }

    winrt::fire_and_forget BrowserPaneContent::_Initialize()
    {
        const auto lifetime = get_strong();
        if (_initializing || _closed)
        {
            co_return;
        }
        _initializing = true;
        try
        {
            co_await _web.EnsureCoreWebView2Async();
            if (_closed)
            {
                co_return;
            }
            const auto core = _web.CoreWebView2();
            core.DocumentTitleChanged([weak = get_weak()](const auto& sender, const auto&) {
                if (const auto self = weak.get(); self && !self->_closed)
                {
                    self->_title = sender.DocumentTitle().empty() ? L"Browser" : sender.DocumentTitle();
                    self->TitleChanged.raise(*self, nullptr);
                }
            });
            core.SourceChanged([weak = get_weak()](const auto& sender, const auto&) {
                if (const auto self = weak.get(); self && !self->_closed)
                {
                    self->_url = sender.Source();
                    self->_address.Text(self->_url);
                }
            });
            core.NewWindowRequested([weak = get_weak()](const auto&, const CoreWebView2NewWindowRequestedEventArgs& args) {
                args.Handled(true);
                if (const auto self = weak.get(); self && !self->_closed && args.IsUserInitiated())
                {
                    const auto url = terminal_browser::NormalizeUrl(args.Uri());
                    if (!url.empty())
                    {
                        self->NewTabRequested.raise(*self, winrt::hstring{ url });
                    }
                }
            });
            core.NavigationStarting([](const auto&, const CoreWebView2NavigationStartingEventArgs& args) {
                if (terminal_browser::NormalizeUrl(args.Uri()).empty())
                {
                    args.Cancel(true);
                }
            });
            core.Navigate(_url);
        }
        catch (const winrt::hresult_error& e)
        {
            if (!_closed)
            {
                _title = L"Browser initialization failed";
                _address.Text(L"WebView2: " + e.message());
                TitleChanged.raise(*this, nullptr);
            }
        }
    }

    void BrowserPaneContent::_Navigate()
    {
        const auto url = terminal_browser::NormalizeUrl(_address.Text());
        if (url.empty())
        {
            _address.SelectAll();
            return;
        }
        try
        {
            // URI validation happens before changing the current page.
            const Uri parsed{ url };
            _url = parsed.AbsoluteUri();
            if (const auto core = _web.CoreWebView2())
            {
                core.Navigate(_url);
                _web.Focus(FocusState::Programmatic);
            }
        }
        catch (const winrt::hresult_error&)
        {
            _address.SelectAll();
        }
    }

    void BrowserPaneContent::Focus(const FocusState reason)
    {
        // The shell calls this during Flyout.Opening. WebView2 transfers focus
        // asynchronously to its native child, which would dismiss that flyout.
        // Keep shell-driven focus in XAML; navigation/page clicks focus the web.
        _address.Focus(reason);
    }

    void BrowserPaneContent::Close()
    {
        if (!_closed)
        {
            _closed = true;
            _web.Close();
        }
    }

    INewContentArgs BrowserPaneContent::GetNewTerminalArgs(const BuildStartupKind /*kind*/) const
    {
        NewTerminalArgs args;
        args.Profile(winrt::to_hstring(_profileGuid));
        args.Commandline(_url);
        return args;
    }
}
