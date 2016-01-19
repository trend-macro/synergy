/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2015 Synergy Seamless Inc.
 * 
 * This package is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * found in the file LICENSE that should have accompanied this file.
 * 
 * This package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "synergy/SubscriptionManager.h"

#include "synergy/XSynergy.h"
#include "arch/Arch.h"
#include "base/Log.h"
#include "base/String.h"
#include "common/Version.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <ctime>
//#include <stdexcept>

#if SYSAPI_WIN32
const char* kFile = "Synergy.subkey";
#else
const char* kFile = ".synergy.subkey";
#endif

//
// SubscriptionManager
//

SubscriptionManager::SubscriptionManager() :
	m_key()
{
}

void
SubscriptionManager::checkFile(const String& filename_)
{
	String filename = filename_;
	if (filename.empty()) {
		filename = getFilename();
	}

	std::ifstream stream(filename.c_str());
	if (!stream.is_open()) {
		throw XSubscription(synergy::string::sprintf(
			"Could not open, path=%s", filename.c_str()));
	}

	String serial;
	stream >> serial;
	
	String plainText = decode(serial);
	parsePlainSerial(plainText, m_key);
	
	LOG((CLOG_DEBUG "subscription is valid"));
}

void
SubscriptionManager::activate(const String& serial)
{
	String plainText = decode(serial);
	parsePlainSerial(plainText, m_key);
	
	String filename = getFilename();
	std::ofstream stream(filename.c_str());
	if (!stream.is_open()) {
		throw XSubscription(synergy::string::sprintf(
			"Could not open, file=%s", filename.c_str()));
	}

	stream << serial << std::endl;
	LOG((CLOG_DEBUG "subscription file created, path=%s", filename.c_str()));
}

String
SubscriptionManager::decode(const String& input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();
    if (len & 1) {
		throw XSubscription("Invalid serial, wrong length.");
	}

    String output;
    output.reserve(len / 2);
    for (size_t i = 0; i < len; i += 2) {

        char a = input[i];
        char b = input[i + 1];

        const char* p = std::lower_bound(lut, lut + 16, a);
        const char* q = std::lower_bound(lut, lut + 16, b);

        if (*q != b || *p != a) {
			throw XSubscription("Invalid serial, unrecognized digit.");
		}

        output.push_back(static_cast<char>(((p - lut) << 4) | (q - lut)));
    }

    return output;
}

void
SubscriptionManager::parsePlainSerial(const String& plainText, SubscriptionKey& key)
{
	LOG((CLOG_INFO "%s subscription valid is for unlimited days, registered to Trend Macro.",
}

String
SubscriptionManager::getFilename()
{
	String path = ARCH->getProfileDirectory();
	path = ARCH->concatPath(path, kFile);
	if (path.empty()) {
		throw XSubscription("Could not get filename.");
	}

	return path;
}

void 
SubscriptionManager::printFilename()
{
	std::cout << getFilename() << std::endl;
}
