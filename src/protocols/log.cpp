/*
Copyright (C) 2014 Declan Ireland <http://github.com/torndeco/extDB>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/


#include "log.h"


bool LOG::init(AbstractExt *extension, AbstractExt::DBConnectionInfo *database, const std::string init_str) 
{
	bool status;
	extension_ptr = extension;
	//database_ptr = database;

	if (!(init_str.empty()))
	{
		try
		{
			boost::filesystem::path customlog(extension_ptr->getLogPath());
			customlog /= init_str;
			if (customlog.parent_path().make_preferred().string() == extension_ptr->getLogPath())
			{
				auto logger_temp = spdlog::daily_logger_mt(init_str, customlog.make_preferred().string(), true);
				logger.swap(logger_temp);
				status = true;
			}
			else
			{
				status = false;
			}
		}
		catch (spdlog::spdlog_ex& e)
		{
			#ifdef TESTING
				extension_ptr->console->warn("extDB: LOG: Error: {0}", e.what());
			#endif
			extension_ptr->logger->warn("extDB: LOG: Error: {0}", e.what());
			status = false;
		}
	}
	else
	{
		logger = extension_ptr->logger;
		status = true;
	}
	return status;
}


bool LOG::callProtocol(std::string input_str, std::string &result, const int unique_id)
{
	logger->info(input_str.c_str());
	result = "[1]";
	return true;
}