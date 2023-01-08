#include <qapplication.h>
#include "mainform.h"

int main( int argc, char ** argv )
{
    QApplication a(argc, argv);
    MainForm w;
    w.show();

    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
