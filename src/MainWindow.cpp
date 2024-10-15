#include "MainWindow.h"
#include "TitleButton.h"
#include "EpisodeButton.h"
#include "DubButton.h"
#include "Kodik/include/Kodik_API.h"

MainWindow::MainWindow(QWidget* parent):QMainWindow(parent){
	QWidget* centralWidget=new QWidget(this);
	containersStorage=new QStackedWidget(this);
	searchArea=new QScrollArea(this);
	searchEdit=new QLineEdit(this);
	searchEdit->setPlaceholderText("Поиск...");
	mainLayout=new QGridLayout;
	
	searchLayout=new QVBoxLayout;
	searchLayoutContainer=new QWidget(searchArea);
	searchLayoutContainer->setLayout(searchLayout);

	episodeLayout=new QVBoxLayout;
	episodeLayoutContainer=new QWidget(searchArea);
	episodeLayoutContainer->setLayout(episodeLayout);

	dubsLayout=new QVBoxLayout;
	dubsLayoutContainer=new QWidget(searchArea);
	dubsLayoutContainer->setLayout(dubsLayout);

	mainLayout->setSizeConstraint(QLayout::SetMinimumSize);
	episodeLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
	searchLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
	dubsLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
	
	mainLayout->addWidget(searchEdit,0,0,1,10);
	mainLayout->addWidget(searchArea,1,0,10,10);	
	QWidget* spacer=new QWidget(this);
	mainLayout->addWidget(spacer,11,0,5,10);

	containersStorage->addWidget(searchLayoutContainer);
	containersStorage->addWidget(episodeLayoutContainer);
	containersStorage->addWidget(dubsLayoutContainer);
	
	centralWidget->setLayout(mainLayout);

	searchArea->setWidget(containersStorage);
	searchArea->setWidgetResizable(true);

	setCentralWidget(centralWidget);
	setupMenu();

	connect(searchEdit,&QLineEdit::returnPressed,this,&MainWindow::onTitleSearch);	
}
void MainWindow::onTitleSearch(){
	clearLayout(searchLayout);
	
	titles=animego::search((searchEdit->text().toStdString()));
	if(titles.size()==0){
		QMessageBox::information(NULL,"Warning","Тайтл не найден");
		return;
	}
	searchLayoutContainer->hide();
	for(size_t i = 0;i<titles.size();i++){
		TitleButton* title=new TitleButton(titles[i],this,searchArea);
		searchLayout->addWidget(title);	
	}
	searchLayoutContainer->show();
	containersStorage->setCurrentWidget(searchLayoutContainer);
	containersStorage->setMinimumSize(searchLayoutContainer->size());
}	
void MainWindow::onTitleClick(vector<animego::episode>& episodes){
	clearLayout(episodeLayout);
	if(episodes.size()==0){
		QMessageBox::information(NULL,"Warning","Тайтл не найден");
		return;
	}	
	searchEdit->clear();
	QPushButton* backBttn=new QPushButton("Назад",searchArea);
	connect(backBttn,&QPushButton::clicked,this,[this](){
			searchEdit->disconnect();
			connect(searchEdit,&QLineEdit::returnPressed,this,&MainWindow::onTitleSearch);
			searchEdit->setPlaceholderText("Поиск...");
			containersStorage->setCurrentWidget(searchLayoutContainer);
			containersStorage->setMinimumSize(searchLayoutContainer->minimumSize());
	});
	episodeLayout->addWidget(backBttn);
	episodeLayoutContainer->hide();
	for(size_t i = 0;i<episodes.size();i++){
		EpisodeButton* episode=new EpisodeButton(episodes[i],this,searchArea);
		episodeLayout->addWidget(episode);
	}
	episodeLayoutContainer->show();
	containersStorage->setCurrentWidget(episodeLayoutContainer);
	containersStorage->setMinimumSize(episodeLayoutContainer->size());
	searchEdit->disconnect();
	connect(searchEdit,&QLineEdit::textChanged,this,&MainWindow::onEpisodeSearch);
	searchEdit->setPlaceholderText("Эпизод...");
}
void MainWindow::onEpisodeSearch(){
	int id=searchEdit->text().toInt();
	QLayoutItem* item;
	for(int i = 0;(item=episodeLayout->itemAt(i));i++){
		item->widget()->show();
	}
	if(id!=0){
		for(int i = episodeLayout->count()-1,j=1;(item=episodeLayout->itemAt(i));i--,j++){
			if(j!=id){
				item->widget()->hide();
			}
		}
	}
}
void MainWindow::onEpisodeClick(vector<animego::embed>& dubs){
	clearLayout(dubsLayout);
	if(dubs.size()==0){
		QMessageBox::information(NULL,"Warning","Дубляжи не найдены");
		return;
	}	
	searchEdit->clear();
	QPushButton* backBttn=new QPushButton("Назад",searchArea);
	connect(backBttn,&QPushButton::clicked,this,[this](){
			searchEdit->disconnect();
			connect(searchEdit,&QLineEdit::returnPressed,this,&MainWindow::onTitleSearch);
			searchEdit->setPlaceholderText("Эпизод...");
			containersStorage->setCurrentWidget(episodeLayoutContainer);
			containersStorage->setMinimumSize(episodeLayoutContainer->size());
	});
	dubsLayout->addWidget(backBttn);
	dubsLayoutContainer->hide();
	for(size_t i = 0;i<dubs.size();i++){
		DubButton* dub=new DubButton(dubs[i],this,searchArea);
		dubsLayout->addWidget(dub);
	}
	dubsLayoutContainer->show();
	containersStorage->setCurrentWidget(dubsLayoutContainer);
	containersStorage->setMinimumSize(dubsLayoutContainer->size());
	searchEdit->disconnect();
	connect(searchEdit,&QLineEdit::textChanged,this,&MainWindow::onDubSearch);
	searchEdit->setPlaceholderText("Дубляж...");	
}

void MainWindow::onDubSearch(){
	QLayoutItem* item;
	std::string searchEditText=searchEdit->text().toStdString();
	for(int i = 0;(item=dubsLayout->itemAt(i));i++){
		DubButton* currDub=qobject_cast<DubButton*>(item->widget());
		if(!currDub)
			continue;
		string currDubText=currDub->getText();
		if(searchEditText.size()==0){
			currDub->show();
			continue;
		}
		if(currDubText.size() < searchEditText.size()){
			currDub->hide();
			continue;
		}
		auto iter=std::search(currDubText.begin(),currDubText.end(),searchEditText.begin(),searchEditText.end(),
				[](unsigned char ch1, unsigned char ch2) {
				return std::tolower(ch1) == std::tolower(ch2);
			}
		);
		if(iter!=currDubText.end()){
			currDub->show();
		}
		else{
			currDub->hide();
		}
	}
}
void MainWindow::onDubClick(string& link){
	bool isKodik=link.find("kodik")!=string::npos?true:false;
	string source;
	if(isKodik){
		source=kodik::kodik_get_dash_link(link);
	}
	else{
		source=animego::aniboom_get_media(link);
	}
	
	QProcess *mpdPlayer=new QProcess(this);
	QStringList stringLists;
	QMessageBox::information(NULL,"",QString::fromStdString(source));
	stringLists<<"--http-header-fields=Accept-Language: ru-RU,Origin: https://aniboom.one,Referer: https://aniboom.one/";
	stringLists<<QString::fromStdString(source);
	mpdPlayer->start("mpv",stringLists);	
	
}
void MainWindow::setupMenu(){
	QMenu* menu=menuBar()->addMenu("&File");
	QAction* exitAct=new QAction(tr("&Exit"),this);
	exitAct->setShortcut(QKeySequence::Quit);
	connect(exitAct,&QAction::triggered,qApp,&QApplication::exit);
	menu->addAction(exitAct);
}
void MainWindow::clearLayout(QLayout* layout){
	QLayoutItem* item;
	while((item=layout->takeAt(0))){
		if(item->layout()){
			clearLayout(item->layout());
			delete item->layout();
		}
		if(item->widget()){
			delete item->widget();
		}
		delete item;
	}
}
