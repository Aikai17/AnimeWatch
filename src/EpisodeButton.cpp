#include  "EpisodeButton.h"

EpisodeButton::EpisodeButton(animego::episode& _episode,MainWindow* mainWindow,QWidget* parent):QPushButton(parent),episode(_episode){
	lblEpisode=new QLabel(this);
	lblEpisode->setText(QString::fromStdString(episode.seria)+" "+QString::fromStdString(episode.title)+" ("+QString::fromStdString(episode.date)+")");
	lblEpisode->setFixedHeight(20);	
	mainLayout.addWidget(lblEpisode);
	setLayout(&mainLayout);	
	
	connect(this,&EpisodeButton::clicked,this,&EpisodeButton::onClick);
	connect(this,&EpisodeButton::episodeClicked,mainWindow,&MainWindow::onEpisodeClick);
}
void EpisodeButton::onClick(){
	size_t pos=episode.seria.find(' ');
	if(pos!=std::string::npos){
		episode.seria.erase(pos,episode.seria.size()-pos);
	}
	dubs=animego::get_embed_link(episode.id,episode.seria);
	emit episodeClicked(dubs);
}
QSize EpisodeButton::sizeHint() const{
	int scrollBarWidth=qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
	int width=parentSize.width()-20-scrollBarWidth;
	int height=lblEpisode->height()+20;
	QSize preferredSize(width,height);
	return preferredSize;
}
