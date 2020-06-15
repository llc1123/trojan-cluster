/*
 * This file is part of the trojan project.
 * Trojan is an unidentifiable mechanism that helps you bypass GFW.
 * Copyright (C) 2017-2020  The Trojan Authors.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "authenticator.h"
#include <cstdlib>
#include <stdexcept>
using namespace std;

#ifdef ENABLE_REDIS

Authenticator::Authenticator(const Config &config) {
    redis = new RedisHelper(config.redis.server_addr, config.redis.server_port);
    Log::log_with_date_time("connecting to Redis server " + config.redis.server_addr + ':' + to_string(config.redis.server_port), Log::INFO);
    if(redis->ping()){
        Log::log_with_date_time("connected to Redis server", Log::INFO);
    } else {
        Log::log_with_date_time("failed to connect to Redis server", Log::INFO);
    }
}

bool Authenticator::auth(const string &password) {
    if (!is_valid_password(password)) {
        return false;
    } else if(!redis->exists(password)) {
        return false;
    }
    return true;
}

void Authenticator::record(const std::string &password, uint64_t download, uint64_t upload) {
    if (!is_valid_password(password)) {
        return;
    }
    if (!redis->increaseValue(password, "download", download) || !redis->increaseValue(password, "upload", upload)) {
        return;
    }
}

bool Authenticator::is_valid_password(const std::string &password) {
    if (password.size() != PASSWORD_LENGTH) {
        return false;
    }
    for (size_t i = 0; i < PASSWORD_LENGTH; ++i) {
        if (!((password[i] >= '0' && password[i] <= '9') || (password[i] >= 'a' && password[i] <= 'f'))) {
            return false;
        }
    }
    return true;
}

Authenticator::~Authenticator() {
    delete redis;
}


#else // ENABLE_REDIS

Authenticator::Authenticator(const Config&) {}
bool Authenticator::auth(const string&) { return true; }
void Authenticator::record(const string&, uint64_t, uint64_t) {}
bool Authenticator::is_valid_password(const string&) { return true; }
Authenticator::~Authenticator() {}

#endif // ENABLE_REDIS
