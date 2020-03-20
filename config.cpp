#include "config.h"

Config::Config()
{

}
void Config::set_subscription_url(std::string s){
    subscription_url=s;
}
void Config::set_socks_port(std::string s){
    socks_port=s;
}
void Config::set_http_port(std::string s){
    http_port=s;
}

std::string Config::get_subscription_url() const
{
    return subscription_url;
}

std::string Config::get_socks_port() const
{
    return socks_port;
}

std::string Config::get_http_port() const
{
    return http_port;
}
