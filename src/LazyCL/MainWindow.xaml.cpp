#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include <Scanner.h>
#include <algorithm>
#include <atomic>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::LazyCL::implementation
{
    int32_t MainWindow::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainWindow::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    winrt::fire_and_forget MainWindow::myButton_Click(
        winrt::Windows::Foundation::IInspectable const& /*sender*/,
        winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*args*/)
    {
        auto strong = get_strong();

        myButton().IsEnabled(false);
        busyRing().IsActive(true);
        summaryText().Text(L"Scanning...");
        resultsList().Items().Clear();

        winrt::apartment_context ui;          // remember the UI thread
        co_await winrt::resume_background();  // work off the UI thread

        std::atomic<bool> cancel{ false };
        auto items = lazy::ScanFolder(lazy::UserTempFolder(), cancel);

        std::uintmax_t total = 0;
        for (auto& i : items) total += i.size;

        std::sort(items.begin(), items.end(),
            [](auto& a, auto& b) { return a.size > b.size; });

        co_await ui;                          // back on the UI thread

        summaryText().Text(winrt::to_hstring(items.size()) + L" files, " +
            winrt::to_hstring(total / (1024 * 1024)) + L" MB found");

        size_t shown = 0;
        for (auto& i : items)
        {
            if (++shown > 200) break;         // show the 200 largest only
            resultsList().Items().Append(winrt::box_value(
                winrt::hstring(i.path.wstring()) + L"  (" +
                winrt::to_hstring(i.size / 1024) + L" KB)"));
        }

        busyRing().IsActive(false);
        myButton().IsEnabled(true);
    }
}