// Copyright (c) 2018-2020 The MARTEX developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "forgeman.h"
#include "util.h"
#include "guiinterface.h"
#include <base58.h>

CForge forgeMain;

CForge::CForgeItem* CForge::add(std::string name, std::string txHash, std::string outputIndex)
{
    CForgeItem item(name, txHash, outputIndex);
    entries.push_back(item);
    return &(entries[entries.size()-1]);
}

void CForge::remove(std::string txHash) {
    int pos = -1;
    for (int i = 0; i < ((int) entries.size()); ++i) {
        CForgeItem e = entries[i];
        if (e.getTxHash() == txHash) {
            pos = i;
            break;
        }
    }
    entries.erase(entries.begin() + pos);
}

bool CForge::readForgeConfig(std::string& strErr)
{
    int linenumber = 1;
    boost::filesystem::path pathForgeConfigFile = GetForgeConfigFile();
    boost::filesystem::ifstream streamConfig(pathForgeConfigFile);

    if (!streamConfig.good()) {
        FILE* configFile = fopen(pathForgeConfigFile.string().c_str(), "a");
        if (configFile != NULL) {
            std::string strHeader = "# Forge config file\n"
                                    "# Format: item_name item_output_txid item_output_index\n"
                                    "# Example: Grandmasters_Scroll 2bcd3c84c84f87eaa86e4e56834c92927a07f9e18718810b92e0d0324456a67c 0\n"
                                    "#\n";
            fwrite(strHeader.c_str(), std::strlen(strHeader.c_str()), 1, configFile);
            fclose(configFile);
        }
        return true; // Nothing to read, so just return
    }

    for (std::string line; std::getline(streamConfig, line); linenumber++) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string comment, name, txHash, outputIndex;

        if (iss >> comment) {
            if (comment.at(0) == '#') continue;
            iss.str(line);
            iss.clear();
        }

        if (!(iss >> name >> txHash >> outputIndex)) {
            iss.str(line);
            iss.clear();
            if (!(iss >> txHash >> outputIndex)) {
                strErr = _("Could not parse forge.conf") + "\n" +
                         strprintf(_("Line: %d"), linenumber) + "\n\"" + line + "\"";
                streamConfig.close();
                return false;
            }
        }

        add(name, txHash, outputIndex);
    }

    streamConfig.close();
    return true;
}

bool CForge::CForgeItem::castOutputIndex(int &n) const
{
    try {
        n = std::stoi(outputIndex);
    } catch (const std::exception& e) {
        LogPrintf("%s: %s on getOutputIndex\n", __func__, e.what());
        return false;
    }

    return true;
}