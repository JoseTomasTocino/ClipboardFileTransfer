/**
 ** This file is part of the DelfosUI project.
 **
 ** Copyright (C) 2020 NAVANTIA Sistemas
 **
 ** This is propietary software. Unauthorized copying of this file
 ** via any medium is strictly prohibited. Propietary and confidential.
 **
 ** Authors:
 **   abr. 2020 - José Tomás Tocino García <elimco.jttocinog@navantia.es>
 **/

#include "Widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    return a.exec();
}
