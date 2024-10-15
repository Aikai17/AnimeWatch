#pragma once
#include "MainWindow.h"

class DubButton:public QPushButton{
	Q_OBJECT
public:
	DubButton(animego::embed& _dub,MainWindow* mainWindow,QWidget* parent);
	std::string getText(){ return dub.dub_name;}
private: 
	QLabel* lblDub;
	QHBoxLayout mainLayout;
	QSize parentSize;
	QSize sizeHint()const override;
private slots:
	void onClick();
private:
	std::string link;
	animego::embed dub;
signals:
	void dubClicked(std::string& embedLink);
};
