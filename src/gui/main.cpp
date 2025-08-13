#include "main_window.h"
#include <QApplication>
#include <QStyleFactory>
#include <QDir>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    app.setApplicationName("IMAP Kanban");
    app.setApplicationVersion("1.0.0");
    app.setApplicationDisplayName("IMAP Kanban");
    app.setOrganizationName("IMAP Kanban Project");
    app.setOrganizationDomain("imap-kanban.org");
    
    // Set application icon (if available)
    // app.setWindowIcon(QIcon(":/icons/app.png"));
    
    // Use a modern style
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Apply a modern color scheme
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    
    // Uncomment to use dark theme
    // app.setPalette(darkPalette);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}