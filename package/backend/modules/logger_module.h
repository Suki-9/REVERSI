#pragma once

/* ----- LOGGER macros declaration -----*/
#define ERR_LOGGER( service, text ) std::cout << "[" << service << " service - " << __func__ << "]: " << text << std::endl
#define LOGGER( service, text ) std::cout << "[" << service << " service - " << __func__ << "]: " << text << std::endl
