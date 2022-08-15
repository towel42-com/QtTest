#ifndef __SSLOVERRIDEHTTP_H
#define __SSLOVERRIDEHTTP_H

#include <QHttp>
#include <QSslError>
#include <QHttpResponseHeader>

class QSslSocket;

class CSslOverrideHttp : public QHttp
{
	Q_OBJECT
public:
	CSslOverrideHttp( QObject * parent = 0 );
	CSslOverrideHttp( const QString & hostName, quint16 port = 80, QObject * parent = 0 );
	CSslOverrideHttp( const QString & hostName, ConnectionMode mode, quint16 port = 0, QObject * parent = 0 );
	~CSslOverrideHttp();

	QString errorString() const;
	QSslError::SslError sslError() const;
	void setResponseHeader( const QHttpResponseHeader & header ){ fResponseHeader = header; }
	QString getResponseHeaderMsg() const;
	QHttpResponseHeader responseHeader(){ return fResponseHeader; }
	QString getCertInfo() const;
public slots:
	void slotSslError( const QList< QSslError > & errors );
private:
	QList< QSslError > fSslErrors;
	QHttpResponseHeader fResponseHeader;
	QSslSocket * fSslSocket;
};

#endif
