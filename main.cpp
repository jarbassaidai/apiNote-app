#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include "serverlb.h"
#include "apiNote.h"
using namespace std;
using namespace boost;

///\fn buildPaths
///\arg string of comma separated paths
///\return std::set<std::string>

std::set<std::string> buildPaths(std::string pathList){
    std::set<std::string> rset;
    std::vector<std::string> fields;
    split( fields, pathList, is_any_of( ", ;" ) ); /** split on comma space or semicolon */
    for (std::vector<std::string>::iterator it = fields.begin();
         it != fields.end(); it++ ) {
            if (!(*it).empty())
                rset.insert(*it);
         }
     if (rset.empty()){
        rset.insert({"/api/notes","/api/notes/{id: [0-9]*}"});
    }
    return rset;
}

int main(int argc, char * argv[])
{
  namespace bpo = boost::program_options;

    bpo::options_description desc;
    bpo::variables_map vm;
    int retValue = 0 ;
    int port = 0;
    std::string filespec,urladdr,ipaddr;
    std::string helpstr("this program will put up a mini webserver that will keep notes\n \
        Input Args are --port <port number>  defaults to 1984 \n\
                       --ipadress <ip address> defaults to 127.0.0.1\n\
                       --jsonStore <file specification> defaults  to./apiNote.json\n\
                       --urladdress <url address of notes> defaults to (comma/space/semicolon) separated list \n\
                       \"/api/notes,/api/notes/{id: [0-9]*}\"\n\
        to add a new note \n\tcurl -i -H \"Content-Type: application/json\" -X POST -d \'{\"body\" : \"Pick up milk!\"}\' \
\"http://localhost:1894/api/notes \
    \nto find a specific note by its id\n\tcurl -i -H \"Content-Type: application/json\" -X GET http://localhost:1894/api/notes/1\
    \nto fetch all notes\n\tcurl -i -H \"Content-Type: application/json\" -X GET http://localhost:1894/api/notes\
    \nto find a specific note\n\tcurl -i -H \"Content-Type: application/json\" -X GET http://localhost:1894/api/notes?query=milk");

    desc.add_options()
        ("help", helpstr.c_str())
        ("port", bpo::value<int>(), "port for webservice")
        ("ipaddress",bpo::value<std::string>(),"ip address to listen on")
        ("urladdress",bpo::value<std::string>(),"url address to for notes")
        ("jsonStore",bpo::value<std::string>(),"file specification for note storage")
        ;

        bpo::store(bpo::parse_command_line(argc, argv, desc), vm);
        bpo::notify(vm);

        if (vm.count("help")) {
            std::cerr << helpstr.c_str() << std::endl;
            retValue = 1;
        }
        if (vm.count("port")) {
            port = (vm["port"].as<int>() );
        }
        if (vm.count("ipaddress")) {
            ipaddr = (vm["ipaddress"].as<std::string>());
        }
        if (vm.count("urladdress")) {
            urladdr = (vm["urladdress"].as<std::string>());
        }
        if (vm.count("jsonStore")) {
            filespec = (vm["jsonStore"].as<std::string>());
        }

        if (vm.count("unitTest")) {
            std::string unitTest = (vm["unitTest"].as<string>() );
        }




    if (retValue == 1){
        /** start our http rest framework and mini-webserver **/
        std::cout << "--help for usage starting webserver-note-app ^C to exit" << std::endl;
        /** fire up a apiNote-application that the  webserver will call out to. **/
        t_shApiNoteptr appPtr = apiNote::getInstance();
        appPtr->openStore(filespec);
        serverlb levelBeyond(port,ipaddr, buildPaths(urladdr));

    std::cout << "ending apiNote" << std::endl;
    }
    return (retValue);
}
