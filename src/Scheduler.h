#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <string>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>

/**
 * @brief Task scheduling for periodic DNS hydration
 * 
 * Manages background tasks that periodically refresh DNS resolutions
 * for blocked FQDNs and update the firewall rules accordingly.
 */
class Scheduler {
public:
    /**
     * @brief Initialize the scheduler
     */
    static void Initialize();

    /**
     * @brief Start the scheduler event loop
     * Runs in a background thread and triggers refresh tasks at specified intervals
     */
    static void Start();

    /**
     * @brief Stop the scheduler and cleanup
     */
    static void Stop();

    /**
     * @brief Add a scheduled task for an FQDN
     * @param fqdn FQDN to refresh periodically
     * @param intervalMinutes Refresh interval in minutes
     * @return true if task added successfully, false otherwise
     */
    static bool AddTask(const std::string& fqdn, int intervalMinutes);

    /**
     * @brief Remove a scheduled task
     * @param fqdn FQDN task to remove
     * @return true if task removed successfully, false otherwise
     */
    static bool RemoveTask(const std::string& fqdn);

    /**
     * @brief Get the number of active tasks
     * @return Number of scheduled tasks
     */
    static int GetTaskCount();

    /**
     * @brief Check if scheduler is running
     * @return true if running, false otherwise
     */
    static bool IsRunning();

private:
    struct Task {
        std::string fqdn;
        int intervalMinutes;
        std::chrono::steady_clock::time_point nextRun;

        Task() : intervalMinutes(0) {}
        Task(const std::string& f, int interval)
            : fqdn(f), intervalMinutes(interval),
              nextRun(std::chrono::steady_clock::now() + std::chrono::minutes(interval)) {}
    };

    /**
     * @brief Main scheduler loop (runs in background thread)
     */
    static void SchedulerLoop();

    /**
     * @brief Trigger refresh for a specific FQDN
     * @param fqdn FQDN to refresh
     */
    static void TriggerRefresh(const std::string& fqdn);

    static std::map<std::string, Task> tasks;
    static std::mutex taskMutex;
    static std::thread schedulerThread;
    static std::atomic<bool> running;
    static std::atomic<bool> initialized;
};

#endif // SCHEDULER_H
