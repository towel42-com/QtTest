#include "SslOverrideHttp.h"
#include <QSslCertificate>
#include <QSslSocket>
#include <QTextStream>
#include <QDateTime>

CSslOverrideHttp::CSslOverrideHttp( QObject * parent ) :
	QHttp( parent ),
	fResponseHeader( QHttpResponseHeader() )
{
	fResponseHeader.setStatusLine( -1, "Unconnected", 1,1 );

	fSslSocket = new QSslSocket;
	setSocket( fSslSocket );
	connect( this, SIGNAL( sslErrors( const QList< QSslError > & ) ), this, SLOT( slotSslError( const QList< QSslError > & ) ) );
}

CSslOverrideHttp::CSslOverrideHttp( const QString & hostName, quint16 port, QObject * parent ) :
	QHttp( hostName, port, parent ),
	fResponseHeader( QHttpResponseHeader() )
{
	fResponseHeader.setStatusLine( -1, "Unconnected", 1,1 );

	fSslSocket = new QSslSocket;
	setSocket( fSslSocket );
	connect( this, SIGNAL( sslErrors( const QList< QSslError > & ) ), this, SLOT( slotSslError( const QList< QSslError > & ) ) );
}

CSslOverrideHttp::CSslOverrideHttp( const QString & hostName, ConnectionMode mode, quint16 port, QObject * parent ) :
	QHttp( hostName, mode, port, parent ),
	fResponseHeader( QHttpResponseHeader() )
{
	fResponseHeader.setStatusLine( -1, "Unconnected", 1,1 );

	fSslSocket = new QSslSocket;
	setSocket( fSslSocket );
	connect( this, SIGNAL( sslErrors( const QList< QSslError > & ) ), this, SLOT( slotSslError( const QList< QSslError > & ) ) );
}

CSslOverrideHttp::~CSslOverrideHttp()
{
	if ( fSslSocket )
	{
		fSslSocket->deleteLater();
		fSslSocket = NULL;
	}
}

void CSslOverrideHttp::slotSslError( const QList< QSslError > & errors )
{
	QHttp * http = qobject_cast< QHttp * >( sender() );
	if ( !http )
		return;
	foreach( QSslError error, errors )
	{
#ifdef _DEBUG
		switch( error.error() )
		{
		case QSslError::SelfSignedCertificate:
		case QSslError::HostNameMismatch:
		case QSslError::CertificateNotYetValid:
			continue;
		}
#endif
		fSslErrors.push_back( error );
	}

	if ( fSslErrors.isEmpty() )
		http->ignoreSslErrors();
}

QSslError::SslError CSslOverrideHttp::sslError() const
{
	if ( !fSslErrors.isEmpty() )
	{
		return fSslErrors.front().error();
	}
	return QSslError::NoError;
}

QString CSslOverrideHttp::errorString() const
{
	if ( !fSslErrors.isEmpty() )
	{
		QStringList errors;
		foreach( QSslError error, fSslErrors )
		{
			errors << error.errorString();
		}
		return "There were issues with the servers CA certificate, " + errors.join( "." ) + ".";
	}
	return QHttp::errorString();
}

QString CSslOverrideHttp::getCertInfo() const
{
	QSslCertificate ca = fSslSocket->peerCertificate();
	if ( ca.isNull() )
		return "";

	QString retVal;
	QTextStream ts( &retVal );
	ts
		<< "Issuer Organization: " << ca.issuerInfo(QSslCertificate::Organization) << "\n"
		<< "Issuer CommonName: " << ca.issuerInfo(QSslCertificate::CommonName) << "\n"
		<< "Issuer LocalityName: " << ca.issuerInfo(QSslCertificate::LocalityName) << "\n"
		<< "Issuer Organizational Unit Name: " << ca.issuerInfo(QSslCertificate::OrganizationalUnitName) << "\n"
		<< "Issuer Country Name: " << ca.issuerInfo(QSslCertificate::CountryName) << "\n"
		<< "Issuer State Or Province Name: " << ca.issuerInfo(QSslCertificate::StateOrProvinceName) << "\n"

		<< "Subject Organization: " << ca.subjectInfo(QSslCertificate::Organization) << "\n"
		<< "Subject CommonName: " << ca.subjectInfo(QSslCertificate::CommonName) << "\n"
		<< "Subject LocalityName: " << ca.subjectInfo(QSslCertificate::LocalityName) << "\n"
		<< "Subject Organizational Unit Name: " << ca.subjectInfo(QSslCertificate::OrganizationalUnitName) << "\n"
		<< "Subject Country Name: " << ca.subjectInfo(QSslCertificate::CountryName) << "\n"
		<< "Subject State Or Province Name: " << ca.subjectInfo(QSslCertificate::StateOrProvinceName) << "\n"
		<< "Effective Date: " << ca.effectiveDate().toString() << "\n"
		<< "Expiration Date: " << ca.expiryDate().toString() << "\n";
	return retVal;
}

QString CSslOverrideHttp::getResponseHeaderMsg() const
{
	QString retVal = QString( "HTTP/%1.%2 %3 %4" )
		.arg( fResponseHeader.majorVersion() )
		.arg( fResponseHeader.minorVersion() )
		.arg( fResponseHeader.statusCode() )
		.arg( fResponseHeader.reasonPhrase() );
	return retVal;
}