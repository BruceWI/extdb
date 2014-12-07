//
// SessionPool.h
//
// $Id: //poco/1.4/Data/include/Poco/Data/SessionPool.h#1 $
//
// Library: Data
// Package: SessionPooling
// Module:  SessionPool
//
// Definition of the SessionPool class.
//
// Copyright (c) 2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef Data_SessionPool_INCLUDED
#define Data_SessionPool_INCLUDED


#include "Poco/Data/Data.h"
#include "Poco/Data/Statement.h"
#include "Poco/Data/PooledSessionHolder.h"
#include "Poco/Data/PooledSessionImpl.h"
#include "Poco/Data/Session.h"
#include "Poco/Timer.h"
#include "Poco/Mutex.h"
#include <list>
#include <unordered_map>
#include <vector>


namespace Poco {
namespace Data {


class Data_API SessionPool
	/// This class implements session pooling for POCO Data.
	///
	/// Creating a connection to a database is often a time consuming
	/// operation. Therefore it makes sense to reuse a session object
	/// once it is no longer needed.
	///
	/// A SessionPool manages a collection of SessionImpl objects 
	/// (decorated with a PooledSessionImpl).
	///
	/// When a SessionImpl object is requested, the SessionPool first
	/// looks in its set of already initialized SessionImpl for an
	/// available object. If one is found, it is returned to the
	/// client and marked as "in-use". If no SessionImpl is available,
	/// the SessionPool attempts to create a new one for the client.
	/// To avoid excessive creation of SessionImpl objects, a limit
	/// can be set on the maximum number of objects.
	/// Sessions found not to be connected to the database are purged
	/// from the pool whenever one of the following events occurs:
	/// 
	///   - JanitorTimer event
	///   - get() request
	///   - putBack() request
	///
	/// Not connected idle sessions can not exist.
	///
	/// Usage example:
	///
	///     SessionPool pool("ODBC", "...");
	///     ...
	///     Session sess(pool.get());
	///     ...
{
public:
	typedef Poco::AutoPtr<PooledSessionHolder> PooledSessionHolderPtr;
	
	typedef std::vector<Poco::Data::Statement> StatementCache;

	typedef std::unordered_map <std::string, StatementCache> StatementCacheMap;
	typedef std::list < std::pair < PooledSessionHolderPtr, StatementCacheMap > > SessionList;

	SessionPool(const std::string& sessionKey, const std::string& connectionString, int minSessions = 1, int maxSessions = 32, int idleTime = 60);
		/// Creates the SessionPool for sessions with the given sessionKey
		/// and connectionString.
		///
		/// The pool allows for at most maxSessions sessions to be created.
		/// If a session has been idle for more than idleTime seconds, and more than
		/// minSessions sessions are in the pool, the session is automatically destroyed.
		///
		/// If idleTime is 0, automatic cleanup of unused sessions is disabled.

	virtual ~SessionPool();
		/// Destroys the SessionPool.
		
	Session get();
		/// Returns a Session.
		///
		/// If there are unused sessions available, one of the
		/// unused sessions is recycled. Otherwise, a new session
		/// is created. 
		///
		/// If the maximum number of sessions for this pool has
		/// already been created, a SessionPoolExhaustedException
		/// is thrown.

// Custom extDB Member		
	Session extDB_get(SessionList::iterator &itr);
	void extDB_updateStatementCacheMap(StatementCacheMap &statement_cachemap, SessionList::iterator &itr);
		
	int capacity() const;
		/// Returns the maximum number of sessions the SessionPool will manage.
		
	int used() const;
		/// Returns the number of sessions currently in use.
		
	int idle() const;
		/// Returns the number of idle sessions.
		
	int dead();
		/// Returns the number of not connected active sessions.

	int allocated() const;
		/// Returns the number of allocated sessions.
		
	int available() const;
		/// Returns the number of available (idle + remaining capacity) sessions.
		
	void putBack(SessionList::iterator ptr);

protected:
	virtual void customizeSession(Session& session);
		/// Can be overridden by subclass to perform custom initialization
		/// of a newly created database session.
		///
		/// The default implementation does nothing.

	typedef Poco::AutoPtr<PooledSessionImpl>   PooledSessionImplPtr;

	
	void purgeDeadSessions();
	int deadImpl(SessionList& rSessions);
	void putBack(PooledSessionHolderPtr pHolder);
	void onJanitorTimer(Poco::Timer&);

private:
	SessionPool(const SessionPool&);
	SessionPool& operator = (const SessionPool&);
		
	std::string _sessionKey;
	std::string _connectionString;
	int _minSessions;
	int _maxSessions;
	int _idleTime;
	int _nSessions;
	SessionList _idleSessions;
	SessionList _activeSessions;
	
	Poco::Timer _janitorTimer;
	mutable Poco::FastMutex _mutex;
	
	friend class PooledSessionImpl;
};


} } // namespace Poco::Data


#endif // Data_SessionPool_INCLUDED
