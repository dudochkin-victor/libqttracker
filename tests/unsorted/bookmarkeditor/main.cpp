#include <QtGui/QApplication>

#include <QtCore/QTimer>

#include "bookmarkeditor.h"

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

    BookmarkEditor editor;
    editor.show();

    return app.exec();
}
