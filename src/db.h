#pragma once

#include <boost/system/error_code.hpp>
#include <string>
#include <queue>
#include <list>
#include <json.hpp>

namespace boost { namespace asio {
    class io_service;
}}

namespace ipfs_cache {

class Backend;
class Republisher;
class Timer;
using Json = nlohmann::json;

class Db {
public:
    Db(Backend&, std::string ipns);

    void update( std::string key, std::string value
               , std::function<void(boost::system::error_code)>);
    void query( std::string key
              , std::function<void(boost::system::error_code, std::string)>);

    boost::asio::io_service& get_io_service();

    const Json& json_db() const;
    const std::string& ipns() const { return _ipns; }
    const std::string& ipfs() const { return _ipfs; }

    ~Db();

private:
    void start_db_download();
    void on_db_download(Json&& json);
    void replay_queued_tasks();
    void merge(const Json&);
    void start_updating();
    void initialize(Json&);

    template<class F> void download_database(const std::string&, F&&);
    template<class F> void upload_database(const Json&, F&&);

private:
    bool _is_uploading = false;
    Json _json;
    std::string _ipns;
    std::string _ipfs; // Last known
    Backend& _backend;
    std::unique_ptr<Republisher> _republisher;
    std::list<std::function<void(boost::system::error_code)>> _upload_callbacks;
    std::shared_ptr<bool> _was_destroyed;
    std::unique_ptr<Timer> _download_timer;
};

} // ipfs_cache namespace

