#include "bussocket.hpp"
#include <eepp/network/ipaddress.hpp>

namespace ecode {

BusSocket::BusSocket( const Connection& connection ) : mConnection( connection ) {}

bool BusSocket::start() {
	bool res =
		mSocket.connect( IpAddress( mConnection.host ), mConnection.port ) == Socket::Status::Done;
	if ( res )
		setState( State::Running );
	return res;
}

bool BusSocket::close() {
	if ( mState == State::Running ) {
		mSocket.disconnect();
		setState( State::Closed );
		return true;
	}

	return false;
}

void BusSocket::startAsyncRead( ReadFn readFn ) {
	mSocket.startAsyncRead( readFn );
}

size_t BusSocket::write( const char* buffer, const size_t& size ) {
	size_t sent = 0;
	mSocket.send( buffer, size, sent );
	return sent;
}

} // namespace ecode
