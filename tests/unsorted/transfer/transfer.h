#ifndef _TRANSFER_H_
#define _TRANSFER_H_

#include <QtGui>
#include <QtTracker/Tracker>
#include <QtTracker/ontologies/mto.h>

using namespace SopranoLive;

class TransferViewer : public QSplitter
{
    Q_OBJECT
    public:
        QListView   *query_view;

        QLineEdit    *transfer_id;
        QListWidget  *selected_files;

        QGroupBox    *accounts;
        QRadioButton *facebook, *youtube, *flickr;

        QPushButton *reset_file_list_button, *add_files_button;
        QPushButton *refresh_query_button, *save_button, *delete_button;

        QFileDialog *file_selection;

        TransferViewer();
        ~TransferViewer();

        LiveNodes query_model;

    public Q_SLOTS:
        void refreshTransferList();
        void saveTransfer();
        void deleteTransfer();
        void loadTransfer(const QModelIndex &item);
        void loadTransfer(const QString &item_iri);
        void filesSelected(const QStringList &selected);
        void selectFiles();
};

#endif // _TRANSFER_H_
