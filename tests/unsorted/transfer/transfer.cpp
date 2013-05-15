#include "transfer.h"
#include <assert.h>

TransferViewer::TransferViewer()
{
    QWidget *query_area, *left_manage, *right_public;

	addWidget(query_area = new QWidget(this));
	addWidget(left_manage = new QWidget(this));

    QBoxLayout *layout;
    query_area->setLayout(layout = new QVBoxLayout(query_area));
    layout->addWidget(query_view = new QListView);
    query_view->setSelectionMode(QAbstractItemView::SingleSelection);
    QObject::connect(query_view, SIGNAL(doubleClicked(const QModelIndex &)),
                     this, SLOT(loadTransfer(const QModelIndex &)));

    layout->addWidget(refresh_query_button = new QPushButton("Refresh Transfers"));
    QObject::connect(refresh_query_button, SIGNAL(clicked()), 
                     this, SLOT(refreshTransferList()));

    left_manage->setLayout(layout = new QVBoxLayout);

    QBoxLayout *inner_layout;
    layout->addLayout(inner_layout = new QHBoxLayout);
    inner_layout->addWidget(new QLabel("Transfer iri: "));
    inner_layout->addWidget(transfer_id = new QLineEdit);

    layout->addWidget(accounts = new QGroupBox("Accounts"));
    accounts->setLayout(inner_layout = new QVBoxLayout);
    inner_layout->addWidget(facebook = new QRadioButton("Facebook"));
    inner_layout->addWidget(youtube = new QRadioButton("YouTube"));
    inner_layout->addWidget(flickr = new QRadioButton("Flickr"));
    facebook->setChecked(true);
    youtube->setChecked(false);
    flickr->setChecked(false);

    layout->addWidget(new QLabel("File list"));
    layout->addWidget(selected_files = new QListWidget);

    layout->addLayout(inner_layout = new QHBoxLayout);
    inner_layout->addWidget(reset_file_list_button = new QPushButton("Reset file list"));
    QObject::connect(reset_file_list_button, SIGNAL(clicked()), 
                     selected_files, SLOT(clear()));

    inner_layout->addWidget(add_files_button = new QPushButton("Add files"));
    QObject::connect(add_files_button, SIGNAL(clicked()), 
                     this, SLOT(selectFiles()));

    layout->addWidget(save_button = new QPushButton("Save Transfer"));
    QObject::connect(save_button, SIGNAL(clicked()), 
                     this, SLOT(saveTransfer()));

    layout->addWidget(delete_button = new QPushButton("Delete Transfer"));
    QObject::connect(delete_button, SIGNAL(clicked()), 
                     this, SLOT(deleteTransfer()));

    SopranoLive::BackEnds::Tracker::setTrackerVerbosity(3);
    refreshTransferList();
}

void
TransferViewer::refreshTransferList()
{
    SopranoLive::BackEnds::Tracker::setTrackerVerbosity(1);
    RDFSelect select;
    select.newColumn("Transfer_iri") = RDFVariable::fromType<mto::Transfer>();
    query_model = ::tracker()->modelQuery(select);
    query_view->setModel(query_model.model());
    SopranoLive::BackEnds::Tracker::setTrackerVerbosity(3);
}

void
TransferViewer::loadTransfer(const QModelIndex &item)
{

    loadTransfer(item.data().toString());
}

void
TransferViewer::loadTransfer(const QString &item_iri)
{
    QString transfer_id_text = item_iri;
    //transfer_id_text.remove(0, strlen("transfer://transfer.com/#"));
    transfer_id->setText(transfer_id_text);

    Live<mto::Transfer> transfer = ::tracker()->liveNode(QUrl(item_iri));
    Live<mto::TransferMethod> method = transfer->getMethod();
    qDebug() << method.toString();

    selected_files->clear();
    foreach(Live<mto::TransferElement> ri, transfer->getTransferLists().nodes())
    {
        Live<rdfs::Resource> source = ri->getSource();
        QString file_name = source.toString();
        file_name.remove(0, 7); // For "file://"
        qDebug() << "Adding file: " << file_name;
        selected_files->addItem(file_name);
    }

    QString account = transfer->getAccount();
    if(account == "facebook")
    {
        facebook->setChecked(true);
        youtube->setChecked(false);
        flickr->setChecked(false);
    }
    else if(account == "youtube")
    {
        facebook->setChecked(false);
        youtube->setChecked(true);
        flickr->setChecked(false);
    }
    else if(account == "flickr")
    {
        facebook->setChecked(false);
        youtube->setChecked(false);
        flickr->setChecked(true);
    }
    else 
    {
        qDebug() << "Invalid account: " << account;
        facebook->setChecked(true);
        youtube->setChecked(false);
        flickr->setChecked(false);
    }
}

void
TransferViewer::filesSelected(const QStringList &selected)
{
    selected_files->addItems(selected);
}

void
TransferViewer::selectFiles()
{
    QStringList list;

    list.push_back(QString("/home/divya/Pictures/s561332551_684238_9037.jpg"));
    list.push_back(QString("/home/divya/Pictures/s561332551_1086990_8945.jpg"));
    list.push_back(QString("/home/divya/Pictures/s561332551_684223_4952.jpg"));
    list.push_back(QString("/home/divya/Pictures/s561332551_684225_5228.jpg"));
    list.push_back(QString("/home/divya/Pictures/s561332551_1086893_4928.jpg"));
    list.push_back(QString("/home/divya/Pictures/s561332551_953595_2261.jpg"));
    list.push_back(QString("/home/divya/Pictures/LSR3.jpg"));
    list.push_back(QString("/home/divya/Pictures/s561332551_684209_8842.jpg"));
    list.push_back(QString("/home/divya/Pictures/s561332551_969994_1591.jpg"));
    list.push_back(QString("/home/divya/Pictures/s561332551_969998_2860.jpg"));
    list.push_back(QString("/home/divya/Pictures/s561332551_970000_4025.jpg"));

    filesSelected(list);
}

void
TransferViewer::deleteTransfer()
{
    QString transfer_iri = transfer_id->text();

    qDebug() << "Deleting " << transfer_iri;

    Live<mto::Transfer> transfer = ::tracker()->liveNode(QUrl(transfer_iri));
    transfer->remove();

    // clearing out the UI
    transfer_id->clear();
    selected_files->clear();
    refreshTransferList();
}


void 
TransferViewer::saveTransfer()
{
    int file_count =  selected_files->count();
    static int count = 0;

    if(file_count == 0)
    {
        qDebug() << "No files in transfer";
        transfer_id->clear();
        return;
    }

    qDebug() << "Adding a transfer with " << file_count << " files";

    QString transfer_iri("transfer://transfer.com/#");
    if(transfer_id->text().isEmpty())
    {
        QString num;
        num.setNum(++count);
        transfer_iri.append(num);
    }
    else if(transfer_id->text().startsWith(transfer_iri) == false)
    {
        transfer_iri.append(transfer_id->text());
    }
    else
    {
        transfer_iri = transfer_id->text();
    }

    Live<mto::UploadTransfer> transfer = ::tracker()->liveNode(QUrl(transfer_iri));
    transfer_id->clear();

    Live<mto::TransferMethod> method_web = 
        ::tracker()->liveNode(mto::iri("transfer-method-web"));
    transfer->setMethod(method_web);
    qWarning() << transfer->getMethod().toString();
    transfer->setCreated(QDateTime::currentDateTime());

    Live<mto::State> state_paused = ::tracker()->liveNode(mto::iri("state-pending"));

    for(int i = 0; i < file_count; i++)
    {
        QListWidgetItem *item = selected_files->item(i);
        QString file_name = item->text();
        file_name.prepend("file://");
        Live<mto::TransferElement> transferElem = transfer->addTransferList();
        Live<nfo::Media> mediaFile = ::tracker()->liveNode(QUrl(file_name));
        qDebug() << "Mime type of " << file_name << " is " << mediaFile->getMimeType();
        transferElem->setSource(mediaFile);
        qDebug() << transferElem->getSource().toString();
        transferElem->setState(state_paused);
        /*
         * Fields destination, startedTime and completedTime will be filled
         * later when the transfer actually starts/ends
         */
    }

    selected_files->clear();

    if(facebook->isChecked())
        transfer->setAccount("facebook");  
    else if(youtube->isChecked())
        transfer->setAccount("youtube");  
    else if(flickr->isChecked())
        transfer->setAccount("flickr");  
    else 
        // Should never happen
        assert(0);

    /* TODO
     * tracker->setStarter
     * tracker->setAgent
     */

    transfer->setTitle("transfer");

    refreshTransferList();
    loadTransfer(transfer_iri);
}

TransferViewer::~TransferViewer()
{
    SopranoLive::BackEnds::Tracker::setTrackerVerbosity(1);
}


int 
main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TransferViewer view;
    view.show();

    return app.exec();
}

