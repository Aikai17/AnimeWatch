#include "DubButton.h"

DubButton::DubButton(animego::embed& _dub,MainWindow* mainWindow,QWidget* parent):QPushButton(parent),dub(_dub){
	parentSize=parent->size();
	lblDub=new QLabel(this);
	lblDub->setText(QString::fromStdString(dub.dub_name)+" ("+QString::fromStdString(dub.player_name)+")");
	lblDub->setFixedHeight(20);
	mainLayout.addWidget(lblDub);
	setLayout(&mainLayout);

	connect(this,&DubButton::clicked,this,&DubButton::onClick);
	connect(this,&DubButton::dubClicked,mainWindow,&MainWindow::onDubClick);
}
void DubButton::onClick(){
	emit dubClicked(dub.link);
}
QSize DubButton::sizeHint() const{
	int scrollBarWidth=qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
	int width=parentSize.width()-20-scrollBarWidth;
	int height=lblDub->height()+20;
	QSize preferredSize(width,height);
	return preferredSize;
}
