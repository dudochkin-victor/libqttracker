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
 * email.h
 *
 *  Created on: Jan 13, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef EMAIL_H_
#define EMAIL_H_
#define number_of_lines_save 16
#define number_of_lines_load 9

#include <QtGui>
#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nmo.h>

#include <QString>
#include <sys/time.h>

using namespace SopranoLive;
using namespace SopranoLive::Ontologies;

class EmailViewer
{
public:

	QTime timer;
	int time_stats_save[number_of_lines_save][2];
	int time_stats_load[number_of_lines_load][2];

	EmailViewer();

public:
	void refreshEmails(bool isLive = false);
	void refreshEmails();
	void liveEmails();
	void setEmailId(/*const QModelIndex &item*/);
	void loadEmail(char *email_id);
	void saveEmail(char *email_id);
};

#endif /* EMAIL_H_ */
