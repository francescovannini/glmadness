/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

using namespace vcg; 

void MainForm::fileNew()
{
	InitDialog *inid= new InitDialog();
	if(inid->exec()){
			int vx =inid->spinBoxXSize->value();
			int vy =inid->spinBoxYSize->value();	
			sg.g->resize(vx,vy);
			QString buf;
			textGridSize->setText(buf.sprintf("Grid size %i x %i",vx,vy));
			}

}


void MainForm::fileOpen()
{
	QString s = QFileDialog::getOpenFileName("../levels/","xml grids (*.xml)",this,"Open file dialog","Choose one level file");
	sg.XMLRead(s);
	GLView->setDirty(true);
	spinboxlevelTime->setValue(sg.g->time);
}


void MainForm::fileSave()
{
	QString s = QFileDialog::getSaveFileName("../levels","xml grids (*.xml)",this,"Save file dialog","Choose destination file" );
	sg.g->time=spinboxlevelTime->value();
	QFile f(s);
	f.open(IO_WriteOnly);
	sg.XMLWrite(QTextStream(&f));
	f.close();
}

void MainForm::fileExit()
{
    exit(0);
}

void MainForm::init()
{
	idleTimer.changeInterval(0);
	connect(&idleTimer, SIGNAL(timeout()),GLView,SLOT(update()));
	initSG();	
}




void MainForm::helpAbout()
{
    QMessageBox::about(this,"About GLMadness Level Editor","Written by F.Vannini for CI2004\nBased on skeleton code by P.Cignoni");
}

void MainForm::editGridSize()
{
    int ret=QMessageBox::question(this,"Changing ground size","Do you really want to change the size of the ground? It will destroy the geometry!",QMessageBox::Yes,QMessageBox::Cancel);
    if(ret==QMessageBox::Yes) fileNew();

}

void MainForm::chooseCubeMap()
{
	QString s = QFileDialog::getOpenFileName("../data/cubemaps/","Images (*.png *.jpg)",this,"Open file dialog","Choose one of cube map images (assuming *_negx.jpg *_negx.png format)");
	// now strip the ending part of the cubemap name.
		QString extn=s.right(4);
		QString base=s.left(s.length()-9);
		QString name=base+extn;
		sg.c->textureBaseName=name;
		editCubeMap->setText(name);
	setCubeMap();
 }



void MainForm::setCubeMap()
{
 QString nm=editCubeMap->text();
 sg.c->textureBaseName=nm;
 sg.c->dirty=true;

}


void MainForm::initSG()
{
    GLView->sg=&sg;
	sg.XMLRead("../levels/default.xml");
	spinboxlevelTime->setValue(sg.g->time);
	GLView->setDirty(true);
}

void MainForm::setBallPos()
{
	sg.g->setBallOnSelectionCenter();
	sg.g->generateFace();
	GLView->animBall=true;
}

void MainForm::smooth()
{
    sg.g->smoothSelected();   
}


void MainForm::setAsStart()
{
    sg.g->setCellTypeSelection(1);
}


void MainForm::setAsFinish()
{
    sg.g->setCellTypeSelection(2);
}


void MainForm::setAsIce()
{
    sg.g->setCellTypeSelection(4);
}


void MainForm::setAsGlue()
{
    sg.g->setCellTypeSelection(3);
}


void MainForm::vSmooth()
{
    sg.g->smoothVerticalSelected();
}


void MainForm::hSmooth()
{
    sg.g->smoothHorizontalSelected();
}


void MainForm::resetTrackBall()
{
    GLView->resetTB();
}


void MainForm::flatten()
{
    sg.g->flattenSelected();
}
