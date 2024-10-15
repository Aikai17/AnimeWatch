#include "TitleButton.h"
TitleButton::TitleButton(animego::title& _title,MainWindow* mainWindow,QWidget* _parent):QPushButton(_parent),parent(_parent),title(_title){
	lblTitle=new QLabel(this);
	lblTitle->setText(QString::fromStdString(title.date)+" "+QString::fromStdString(title.name)+"("+QString::fromStdString(title.type)+")");
	lblTitle->setFixedHeight(20);
	mainLayout.addWidget(lblTitle);
	setLayout(&mainLayout);

	connect(this,&TitleButton::clicked,this,&TitleButton::onClick);
	connect(this,&TitleButton::titleClicked,mainWindow,&MainWindow::onTitleClick);
}
void TitleButton::onClick(){
	episodes=animego::get_title_info(title.link);
	emit titleClicked(episodes);
}
QSize TitleButton::sizeHint() const{
	int scrollBarWidth=qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
	int width=parent->width()-20-scrollBarWidth;
	int height=lblTitle->height()+20;
	QSize preferredSize(width,height);
	return preferredSize;
}
