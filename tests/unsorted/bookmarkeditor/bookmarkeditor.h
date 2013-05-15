#ifndef BOOKMARKEDITOR_H
#define BOOKMARKEDITOR_H

#include <QtGui/QWidget>

#include <sopranolive/live.h>
#include <QtTracker/QLiveAutogenerationBase>

#include "ui_bookmarkeditor.h"

class BookmarkEditor : public QWidget
{
    Q_OBJECT
public:
    BookmarkEditor(QWidget * parent = 0);
    ~BookmarkEditor();

public Q_SLOTS:
    void insertItem();

private:
    Ui::BookmarkEditor m_ui;
    SopranoLive::LiveNodes m_model;
};

#endif // BOOKMARKEDITOR_H
