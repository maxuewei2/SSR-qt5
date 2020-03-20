#ifndef CONFIG_H
#define CONFIG_H

#include <string>
class Config
{
public:
    Config();
    void set_subscription_url(std::string s);
    void set_socks_port(std::string s);
    void set_http_port(std::string s);
    std::string get_subscription_url() const;
    std::string get_socks_port() const;
    std::string get_http_port() const;

    int changed_flag=0;
private:

    std::string subscription_url="";
    std::string socks_port="";
    std::string http_port="";
};

#endif // CONFIG_H
