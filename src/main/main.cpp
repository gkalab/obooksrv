#include <iostream>

#include "httplib.h"
#include "Polyglot.h"

int main(int argc, char const *argv[]) {
    chess::Polyglot data("opening.data");
    std::cout << "Available moves from the start position:" << std::endl;
    std::cout << data.getMoves("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") << std::endl;

    httplib::Server server;
    server.Get("/query", [&data](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("fen") && req.has_param("action") && req.get_param_value("action")=="get_book_moves") {
            auto fen = req.get_param_value("fen");
            std::string const moves = data.getMoves(fen, true);
            res.set_content(moves, "text/plain");
        } else {
        	res.set_content("{}", "text/plain");
        }
        res.set_header("Access-Control-Allow-Origin", "*");
    });

    std::cout << "Listening on http://0.0.0.0:7777 ..." << std::endl;
    std::cout << "Start position: http://localhost:7777/query?action=get_book_moves&fen=rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR%20w%20KQkq%20-%200%201" << std::endl;
    server.listen("0.0.0.0", 7777);
    return 0;
}
