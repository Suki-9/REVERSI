#pragma once

/* ----- LOGGER macros declaration -----*/
#define ERR_LOGGER( service, text ) std::cout << red << "ERR [" << service << " Service - " << __func__ << "]: " << reset << text << std::endl
#define WARN_LOGGER( service, text ) std::cout << yellow << "WARN [" << service << " Service - " << __func__ << "]: " << reset << text << std::endl
#define LOGGER( service, text ) std::cout << blue << "INFO [" << service << " Service - " << __func__ << "]: " << reset << text << std::endl

#define red "\e[31m"
#define green "\e[32m"
#define blue "\e[34m"
#define yellow "\e[33m" 
#define reset "\e[0m"