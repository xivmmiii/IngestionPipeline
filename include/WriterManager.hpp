#pragma once
#include "LogEntry.hpp"
#include "RawLine.hpp"
#include "Severity.hpp"
#include <map>
#include <fstream>
#include <mutex>
#include <string>
#include <filesystem>

class WriterManager
{
private:
    std::map<Severity, std::ofstream> severityFiles;
    std::ofstream malformedFile;
    std::mutex m1; // for severity files
    std::mutex m2; // for malformed file

    std::string severityToString(Severity s) const
    {
        switch (s)
        {
        case Severity::DEBUG:
            return "DEBUG";
        case Severity::INFO:
            return "INFO";
        case Severity::WARN:
            return "WARN";
        case Severity::ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
        }
    }

public:
    explicit WriterManager(const std::string &outDir)
    {
        std::filesystem::create_directories(outDir);

        severityFiles[Severity::INFO].open(outDir + "/INFO.log", std::ios::app);
        severityFiles[Severity::WARN].open(outDir + "/WARN.log", std::ios::app);
        severityFiles[Severity::ERROR].open(outDir + "/ERROR.log", std::ios::app);
        severityFiles[Severity::DEBUG].open(outDir + "/DEBUG.log", std::ios::app);

        malformedFile.open(outDir + "/malformed.log", std::ios::app);
    }

    void writeSeverity(const LogEntry &e, const std::string &rawLine)
    {
        std::lock_guard<std::mutex> lock(m1);
        Severity s = e.getSeverity();
        auto it = severityFiles.find(s);
        if (it != severityFiles.end() && it->second.is_open())
        {
            it->second << rawLine << "\n";
        }
    }

    void writeMalformed(const RawLine &raw)
    {
        std::lock_guard<std::mutex> lock(m2);
        if (malformedFile.is_open())
        {
            malformedFile << raw.getLine() << "\n";
        }
    }

    void closeAll()
    {
        std::lock_guard<std::mutex> lock1(m1);
        std::lock_guard<std::mutex> lock2(m2);

        for (auto &pair : severityFiles)
        {
            if (pair.second.is_open())
                pair.second.close();
        }
        if (malformedFile.is_open())
            malformedFile.close();
    }
};