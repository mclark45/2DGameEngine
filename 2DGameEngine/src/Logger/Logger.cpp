#include "Logger.hpp"

// Creates a vector to hold the log messages
std::vector<LogEntry> Logger::messages;

/*
	Function gets current time in the correct format as a string
	@return current time as string
*/
std::string CurrentTimeToString() {
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::string output(30, '\0');
	std::tm timeInfo;
	localtime_s(&timeInfo, &now);
	std::strftime(&output[0], output.size(), "%d-%b-%Y %H:%M:%S", &timeInfo);
	return output;
}

/*
	Function logs a message to the console
*/
void Logger::Log(const std::string& message) {
	LogEntry logEntry;
	logEntry.type = LOG_INFO;
	logEntry.message = "LOG: [" + CurrentTimeToString() + "] " + message;
	std::cout << "\x1B[32m" << logEntry.message << "\033[0m" << std::endl;
	messages.push_back(logEntry);
}

/*
	Function logs an error message to the console
*/
void Logger::Err(const std::string& message) {
	LogEntry logEntry;
	logEntry.type = LOG_ERROR;
	logEntry.message = "ERR: [" + CurrentTimeToString() + "] " + message;
	std::cout << "\x1B[32m" << logEntry.message << "\033[0m" << std::endl;
	messages.push_back(logEntry);
}