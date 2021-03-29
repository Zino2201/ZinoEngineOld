#include "App.h"
#include "Editor/MainWindow.h"
#include "Editor/ZEEditor.h"
#include <QLabel>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QIcon>
#include <QGraphicsColorizeEffect>
#include "EngineVer.h"
#if ZE_PLATFORM(WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace ze::editor
{

QIcon load_and_invert_standard_icon(QStyle::StandardPixmap type)
{
    QIcon icon = QApplication::style()->standardIcon(type);
    QIcon ret;
    for(auto mode : { QIcon::Normal, QIcon::Disabled, QIcon::Active, QIcon::Selected })
    {
        for(auto state : { QIcon::On, QIcon::Off })
        {
            const auto sizes = icon.availableSizes(mode, state);
            for(const auto size : sizes)
            {
                QImage img = icon.pixmap(size, mode, state).toImage();
                img.invertPixels();
                ret.addPixmap(QPixmap::fromImage(std::move(img)), mode, state);
            }
        }
    }

    return ret;
}

MainWindow::MainWindow() :
    dock_mgr(std::make_unique<ads::CDockManager>())
{
    setWindowTitle(QString::asprintf("ZinoEngine Editor v%i.%i.%i", 
        get_version().major,
        get_version().minor,
        get_version().patch));
    setObjectName("main_window");

    dock_mgr->setStyleSheet("");
    dock_mgr->setFrameShape(QFrame::Shape::NoFrame);
    dock_mgr->setContentsMargins(QMargins(0, 0, 0, 0));
    
    /** Window header */
#if 0
    QWidget* header_widget = new QWidget;
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    header_widget->setContentsMargins(QMargins(0, 0, 0, 0));
    header_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QHBoxLayout* header = new QHBoxLayout;
    header->setContentsMargins(QMargins(5, 5, 5, 5));
    {
        QLabel* label = new QLabel(windowTitle());
        perf_text = new QLabel("FPS: 0 fps (0.00 ms)");
        perf_text->setAlignment(Qt::AlignCenter);

        QHBoxLayout* buttons_layout = new QHBoxLayout;

        QPushButton* minimize_button = new QPushButton;
        minimize_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        minimize_button->setIcon(load_and_invert_standard_icon(QStyle::SP_TitleBarMinButton));
        minimize_button->setObjectName("main_window_border_button");

        QPushButton* maximize_button = new QPushButton;
        maximize_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        maximize_button->setIcon(load_and_invert_standard_icon(QStyle::SP_TitleBarMaxButton));
        maximize_button->setObjectName("main_window_border_button");

        QPushButton* close_button = new QPushButton;
        close_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        close_button->setIcon(load_and_invert_standard_icon(QStyle::SP_TitleBarCloseButton));
        close_button->setObjectName("main_window_border_button_red");


        buttons_layout->addWidget(minimize_button, 0, Qt::AlignRight);
        buttons_layout->addWidget(maximize_button, 0, Qt::AlignRight);
        buttons_layout->addWidget(close_button, 0, Qt::AlignRight);

        header->addWidget(label, 0, Qt::AlignLeft | Qt::AlignVCenter);
        header->addWidget(perf_text, 0, Qt::AlignCenter | Qt::AlignVCenter);
        header->addLayout(buttons_layout, 0);
        header->setAlignment(buttons_layout, Qt::AlignRight);
    }

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setContentsMargins(QMargins(0, 0, 0, 0));
    vbox->addLayout(header, 0);
    vbox->addWidget(dock_mgr.get(), 1000);

    setLayout(vbox);
#else
    perf_text = new QLabel("FPS: 0 fps (0.00 ms)");
    perf_text->setAlignment(Qt::AlignCenter);

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setContentsMargins(QMargins(0, 0, 0, 0));
    vbox->addWidget(perf_text, 0, Qt::AlignCenter | Qt::AlignVCenter);
    vbox->addWidget(dock_mgr.get(), 1000);
    setLayout(vbox);
#endif
}

MainWindow::~MainWindow() = default;

void MainWindow::add_window(OwnerPtr<QWidget> in_widget)
{
    auto dock = windows.emplace_back(new ads::CDockWidget(in_widget->windowTitle(),
        this));
    dock->setFeature(ads::CDockWidget::DockWidgetFeature::DockWidgetFloatable, false);
    dock->setWidget(in_widget);
    dock->setContentsMargins(QMargins(0, 0, 0, 0));
    dock_mgr->addDockWidget(ads::TopDockWidgetArea, dock);
}

bool MainWindow::event(QEvent* event)
{
    switch(event->type())
    {
    case QEvent::WindowActivate:
        EditorApp::get().set_focused(true);
        break;
    case QEvent::WindowDeactivate:
        EditorApp::get().set_focused(false);
        break;
    }

    return QWidget::event(event);
}

bool MainWindow::nativeEvent(const QByteArray& event_type, void* message, long* result)
{
#if ZE_PLATFORM(WINDOWS)
    MSG* msg = static_cast<MSG*>(message);

    if(msg->message == WM_NCHITTEST)
    {
        if(isMaximized())
            return false;

        *result = 0;
        LONG border_width = 8;

        RECT win_rect;
        GetWindowRect(reinterpret_cast<HWND>(winId()), &win_rect);

        short x = msg->lParam & 0x0000FFFF;
        short y = (msg->lParam & 0xFFFF0000) >> 16;

        if(minimumWidth() != maximumWidth())
        {
            /** Left */
            if(x >= win_rect.left && x < win_rect.left + border_width)
                *result = HTLEFT;
            
            /** Right */
            if(x < win_rect.right && x >= win_rect.right - border_width)
                *result = HTRIGHT;
        }

        if(minimumHeight() != maximumHeight())
        {
            /** Top */
            if(y < win_rect.bottom && y >= win_rect.bottom - border_width)
                *result = HTTOP;

            /** Bottom */
            if(y >= win_rect.top && y < win_rect.top + border_width)
                *result = HTBOTTOM;
        }

        if(minimumWidth() != maximumWidth() &&
            minimumHeight() != maximumHeight())
        {
            if(x >= win_rect.left && x < win_rect.left + border_width &&
                y < win_rect.bottom && y >= win_rect.bottom - border_width)
                *result = HTBOTTOMLEFT;

            if (x < win_rect.right && x >= win_rect.right - border_width &&
                y < win_rect.bottom && y >= win_rect.bottom - border_width)
            {
                *result = HTBOTTOMRIGHT;
            }

            if (x >= win_rect.left && x < win_rect.left + border_width &&
                y >= win_rect.top && y < win_rect.top + border_width)
            {
                *result = HTTOPLEFT;
            }

            if (x < win_rect.right && x >= win_rect.right - border_width &&
                y >= win_rect.top && y < win_rect.top + border_width)
            {
                *result = HTTOPRIGHT;
            }
        }

        if (*result != 0)
            return true;

        QWidget* action = QApplication::widgetAt(QCursor::pos());
        if (action == this)
        {
            *result = HTCAPTION;
            return true;
        }
    }
#endif

    return false;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);
	EditorApp::get().exit(0);
}

}