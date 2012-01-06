/*------------------------------------------------------------------
// Copyright (c) 1997 - 2011
// Robert Umbehant
// htmapp@wheresjames.com
// http://www.wheresjames.com
//
// Redistribution and use in source and binary forms, with or
// without modification, are permitted for commercial and
// non-commercial purposes, provided that the following
// conditions are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * The names of the developers or contributors may not be used to
//   endorse or promote products derived from this software without
//   specific prior written permission.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
//   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
//   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------*/

#include "qtwk.h"
#include "network.h"

CNetworkReply::CNetworkReply( QObject *parent, const QNetworkRequest &req, const QNetworkAccessManager::Operation op )
	: QNetworkReply( parent )
{
	// Setup the request
	setRequest( req );
	setUrl( req.url() );
	setOperation( op );
	QNetworkReply::open( QIODevice::ReadOnly | QIODevice::Unbuffered );

	// Bogus content
	m_lOffset = 0;
	m_content.clear();

	// Get the path to the file
	QByteArray path = req.url().path().toUtf8();

	str::t_string8 full = 
		disk::WebPath< str::t_char8, str::t_string8 >( "res", str::t_string8( path.data(), path.length() ) );

	// Check for linked in resources
	CHmResources res;
	if ( res.IsValid() )
	{
		printf( "%s\n", "Have embedded resources" );
		
		// See if there is such a resource
		HMRES hRes = res.FindResource( 0, full.c_str() );
		if ( hRes )
		{
			printf( "%s\n", "Found resource" );
			
			// Get function pointer
			CHmResources::t_fn pFn = res.Fn( hRes );
			if ( pFn )
				printf( "%s\n", "Found function" );
			
		} // end if		
	
	} // end if

	
	// Set headers
//	setHeader( QNetworkRequest::ContentTypeHeader, QVariant( sMime.c_str() ) );
	setHeader( QNetworkRequest::ContentLengthHeader, QVariant( m_content.size() ) );

	// Call notify functions
	QMetaObject::invokeMethod( this, "metaDataChanged", Qt::QueuedConnection );
	QMetaObject::invokeMethod( this, "readyRead", Qt::QueuedConnection );
	QMetaObject::invokeMethod( this, "downloadProgress", Qt::QueuedConnection,
							   Q_ARG( qint64, m_content.size() ), Q_ARG( qint64, m_content.size() ) );
	QMetaObject::invokeMethod( this, "finished", Qt::QueuedConnection );

}

qint64 CNetworkReply::readData( char* pData, qint64 lMaxSize )
{
	// Have we copied all the data?
	if ( m_lOffset >= m_content.size() )
		return -1;

	// Copy a block of data
	qint64 lCount = qMin( lMaxSize, m_content.size() - m_lOffset );
	memcpy( pData, m_content.constData() + m_lOffset, lCount );
	m_lOffset += lCount;

	// Return the number of bytes copied
	return lCount;
}


CNetworkMgr::CNetworkMgr( QObject *pParent, QNetworkAccessManager *pPrev )
	: QNetworkAccessManager( pParent )
{
	if ( pPrev )
	{
		setCache( pPrev->cache() );
		setCookieJar( pPrev->cookieJar() );
		setProxy( pPrev->proxy() );
		setProxyFactory( pPrev->proxyFactory() );

	} // end if

}

QNetworkReply* CNetworkMgr::createRequest( QNetworkAccessManager::Operation op, const QNetworkRequest &req, QIODevice *device )
{
	// printf( ": %s\n", req.url().toString().toUtf8().data() );

	if ( req.url().host() == "embedded" )
		return new CNetworkReply( this, req, op );

	return QNetworkAccessManager::createRequest( op, req, device );
}
