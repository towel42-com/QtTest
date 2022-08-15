#include "Test.h"
#include "ui_Test.h"
#include "SslOverrideHttp.h"

#include <QUrl>
#include <QTimer>
#include <QMessageBox>
#include <QTime>

CTest::CTest(QWidget *parent)
	: QDialog(parent),
	fHttp( NULL ),
	fID( -1 )
{
	fImpl = new Ui::CTestClass();
	fImpl->setupUi(this);

	fTimeoutTimer = new QTimer( this );
	connect( fImpl->go, SIGNAL( clicked() ), this, SLOT( slotGo() ) );

	fImpl->progressBar->setRange( 0, 100 );
	fImpl->progressBar->setValue( 0 );

	fTimeoutTimer = new QTimer;
	fTimeoutTimer->stop();
	fTimeoutTimer->setInterval( 10*1000 );
	fTimeoutTimer->setSingleShot( true );

	connect( fTimeoutTimer, SIGNAL( timeout() ), this, SLOT( slotTimeoutAbortConnection() ) );
}

CTest::~CTest()
{
	delete fImpl;
}


void CTest::slotGo()
{
	QUrl url( fImpl->url->text() );
	if ( !url.isValid() )
	{
		QMessageBox::warning( this, "Invalid url", tr( "%1 is not valid" ).arg( fImpl->url->text() ) );
		return;
	}
	fImpl->go->setEnabled( false );
	fImpl->results->clear();

	fHttp = new QHttp;

	QString scheme = url.scheme();
	QHttp::ConnectionMode mode = url.scheme() == "http" ? QHttp::ConnectionModeHttp : QHttp::ConnectionModeHttps;
	fHttp->setHost( url.host(), mode, url.port() == -1 ? 0 : url.port() );

	QString path = url.path();
	if ( !url.encodedQuery().isEmpty())
		path += "?" + url.encodedQuery();

	appendPlainText( url.toString() );

	connect( fHttp, SIGNAL( responseHeaderReceived( const QHttpResponseHeader & ) ), this, SLOT( slotResponseHeader( const QHttpResponseHeader & ) ) );
	connect( fHttp, SIGNAL( requestFinished( int, bool ) ), this, SLOT( slotHttpRequestFinished( int, bool ) ) );
//	connect( fHttp, SIGNAL( dataSendProgress( int, int ) ), fTimeoutTimer, SLOT( start() ) );
	connect( fHttp, SIGNAL( dataReadProgress( int, int ) ), this, SLOT( slotProgress( int, int ) ) );

	//fTimeoutTimer->start();
	fImpl->progressBar->setValue( 0 );
	fID = fHttp->get( path );
}

void CTest::appendPlainText( const QString & text )
{
	fImpl->results->appendPlainText( QTime::currentTime().toString() + " - " + text );
}

void CTest::slotTimeoutAbortConnection()
{
	appendPlainText( "Timeout" );
	if ( fHttp )
		fHttp->abort();
}

void CTest::slotProgress( int value, int max )
{
	fImpl->progressBar->setRange( 0, max );
	fImpl->progressBar->setValue( value );
}

void CTest::slotHttpRequestFinished( int requestId, bool error )
{
	if ( requestId != fID )
		return;

	QString msg;
	if ( error )
	{
		 msg = "Error: " + fHttp->errorString();
	}
	else
	{
		msg = fHttp->readAll();
	}
	appendPlainText( msg );
	fHttp->deleteLater();
	fHttp = NULL;
	fTimeoutTimer->stop();
	fImpl->go->setEnabled( true );
}

void CTest::slotResponseHeader( const QHttpResponseHeader & responseHeader )
{
	if ( responseHeader.statusCode() != 200 )
	{
		CSslOverrideHttp * sslHttp = qobject_cast< CSslOverrideHttp * >( fHttp );
		if ( sslHttp )
		{
			sslHttp->setResponseHeader( responseHeader );
		}

		QString msg;
		
		msg += "url: " + fImpl->url->text() + ": " ;
		msg += QString( "HTTP/%1.%2 %3 %4" )
			.arg( responseHeader.majorVersion() )
			.arg( responseHeader.minorVersion() )
			.arg( responseHeader.statusCode() )
			.arg( responseHeader.reasonPhrase() );
		appendPlainText( msg );

		return;
	}
}
