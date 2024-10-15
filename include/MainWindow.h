#pragma once
#include <QtWidgets>
#include <vector>
#include "AnimeGO_API.h"

class MainWindow: public QMainWindow{
	Q_OBJECT
public:
	MainWindow(QWidget* parent=nullptr);
private:
	QLineEdit* searchEdit;
	QScrollArea* searchArea;
	QGridLayout* mainLayout;
	QVBoxLayout* searchLayout;
	QVBoxLayout* episodeLayout;
	QVBoxLayout* dubsLayout;
	QWidget* dubsLayoutContainer;
	QWidget* episodeLayoutContainer;
	QWidget* searchLayoutContainer;
	QStackedWidget* containersStorage;
public slots:
	void onTitleClick(std::vector<animego::episode>& ep);
	void onEpisodeClick(std::vector<animego::embed>& dubs);
	void onDubClick(std::string& link);
private slots:
	void onDubSearch();
	void onTitleSearch();
	void onEpisodeSearch();
private:
	void clearLayout(QLayout* layout);
	void setupMenu();
private:
	std::vector<animego::title> titles;
};
