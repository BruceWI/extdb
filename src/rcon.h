/*
Copyright (C) 2014 Declan Ireland <http://github.com/torndeco/extDB>
Copyright (C) 2012 Prithu "bladez" Parker <https://github.com/bladez-/bercon>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

 * Change Log
 * Changed Code to use Poco Net Library 
*/


#pragma once

#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/unordered_map.hpp>

#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/NetException.h>

#include <Poco/Stopwatch.h>

#include <Poco/ExpireCache.h>

#include "spdlog/spdlog.h"
#include "protocols/abstract_ext.h"

#include <atomic>

class Rcon: public Poco::Runnable
{
	public:
		void init(std::shared_ptr<spdlog::logger> console, std::string address, int port, std::string password);
		void run();
		void disconnect();
		
		void addCommand(std::string command);

	private:
		typedef std::pair< int, std::unordered_map < int, std::string > > RconMultiPartMsg;

		struct RconPacket
		{
			char cmd_char_workaround;
			char *cmd;
			unsigned char packetCode;
			std::string packet;
		};
		
		struct RconLogin {
			std::string address;
			int port;
			char *password;
			bool auto_reconnect;
		};

		RconPacket rcon_packet;
		RconLogin rcon_login;

		Poco::Net::SocketAddress sa;
		Poco::Net::DatagramSocket dgs;

		Poco::Stopwatch rcon_timer;

		std::shared_ptr<spdlog::logger> logger_console;
		std::shared_ptr<spdlog::logger> logger_file;
		
		char buffer[4096];
		int buffer_size;
		
		// Mutex Locks
		std::atomic<bool> rcon_run_flag = false;
		bool logged_in = false;
		
		boost::recursive_mutex mutex_rcon_commands;
		std::vector< std::string > rcon_commands;

		// Functions
		void connect();
		void mainLoop();

		void makePacket(RconPacket &rcon);
		void extractData(int pos, std::string &result);
};
