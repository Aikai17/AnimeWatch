#pragma once
#include <QtWidgets>
#include "AnimeGO_API.h"
#include "MainWindow.h"

class TitleButton:public QPushButton{
	Q_OBJECT
public:
	TitleButton(animego::title& _title,MainWindow* mainWindow,QWidget* _parent);
	QSize sizeHint() const override;
private:
	QLabel* lblTitle;
	QHBoxLayout mainLayout;
	QWidget* parent;
	
private:
	animego::title title;
	std::vector<animego::episode> episodes;
private slots:
	void onClick();
signals:
	void titleClicked(std::vector<animego::episode>& ep);
};
