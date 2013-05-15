/*
 * This file is part of LibQtTracker project
 *
 * Copyright (C) 2009, Nokia
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
/*
 * main.cpp
 *
 *  Created on: Jan 9, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#include "email.h"


EmailViewer::EmailViewer()
{

	for (int i = 0; i < number_of_lines_save; i++)
	{
		time_stats_save[i][0] = 0;
		time_stats_save[i][1] = 0;
	}

	for (int i = 0; i < number_of_lines_load; i++)
	{
		time_stats_load[i][0] = 0;
		time_stats_load[i][1] = 0;
	}

	for (int i = 0; i < 10; i++)
	{
		char email_id[32];
		sprintf(email_id, "%s%u", "http://www.ivan.com/ivan#", i);
		saveEmail(email_id);
		loadEmail(email_id);
	}

	char save_code_list[number_of_lines_save][128] =
	{
		"Live<nmo::Email> email = ::tracker()->liveNode(QUrl(QString::QString(email_id)));",
		"LiveNodes headernodes = email->getMessageHeaders();",
		"Live<nmo::MessageHeader> header;",
		"header = headernodes.front();",
		"header = email->addMessageHeader();",
		"header->setHeaderName(QString::QString(\"Header iri.\"));",
		"header->setHeaderValue(QString::QString(\"Message header.\"));",
		"email->addMessageHeader(header);",
		"Live<nco::Contact> sender = email->getFrom();",
		"sender->setFullname(QString::QString(\"Iván Arias Rodríguez\"));",
		"QStringList recipient_stringlist = QString::QString(\"ivan.arias-rodriguez@nokia.com\").split(\";\");",
		"QList<LiveNodes> recipient_list;",
		"recipient_list << LiveNode(*ri);",
		"email->setRecipients(recipient_list);",
		"email->setMessageSubject(QString::QString(\"This is the subject.\"));",
		"email->setPlainTextMessageContent(QString::QString(\"This is the message.\"));"
	};

	char load_code_list[number_of_lines_load][128] =
	{
		"Live<nmo::Email> email = ::tracker()->liveNode(QUrl(QString::QString(email_id)));",
		"LiveNodes headernodes = email->getMessageHeaders();",
		"Live<nmo::MessageHeader> header;",
		"header = headernodes.front();",
		"header = email->addMessageHeader();",
		"Live<nco::Contact> sender = email->getFrom();",
		"LiveNodes recipient_list = email->getRecipients();",
		"QStringList recipient_string;",
		"recipient_string << Live<nco::Contact>(*ri)->getFullname();"
	};


	qDebug("\nvoid EmailViewer::saveEmail()");
	for (int time_stats_save_counter = 0; time_stats_save_counter < number_of_lines_save; time_stats_save_counter++)
	{
		if (time_stats_save[time_stats_save_counter][1] > 0)
		{
			qDebug("Line %u:\n%s\nTimes executed %u, Total Time %d ms, Average time %d ms.\n", time_stats_save_counter, save_code_list[time_stats_save_counter], time_stats_save[time_stats_save_counter][1], time_stats_save[time_stats_save_counter][0], time_stats_save[time_stats_save_counter][0]/time_stats_save[time_stats_save_counter][1]);
		}
	}

	qDebug("\nvoid EmailViewer::loadEmail()");
	for (int time_stats_load_counter = 0; time_stats_load_counter < number_of_lines_load; time_stats_load_counter++)
	{
		if (time_stats_load[time_stats_load_counter][1] > 0)
		{
			qDebug("Line %u:\n%s\nTimes executed %u, Total Time %d ms, Average time %d ms.\n", time_stats_load_counter, load_code_list[time_stats_load_counter], time_stats_load[time_stats_load_counter][1], time_stats_load[time_stats_load_counter][0], time_stats_load[time_stats_load_counter][0]/time_stats_load[time_stats_load_counter][1]);
		}
	}
}

void EmailViewer::refreshEmails()
{
//	RDFSelect select;
//	select.addColumn("Email_iri", RDFVariable::fromType<nmo::Email>());
//	query_model = ::tracker()->modelQuery(select);
//	query_view->setModel(query_model.data());
}

void EmailViewer::setEmailId(/*const QModelIndex &item*/)
{
//	email_id->setText(item.data().toString());
}
void EmailViewer::loadEmail(char *email_id)
{
	timer.restart();
//	Live<nmo::Email> email = ::tracker()->liveNode(QUrl(email_id->text()));
	Live<nmo::Email> email = ::tracker()->liveNode(QUrl(QString::QString(email_id)));
	time_stats_load[0][0] += timer.restart();
	time_stats_load[0][1]++;

	LiveNodes headernodes = email->getMessageHeaders();
	time_stats_load[1][0] += timer.restart();
	time_stats_load[1][1]++;

//	qDebug() << headernodes.size();
	Live<nmo::MessageHeader> header;
	time_stats_load[2][0] += timer.restart();
	time_stats_load[2][1]++;

	if(headernodes.nodes().size())
	{
		timer.restart();
		header = headernodes.nodes().front();
		time_stats_load[3][0] += timer.restart();
		time_stats_load[3][1]++;
	}
	else
	{
		timer.restart();
		header = email->addMessageHeader();
		time_stats_load[4][0] += timer.restart();
		time_stats_load[4][1]++;
	}

//	headername->setText(header->getHeaderName());
//	headers->setPlainText(header->getHeaderValue());

	Live<nco::Contact> sender = email->getFrom();
	time_stats_load[5][0] += timer.restart();
	time_stats_load[5][1]++;
//	from->setText(sender->getFullname());

	LiveNodes recipient_list = email->getRecipients();
	time_stats_load[6][0] += timer.restart();
	time_stats_load[6][1]++;

	QStringList recipient_string;
	time_stats_load[7][0] += timer.restart();
	time_stats_load[7][1]++;

	for(LiveNodes::const_iterator ri = recipient_list.constBegin(); ri != recipient_list.constEnd(); ++ri)
	{
		timer.restart();
		recipient_string << Live<nco::Contact>(*ri)->getFullname();
		time_stats_load[8][0] += timer.restart();
		time_stats_load[8][1]++;
	}
//	recipients->setText(recipient_string.join("; "));


//	subject->setText(email->getMessageSubject());
//	body->setPlainText(email->getPlainTextMessageContent());
}

void EmailViewer::saveEmail(char *email_id)
{
	timer.restart();
//	Live<nmo::Email> email = ::tracker()->liveNode(QUrl(email_id->text()));
	Live<nmo::Email> email = ::tracker()->liveNode(QUrl(QString::QString(email_id)));
	time_stats_save[0][0] += timer.restart();
	time_stats_save[0][1]++;

	LiveNodes headernodes = email->getMessageHeaders();
	time_stats_save[1][0] += timer.restart();
	time_stats_save[1][1]++;

	Live<nmo::MessageHeader> header;
	time_stats_save[2][0] += timer.restart();
	time_stats_save[2][1]++;

	if(headernodes.nodes().size())
	{
		timer.restart();
		header = headernodes.nodes().front();
		time_stats_save[3][0] += timer.restart();
		time_stats_save[3][1]++;
	}
	else
	{
		timer.restart();
		header = email->addMessageHeader();
		time_stats_save[4][0] += timer.restart();
		time_stats_save[4][1]++;
	}
//	header->setHeaderName(headername->text());
	header->setHeaderName(QString::QString("Header iri."));
	time_stats_save[5][0] += timer.restart();
	time_stats_save[5][1]++;

//	header->setHeaderValue(headers->toPlainText());
	header->setHeaderValue(QString::QString("Message header."));
	time_stats_save[6][0] += timer.restart();
	time_stats_save[6][1]++;

	email->addMessageHeader(header);
	time_stats_save[7][0] += timer.restart();
	time_stats_save[7][1]++;

	Live<nco::Contact> sender = email->getFrom();
	time_stats_save[8][0] += timer.restart();
	time_stats_save[8][1]++;

//	sender->setFullname(from->text());
	sender->setFullname(QString::QString("Iván Arias Rodríguez"));
	time_stats_save[9][0] += timer.restart();
	time_stats_save[9][1]++;

//	QStringList recipient_stringlist = recipients->text().split(";");
	QStringList recipient_stringlist = QString::QString("ivan.arias-rodriguez@nokia.com").split(";");
	time_stats_save[10][0] += timer.restart();
	time_stats_save[10][1]++;

	QList<LiveNode> recipient_list;
	time_stats_save[11][0] += timer.restart();
	time_stats_save[11][1]++;

	for(QStringList::const_iterator ri = recipient_stringlist.constBegin(); ri != recipient_stringlist.constEnd(); ++ri)
	{
		timer.restart();
		recipient_list << LiveNode(*ri);
		time_stats_save[12][0] += timer.restart();
		time_stats_save[12][1]++;
	}

	email->setRecipients(recipient_list);
	time_stats_save[13][0] += timer.restart();
	time_stats_save[13][1]++;

//	email->setMessageSubject(subject->text());
	email->setMessageSubject(QString::QString("This is the subject."));
	time_stats_save[14][0] += timer.restart();
	time_stats_save[14][1]++;

//	email->setPlainTextMessageContent(body->toPlainText());
	email->setPlainTextMessageContent(QString::QString("This is the message."));
	time_stats_save[15][0] += timer.restart();
	time_stats_save[15][1]++;

//	refreshEmails();
}

int main(int argc, char *argv[])
{
    EmailViewer view;
}
