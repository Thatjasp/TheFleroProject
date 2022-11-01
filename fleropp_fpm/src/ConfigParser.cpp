#include "ConfigParser.hpp"
namespace fleropp_fpm
{
    namespace pt = boost::property_tree;
    using namespace std;

    ConfigParser::ConfigParser(const string &lib_dir) : _lib_dir(lib_dir) {}

    void ConfigParser::load(const string &filename)
    {
        try
        {
            // Create empty property tree object
            pt::ptree tree;
            // Parse the JSON into the property tree.
            pt::read_json(filename, tree);

            // loop for reading through the endpoint array.
            for (auto &it1 : tree.get_child("endpoint"))
            {
                pt::ptree endpoint = it1.second; // stores the data of the cur idx of the endpoint array
                string uri = endpoint.get<string>("uri");

                vector<CompUnit<IView>> dependencies;
                for (auto &it2 : endpoint.get_child("dependencies"))
                {
                    pt::ptree dependency = it2.second; // stores the data of the curr idx of the dependencies array
                    string shared_object = this->_lib_dir + "/" + dependency.get<string>("sharedObject");

                    vector<string> sources;
                    for (auto &it3 : dependency.get_child("source"))
                    {
                        sources.push_back(this->_lib_dir + "/" + it3.second.data());
                    }

                    // creates a CompUnit object and stores it in the dependencies vector
                    dependencies.emplace_back(shared_object, sources);
                }
                _endpoints[uri] = dependencies;
            }
        }
        catch (const exception &e)
        {
            cerr << e.what() << '\n';
        }
    }
}