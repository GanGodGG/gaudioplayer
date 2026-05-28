#pragma once

#include <string>
#include <iostream>
#include <vector>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>


namespace gserver{
    // Creating a server object


    static void startServer(std::string& auth_code){
            httplib::Server svr;
        // Setting up a simple GET endpoint at /ganuth
            svr.Get("/", [&](const httplib::Request& req, httplib::Response& res) {
            if (req.has_param("code")) {
                auth_code = req.get_param_value("code");
                res.set_content("<h1>Succesful auth!</h1><p>You can get back to thee app.</p>", "text/html; charset=utf-8");
                svr.stop();
            } 
            else if (req.has_param("error")) {
                std::string error = req.get_param_value("error");
                res.set_content("<h1>Error!</h1><p>" + error + "</p>", "text/html; charset=utf-8");
                svr.stop();
            } 
            else {
                res.set_content("<h1>Waiting for para...</h1>", "text/html; charset=utf-8");
            }
        });
        // Starting the server on localhost at port 8080
        svr.listen("127.0.0.1", 8080);
    }
}