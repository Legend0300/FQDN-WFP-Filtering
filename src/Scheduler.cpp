#include "Scheduler.h"
#include "AuditLogger.h"
#include "Resolver.h"
#include "FirewallManager.h"
#include <iostream>
#include <algorithm>

// Initialize static members
std::map<std::string, Scheduler::Task> Scheduler::tasks;
std::mutex Scheduler::taskMutex;
std::thread Scheduler::schedulerThread;
std::atomic<bool> Scheduler::running(false);
std::atomic<bool> Scheduler::initialized(false);

void Scheduler::Initialize() {
    if (initialized) {
        return;
    }

    std::cout << "Scheduler initialized" << std::endl;
    initialized = true;
}

void Scheduler::Start() {
    if (running) {
        std::cout << "Scheduler is already running" << std::endl;
        return;
    }

    if (!initialized) {
        Initialize();
    }

    running = true;
    schedulerThread = std::thread(SchedulerLoop);
    std::cout << "Scheduler started" << std::endl;
}

void Scheduler::Stop() {
    if (!running) {
        return;
    }

    running = false;

    if (schedulerThread.joinable()) {
        schedulerThread.join();
    }

    std::cout << "Scheduler stopped" << std::endl;
}

bool Scheduler::AddTask(const std::string& fqdn, int intervalMinutes) {
    std::lock_guard<std::mutex> lock(taskMutex);

    if (tasks.find(fqdn) != tasks.end()) {
        std::cout << "Task for " << fqdn << " already exists, updating interval" << std::endl;
        tasks[fqdn] = Task(fqdn, intervalMinutes);
        return true;
    }

    tasks[fqdn] = Task(fqdn, intervalMinutes);
    std::cout << "Added scheduled task for " << fqdn << " (every " << intervalMinutes << " minutes)" << std::endl;
    return true;
}

bool Scheduler::RemoveTask(const std::string& fqdn) {
    std::lock_guard<std::mutex> lock(taskMutex);

    auto it = tasks.find(fqdn);
    if (it != tasks.end()) {
        tasks.erase(it);
        std::cout << "Removed scheduled task for " << fqdn << std::endl;
        return true;
    }

    std::cout << "Task not found for " << fqdn << std::endl;
    return false;
}

int Scheduler::GetTaskCount() {
    std::lock_guard<std::mutex> lock(taskMutex);
    return static_cast<int>(tasks.size());
}

bool Scheduler::IsRunning() {
    return running;
}

void Scheduler::SchedulerLoop() {
    std::cout << "Scheduler loop started" << std::endl;

    while (running) {
        // Sleep for a short interval (e.g., 10 seconds)
        std::this_thread::sleep_for(std::chrono::seconds(10));

        if (!running) {
            break;
        }

        // Check if any tasks are due
        std::lock_guard<std::mutex> lock(taskMutex);
        auto now = std::chrono::steady_clock::now();

        for (auto& pair : tasks) {
            Task& task = pair.second;

            if (now >= task.nextRun) {
                // Task is due, trigger refresh
                std::cout << "\n[Scheduler] Triggering refresh for: " << task.fqdn << std::endl;
                TriggerRefresh(task.fqdn);

                // Schedule next run
                task.nextRun = now + std::chrono::minutes(task.intervalMinutes);
                std::cout << "[Scheduler] Next refresh in " << task.intervalMinutes << " minutes" << std::endl;
            }
        }
    }

    std::cout << "Scheduler loop ended" << std::endl;
}

void Scheduler::TriggerRefresh(const std::string& fqdn) {
    try {
        // Get the record from audit logger
        Record record;
        if (!AuditLogger::GetRecord(fqdn, record)) {
            std::cerr << "[Scheduler] Record not found for: " << fqdn << std::endl;
            return;
        }

        // Resolve the FQDN to get new IPs
        std::vector<std::string> newIPs = Resolver::ResolveFqdn(fqdn);

        if (newIPs.empty()) {
            std::cerr << "[Scheduler] DNS resolution failed for: " << fqdn << std::endl;
            return;
        }

        // Check if IPs have changed
        bool ipsChanged = false;
        if (newIPs.size() != record.lastResolvedIPs.size()) {
            ipsChanged = true;
        }
        else {
            // Sort both vectors and compare
            std::vector<std::string> sortedNew = newIPs;
            std::vector<std::string> sortedOld = record.lastResolvedIPs;
            std::sort(sortedNew.begin(), sortedNew.end());
            std::sort(sortedOld.begin(), sortedOld.end());

            if (sortedNew != sortedOld) {
                ipsChanged = true;
            }
        }

        if (ipsChanged) {
            std::cout << "[Scheduler] IP addresses changed for: " << fqdn << std::endl;
            std::cout << "[Scheduler] Old IPs: " << record.lastResolvedIPs.size() 
                     << ", New IPs: " << newIPs.size() << std::endl;

            // Update the dynamic keyword address
            if (FirewallManager::UpdateDynamicKeywordAddress(record.keywordId, newIPs)) {
                // Update the audit record
                AuditLogger::UpdateRecord(fqdn, newIPs);
                std::cout << "[Scheduler] Successfully updated firewall rules for: " << fqdn << std::endl;
            }
            else {
                std::cerr << "[Scheduler] Failed to update firewall rules for: " << fqdn << std::endl;
            }
        }
        else {
            std::cout << "[Scheduler] No IP changes detected for: " << fqdn << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[Scheduler] Error refreshing " << fqdn << ": " << e.what() << std::endl;
    }
}
