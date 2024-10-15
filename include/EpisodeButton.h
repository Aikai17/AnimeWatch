#pragma once
#include "MainWindow.h"
class EpisodeButton:public QPushButton{
	Q_OBJECT
public:
	EpisodeButton(animego::episode& episode,MainWindow* mainWindow,QWidget* parent);
	int getId(){return id;}
	QSize sizeHint()const override;
private:
	QLabel* lblEpisode;
	QHBoxLayout mainLayout;
	QSize parentSize;
private slots:
	void onClick();	
private:
	std::vector<animego::embed> dubs;
	animego::episode episode;
	int id;
signals:
	void episodeClicked(std::vector<animego::embed>& _dubs);
};
