#pragma once

/* ----- LOGGER macros declaration -----*/
#define ERR_LOGGER( service, text ) std::cout << es_red << "ERR [" << service << " Service - " << __func__ << "]: " << es_reset << text << std::endl
#define WARN_LOGGER( service, text ) std::cout << es_yellow << "WARN [" << service << " Service - " << __func__ << "]: " << es_reset << text << std::endl
#define LOGGER( service, text ) std::cout << es_blue << "INFO [" << service << " Service - " << __func__ << "]: " << es_reset << text << std::endl

#define es_red "\e[31m"
#define es_green "\e[32m"
#define es_blue "\e[34m"
#define es_yellow "\e[33m"
#define es_reset "\e[0m"
