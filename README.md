Small project to build a webserver that creates notes that can be entered, fetched by id, and searched
originally built on Linux(fedora 24) 64bit machine

dependencies:
    restbed ()  https://github.com/Corvusoft/restbed
required includes:
    <dir>/restbed/source/corvusoft   for *.hpp
    <dir>/restbed/source             for <restbed>

required libraries:
    libpthread.so
    libboost_system.so
    libboost_thread.so
    libcrypt.so
    libcrypto.so
    libboost_log.so
    libssl.so
    libboost_regex.so
    libboost_program_options.so
    restbed.a

required Makefile changes:

RESTBED_DIR=<directory where restbed lives>
RESTBIDLIB=<$(RESTBED_DIR)<subdirector where librestbed.a lives>
LIBDIR=<system library directory> <default is /usr/lib64/

to build release
makefile -f Makefile.mak release
can also be built with Code::Blocks IDE

    this program will put up a mini webserver that will keep notes
        Input Args are --port <port number>  defaults to 1984
                       --ipadress <ip address> defaults to 127.0.0.1
                       --jsonStore <file specification> defaults  to ./apiNote.json & ./apiNote.json.index
                       --urladdress <url address of notes> defaults to (comma/space/semicolon) separated list
                       "/api/notes,/api/notes/{id: [0-9]*}\"
        to add a new note
          curl -i -H "Content-Type: application/json" -X POST -d '{"body" : "Pick up milk!"}' "http://localhost:1894/api/notes
        to find a specific note by its id
            curl -i -H "Content-Type: application/json" -X GET http://localhost:1894/api/notes/1
        to fetch all notes
            curl -i -H "Content-Type: application/json" -X GET http://localhost:1894/api/notes
        to find a specific note
            curl -i -H "Content-Type: application/json" -X GET http://localhost:1894/api/notes?query=milk
