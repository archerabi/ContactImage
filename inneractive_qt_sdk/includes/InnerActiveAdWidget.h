#ifndef INNERACTIVEADWIDGET_H
#define INNERACTIVEADWIDGET_H

#include <QFrame>
#include <QUrl>

class QLabel;
class QBuffer;

/**
* @brief InnerActiveAdWidget description.
*
* InnerActiveAdWidget (derived from QFrame) class object is created by InnerActiveAdModule::createAdBanner function.
*
*/
class InnerActiveAdWidget : public QFrame
{
    Q_OBJECT

public:
	/**
	* @brief InnerActiveAdWidget constructor.
	* 
	*/
    InnerActiveAdWidget(QWidget* pParent,
						const QByteArray& imageData, 
						const QString& url = QString(), 
						bool showAdText = false,
						const QString& text = QString());
    
    void updateAdData(const QByteArray& imageData, const QString& url, const QString& text = QString());

signals:
	void adLinkActivated(const QUrl& adUrl);
    
protected:
    bool eventFilter(QObject *obj, QEvent *ev);

private slots:
    void onMovieFinished();

private:
// functions
    void fitAdTextFont(int controlHeight);

private:
// class members
    QLabel* ipLabelImage;
    QLabel* ipLabelText;
    QMovie* ipMovie;

    QUrl iAdUrl;
    QString iAdText;
    
    QByteArray* ipImageByteArray;
    QBuffer*	ipImageIoBuffer;

};

#endif // INNERACTIVEADWIDGET_H
