/****************************************************************************
**
** Copyright (C) 2005-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.0, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** In addition, as a special exception, Trolltech, as the sole copyright
** holder for Qt Designer, grants users of the Qt/Eclipse Integration
** plug-in the right for the Qt/Eclipse Integration to link to
** functionality provided by Qt Designer and its related libraries.
**
** Trolltech reserves all rights not expressly granted herein.
** 
** Trolltech ASA (c) 2007
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>

#include "OpenFileExDlg.h"
#include "fitsio.h"
#include "math.h"


COpenFileExDlg::COpenFileExDlg(QWidget *parent)
    : QDialog(parent)
{
    browseButton = createButton(tr("&Browse..."), SLOT(browse()));
    findButton = createButton(tr("&Open"), SLOT(find()));
	rate = new QLineEdit(this);

    fileComboBox = createComboBox(tr(""));
	fileExComboBox = createComboBox(tr(""));
 
    fileLabel = new QLabel(tr("Named:"));
	
	fileExLabel = new QLabel(tr("Ex Named:"));
 
//    createFilesTable();

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(findButton);
/*	
	QHBoxLayout *rbuttonsLayout = new QHBoxLayout;
    rbuttonsLayout->addStretch();
	rButton0 = new QRadioButton(tr("640x480"));
	rButton1 = new QRadioButton(tr("1024x768"));
	rButton0->setChecked(true);
    rbuttonsLayout->addWidget(rButton0);
	rbuttonsLayout->addWidget(rButton1);
	*/

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(fileLabel, 0, 0, Qt::AlignJustify);
    mainLayout->addWidget(fileComboBox, 0, 1);
	mainLayout->addWidget(browseButton, 0, 2);
	
	mainLayout->addWidget(fileExLabel, 1, 0);
    mainLayout->addWidget(fileExComboBox, 1, 1);
	
	mainLayout->addWidget(rate, 2, 1);
	
//	mainLayout->addWidget(findButton, 2, 0);

//	mainLayout->addWidget(filesFoundLabel, 4, 0);
//    mainLayout->addLayout(rbuttonsLayout, 2, 0, 1, 3);
	mainLayout->addLayout(buttonsLayout, 3, 0, 1, 3);
   
    setLayout(mainLayout);
	
	
	connect(findButton, SIGNAL(clicked()), SLOT(accept()));

    setWindowTitle(tr("Open File Ex"));
    resize(700, 100);
}

void COpenFileExDlg::browse()
{
    QString FName = QFileDialog::getOpenFileName(this, tr("Choose files"), tr("d:\\berezhnoy\\develop\\ccd\\"), "CCD (*.img *.fit *.fits *.fts)");
    
	QString FExName;
	
    if (FName.count()!=0)
    {
		fileComboBox->setItemText(0, FName);
	
		FExName = FName.left(FName.lastIndexOf("."));
		FExName.append("_ex.fits");
		fileExComboBox->setItemText(0, FExName);
    }
}
/*
QStringList COpenFileExDlg::findFiles(const QDir &directory, const QStringList &files,
                              const QString &text)
{
    QProgressDialog progressDialog(this);
    progressDialog.setCancelButtonText(tr("&Cancel"));
    progressDialog.setRange(0, files.size());
    progressDialog.setWindowTitle(tr("Find Files"));

    QStringList foundFiles;

    for (int i = 0; i < files.size(); ++i) {
        progressDialog.setValue(i);
        progressDialog.setLabelText(tr("Searching file number %1 of %2...")
                                    .arg(i).arg(files.size()));
        qApp->processEvents();

        if (progressDialog.wasCanceled())
            break;

        QFile file(directory.absoluteFilePath(files[i]));

        if (file.open(QIODevice::ReadOnly)) {
            QString line;
            QTextStream in(&file);
            while (!in.atEnd()) {
                if (progressDialog.wasCanceled())
                    break;
                line = in.readLine();
                if (line.contains(text)) {
                    foundFiles << files[i];
                    break;
                }
            }
        }
    }
    return foundFiles;
}

void COpenFileExDlg::showFiles(const QDir &directory, const QStringList &files)
{
    for (int i = 0; i < files.size(); ++i) {
        QFile file(directory.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        fileNameItem->setFlags(Qt::ItemIsEnabled);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB")
                                             .arg(int((size + 1023) / 1024)));
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(Qt::ItemIsEnabled);

        int row = filesTable->rowCount();
        filesTable->insertRow(row);
        filesTable->setItem(row, 0, fileNameItem);
        filesTable->setItem(row, 1, sizeItem);
    }
    filesFoundLabel->setText(tr("%1 file(s) found").arg(files.size()));
}
*/
QPushButton *COpenFileExDlg::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

QComboBox *COpenFileExDlg::createComboBox(const QString &text)
{
    QComboBox *comboBox = new QComboBox;
    comboBox->setEditable(true);
    comboBox->addItem(text);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return comboBox;
}
