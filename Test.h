#ifndef TEST_H
#define TEST_H

#include <QDialog>
namespace Ui {class CTestClass;};

class CSslOverrideHttp;
class QHttpResponseHeader;
class QHttp;
class QTimer;
class CTest : public QDialog
{
	Q_OBJECT

public:
	CTest(QWidget *parent = 0);
	~CTest();

public slots:
	void slotGo();
	void slotTimeoutAbortConnection();
	void slotHttpRequestFinished( int requestId, bool error );
	void slotResponseHeader( const QHttpResponseHeader & responseHeader );
	void slotProgress( int, int );
private:
	Ui::CTestClass * fImpl;
	void appendPlainText( const QString & text );
	int fID;
	QTimer * fTimeoutTimer;
//	CSslOverrideHttp * fHttp;
	QHttp * fHttp;
};

#endif // TEST_H
