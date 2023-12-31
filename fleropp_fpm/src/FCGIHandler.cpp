#include "CGIEnvironment.hpp"
#include "dispatch.hpp"
#include "FCGIHandler.hpp"
#include "FCGIWorker.hpp"
#include "FleroppIO.hpp"
#include "RequestData.hpp"
#include "ScopedRedirect.hpp"

#include "fcgio.h"
#include "fcgios.h"

#include <fmt/format.h>
#include "spdlog/spdlog.h"
#include "util.hpp"

#include <string>

namespace fleropp::fpm {
    FCGIHandler::FCGIHandler(const std::string &unix_sock, 
                                const unsigned int backlog) {
       if (!fleropp::util::permissions::change_group_name("www-data")) {
            spdlog::critical("Unable to change Unix Socket Permissions");
       }

        m_fd = FCGX_OpenSocket(unix_sock.c_str(), backlog);
        
        if ( m_fd == -1 ) {
            spdlog::critical("Unable to open Unix Socket '{}'", unix_sock);
            return;
        }
        FCGX_Init();
        m_db_handle = nullptr;
        spdlog::info("Initialized Unix domain FastCGI handler at {}", unix_sock);
        // Possibly drop permissions or use capabilities to create files

    }

    FCGIHandler::FCGIHandler(const unsigned int tcp_sock, 
                                const unsigned int backlog) {
        auto sock_str = ":" + std::to_string(tcp_sock);
        m_fd = FCGX_OpenSocket(sock_str.c_str(), backlog);
        FCGX_Init();
        m_db_handle = nullptr;
        spdlog::info("Initialized TCP FastCGI handler at {}", sock_str);
    }

    void FCGIHandler::spawn(std::size_t n_thr) {
        for (std::size_t i = 0; i < n_thr; ++i) {
            m_workers.emplace_back([this] () { 
                fleropp::fpm::concurrency::FCGIWorker worker{m_fd, m_endpoints, m_db_handle};
                worker.accept();
            });
        }
    }

    void FCGIHandler::wait() {
        for (auto& thread : m_workers) {
            thread.join();
        }
    }

    void FCGIHandler::load_endpoints(const endpoints_map_t& endpoints_map) {
        m_endpoints = endpoints_map;
    }

    void FCGIHandler::connect_db(
        const std::string &driver,
        const std::string &username,
        const std::string &password,
        const std::string &dbname,
        const std::string &host,
        const std::string &port) {

        fleropp::fpm::CompUnit<IDatabaseDriver> driver_unit{driver};

        auto driver_instance = driver_unit.get_instance(
            username, password, dbname, host, port
        );
        // Load the driver into memory

        m_db_handle = std::move(driver_instance);
    }
}